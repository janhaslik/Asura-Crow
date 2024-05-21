package crawler

import (
	"bytes"
	"encoding/json"
	"errors"
	"fmt"
	"golang.org/x/net/html"
	"io"
	"net/http"
	"strings"
	"sync"
)

type document struct {
	Url     string `json:"url"`
	Content string `json:"content"`
}

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
	urlChan := make(chan string, len(urls))

	// Fill the channel with URLs
	for _, url := range urls {
		urlChan <- url
	}
	close(urlChan)

	var wg sync.WaitGroup
	wg.Add(numWorkers)

	type crawlErr struct {
		err    error
		url    string
		worker int
	}
	errChan := make(chan crawlErr, numWorkers)

	// Start worker goroutines
	for i := 1; i <= numWorkers; i++ {
		go func(workerID int) {
			defer wg.Done()
			for url := range urlChan {
				fmt.Printf("Crawling %s\n", url)
				err := fetch(url)
				if err != nil {
					errChan <- crawlErr{err, url, workerID}
				} else {
					fmt.Printf("Successfully Crawled %s and sent document to indexer server, channel worker: %d\n", url, workerID)
				}
			}
		}(i)
	}

	// Close error channel after all workers are done
	go func() {
		wg.Wait()
		close(errChan)
	}()

	// Handle errors from workers
	for channel := range errChan {
		if channel.err != nil {
			fmt.Printf("Error fetching url: %s, channel worker: %d, error: %s\n", channel.url, channel.worker, channel.err)
		}
	}

	return nil
}

/**
 * @brief Fetches the content of a URL and sends it to the indexer server.
 *
 * @param url The URL to fetch.
 * @return error An error if any issue occurs during fetching or sending.
 */
func fetch(url string) error {
	res, err := http.Get(url)
	if err != nil {
		fmt.Println(err)
		return err
	}
	defer func(Body io.ReadCloser) {
		err := Body.Close()
		if err != nil {
			fmt.Println("Error closing body:", err)
		}
	}(res.Body)

	tokenizer := html.NewTokenizer(res.Body)
	content := ""
	appendHTML := true

	// Tokenize HTML content
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
			req.Header.Set("Host", "localhost:7002")

			fmt.Println("Sending request to indexer")

			client := &http.Client{}
			res, err := client.Do(req)
			if err != nil {
				fmt.Printf("Failed to request indexer server: %s\n", err)
				return err
			}
			defer func(Body io.ReadCloser) {
				err := Body.Close()
				if err != nil {
					fmt.Println("Error closing body:", err)
				}
			}(res.Body)

			// Check the response status code
			if res.StatusCode != http.StatusOK {
				fmt.Printf("Error: Status %s\n", res.Status)
				return errors.New("non-OK HTTP status")
			}
			return nil

		case html.StartTagToken, html.SelfClosingTagToken:
			token := tokenizer.Token()
			tagName := token.Data
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
 * @brief Cleans the content by removing blacklisted characters and turning it into lowercase.
 *
 * @param content The raw content to clean.
 * @return string The cleaned content.
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
 *
 * @param content The raw content to extract strings from.
 * @return string The extracted content.
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
