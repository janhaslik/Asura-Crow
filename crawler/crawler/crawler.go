package crawler

import (
	"bytes"
	"encoding/json"
	"errors"
	"fmt"
	"golang.org/x/net/html"
	"net/http"
	"net/url"
	"strings"
	"sync"
	"web-crawler/data"
)

type document struct {
	Url     string `json:"url"`
	Content string `json:"content"`
}

// Create a global map to track visited URLs and a mutex to ensure thread-safe access
var visitedUrls = make(map[string]bool)
var mu sync.Mutex

/**
 * @brief Crawl multiple URLs concurrently using a specified number of workers.
 *
 * @param urls A slice of URLs to crawl.
 * @param numWorkers The number of concurrent workers to use for crawling.
 * @return error An error if any issue occurs during crawling.
 */
func Crawl(urls []string, numWorkers int) error {
	if len(urls) == 0 {
		return errors.New("no urls provided")
	}

	urlChan := make(chan string, len(urls)) // Channel for URLs to process
	errChan := make(chan error, numWorkers) // Channel for errors

	// Fill the channel with initial URLs
	for _, u := range urls {
		urlChan <- u
	}

	var wg sync.WaitGroup
	wg.Add(numWorkers)

	// Start worker goroutines
	for i := 0; i < numWorkers; i++ {
		go func(workerID int) {
			defer wg.Done()
			for url := range urlChan {
				fmt.Printf("Worker %d crawling: %s\n", workerID, url)
				err := fetch(url, urlChan)
				if err != nil {
					errChan <- fmt.Errorf("worker %d: %w", workerID, err)
				} else {
					fmt.Printf("Worker %d successfully crawled: %s\n", workerID, url)
				}
			}
		}(i)
	}

	// Close the error channel after all workers are done
	go func() {
		wg.Wait()
		close(errChan)
	}()

	// Handle errors from workers
	for err := range errChan {
		if err != nil {
			fmt.Printf("Error: %s\n", err)
		}
	}

	return nil
}

/**
 * @brief Fetches the content of a URL and sends it to the indexer server.
 * Discovered links are passed to other workers through the urlChan.
 *
 * @param url The URL to fetch.
 * @param urlChan The channel to send newly discovered URLs.
 * @return error An error if any issue occurs during fetching or sending.
 */
func fetch(url string, urlChan chan<- string) error {
	// Ensure the URL has not been visited before
	mu.Lock()
	if visitedUrls[url] {
		mu.Unlock()
		fmt.Printf("Skipping already visited URL: %s\n", url)
		return nil
	}
	visitedUrls[url] = true // Mark URL as visited
	mu.Unlock()

	res, err := http.Get(url)
	if err != nil {
		return err
	}
	defer res.Body.Close()

	// Parse the HTML content
	tokenizer := html.NewTokenizer(res.Body)
	content := ""
	appendHTML := true

	for {
		tokenType := tokenizer.Next()
		switch tokenType {

		case html.ErrorToken:
			content = ExtractStrings(content)
			content = CleanContent(content)

			// Create a document object to send to the indexer server
			doc := document{
				Url:     url,
				Content: content,
			}
			docBytes, err := json.Marshal(doc)
			if err != nil {
				return err
			}

			req, err := http.NewRequest("POST", "http://localhost:7001/index", bytes.NewBuffer(docBytes))
			if err != nil {
				return err
			}
			req.Header.Set("Content-Type", "application/json")

			client := &http.Client{}
			res, err := client.Do(req)
			if err != nil {
				return fmt.Errorf("failed to request indexer server: %w", err)
			}
			defer res.Body.Close()

			if res.StatusCode != http.StatusOK {
				return fmt.Errorf("non-OK HTTP status: %s", res.Status)
			}
			return nil

		case html.StartTagToken, html.SelfClosingTagToken:
			token := tokenizer.Token()
			tagName := token.Data

			if tagName == "a" {
				for _, attr := range token.Attr {
					if attr.Key == "href" {
						href := attr.Val
						resolvedUrl, err := resolveUrl(href, url)
						if err != nil {
							fmt.Printf("Error resolving URL: %s\n", err)
							continue
						}
						if resolvedUrl == "" {
							continue
						}

						conn, err := data.Connect()

						if err != nil {
							fmt.Printf("Error connecting to database: %s\n", err)
						}

						err = conn.InsertUrl(resolvedUrl)

						if err != nil {
							fmt.Printf("Error inserting new URL %s: %s\n", resolvedUrl, err)
						}

						// Send discovered URL to the channel for workers to process
						go func() {
							urlChan <- resolvedUrl
						}()
					}
				}
			}

			if tagName == "script" || tagName == "style" {
				appendHTML = false
			} else {
				appendHTML = true
			}

		case html.TextToken:
			if appendHTML {
				text := string(tokenizer.Text())
				content += text
			}
		}
	}
}

/**
 * @brief Resolves a relative or protocol-less URL to an absolute URL.
 *
 * @param href The URL found in the <a> tag.
 * @param baseUrl The base URL of the current page.
 * @return string The resolved absolute URL.
 */
func resolveUrl(href string, baseUrl string) (string, error) {
	if strings.HasPrefix(href, "#") {
		return "", nil
	}

	baseParsed, err := url.Parse(baseUrl)
	if err != nil {
		return "", err
	}

	hrefParsed, err := url.Parse(href)
	if err != nil {
		return "", err
	}

	// If the URL is relative, resolve it against the base URL
	if !hrefParsed.IsAbs() {
		//return baseParsed.ResolveReference(hrefParsed).String(), nil
		// for now skip relative urls
		return "", nil
	}

	// If the URL starts with "//", add the scheme from the base URL (e.g., https:)
	if strings.HasPrefix(href, "//") {
		return baseParsed.Scheme + ":" + href, nil
	}

	return href, nil
}

/**
 * @brief Cleans the content by removing blacklisted characters and turning it into lowercase.
 */
func CleanContent(content string) string {
	blacklist := map[string]bool{"and": true, ";": true, ":": true, ".": true, "{": true, "}": true, "[": true, "]": true, "\\": true, "%": true, "$": true, ",": true, "'": true, "<": true, ">": true, "!": true, "\"": true, "with": true, "or": true, "-": true}

	words := strings.Split(content, " ")
	var cleaned []string
	for _, word := range words {
		if blacklist[word] {
			continue
		}
		var cleanedWord strings.Builder
		for _, char := range word {
			if !blacklist[string(char)] {
				cleanedWord.WriteRune(char)
			}
		}
		cleaned = append(cleaned, strings.ToLower(cleanedWord.String()))
	}
	return strings.Join(cleaned, " ")
}

/**
 * @brief Extracts strings by removing unnecessary whitespace characters from the content.
 */
func ExtractStrings(content string) string {
	lines := strings.Split(content, "\n")
	var result []string
	for _, line := range lines {
		trimmed := strings.TrimSpace(line)
		if trimmed != "" {
			result = append(result, trimmed)
		}
	}
	return strings.Join(result, " ")
}
