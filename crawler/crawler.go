package main

import "C"
import (
	"bytes"
	"encoding/json"
	"fmt"
	"golang.org/x/net/html"
	"io"
	"net/http"
	"strings"
)

type document struct {
	Url     string `json:"url"`
	Content string `json:"content"`
}

func crawl(url string) {
	fetch(url)
}

func fetch(url string) {
	res, err := http.Get(url)
	if err != nil {
		fmt.Println(err)
		return
	}
	defer res.Body.Close()

	tokenizer := html.NewTokenizer(res.Body)
	content := ""
	appendHTML := true

	for {
		tokenType := tokenizer.Next()

		switch tokenType {
		case html.ErrorToken:
			content = extractStrings(content)
			content = cleanContent(content)
			fmt.Println(content)
			// Create a document object to pass to the C function
			doc := document{
				Url:     url,
				Content: content,
			}

			var docBytes, err = json.Marshal(doc)

			if err != nil {
				return
			}
			req, err := http.NewRequest("POST", "http://localhost:7001/index", bytes.NewBuffer(docBytes))

			req.Header.Set("Content-Type", "text/plain")
			req.Header.Set("Host", "localhost:7002")

			client := &http.Client{}

			res, err := client.Do(req)
			if err != nil {
				return
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
				return
			}
			return

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

func cleanContent(content string) string {
	blacklist := map[string]bool{"and": true, ":": true, ".": true, ",": true}

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
func extractStrings(content string) string {
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
