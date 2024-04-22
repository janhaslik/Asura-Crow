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

func Crawl(urls []string, numWorkers int) error {
	if len(urls) == 0 {
		return errors.New("no urls provided")
	}
	urlChan := make(chan string, len(urls))

	//fill the channel
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

	for i := 1; i <= numWorkers; i++ {
		go func() {
			defer wg.Done()
			for url := range urlChan {
				err := fetch(url)

				if err != nil {
					errChan <- crawlErr{err, url, i}
					return
				} else {
					fmt.Printf("Successfully Crawled %s and sent document to indexer server, channel worker: %d\n", url, i)
				}
			}
		}()
	}

	go func() {
		wg.Wait()
		close(errChan)
	}()

	for channel := range errChan {
		if channel.err != nil {
			fmt.Printf("Error fetching url: %s, channel worker: %d, error: %s\n", channel.url, channel.worker, channel.err)
			return channel.err
		}
	}

	return nil
}

func fetch(url string) error {
	res, err := http.Get(url)
	if err != nil {
		fmt.Println(err)
		return err
	}
	defer func(Body io.ReadCloser) {
		err := Body.Close()
		if err != nil {

		}
	}(res.Body)

	tokenizer := html.NewTokenizer(res.Body)
	content := ""
	appendHTML := true

	for {
		tokenType := tokenizer.Next()

		switch tokenType {
		case html.ErrorToken:
			content = ExtractStrings(content)
			content = CleanContent(content)

			// Create a document object to pass to the C function
			doc := document{
				Url:     url,
				Content: content,
			}

			var docBytes, err = json.Marshal(doc)

			if err != nil {
				return err
			}
			req, err := http.NewRequest("POST", "http://localhost:7001/index", bytes.NewBuffer(docBytes))

			req.Header.Set("Content-Type", "text/plain")
			req.Header.Set("Host", "localhost:7002")

			client := &http.Client{}

			res, err := client.Do(req)
			if err != nil {
				fmt.Errorf("Failed to request indexer server %s", err)
				return err
			}

			defer func(Body io.ReadCloser) {
				err := Body.Close()
				if err != nil {
					fmt.Println("Error closing body:", err)
					return
				}
			}(res.Body)

			// check the response status code
			if res.StatusCode != http.StatusOK {
				fmt.Printf("Error: Status %s\n", res.Status)
				return err
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

// removes all blacklisted characters, and turns the content into lowercase
func CleanContent(content string) string {
	blacklist := map[string]bool{"and": true, ";": true, ":": true, ".": true, "{": true, "}": true, "[": true, "]": true, "\\": true, "%": true, "$": true, ",": true, "'": true, "<": true, ">": true, "!": true, "\"": true, "with": true, "or": true, "-	": true}

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
	result := strings.Join(cleaned, " ")
	return result
}

// extractStrings removes unnecessary whitespace characters from content
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
