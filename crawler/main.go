package main

import (
	"fmt"
	"web-crawler/crawler"
)

func main() {
	urls := []string{"https://apple.com", "https://www.kapsch.net", "https://www.microsoft.com", "https://www.google.com", "https://www.amazon.de"}

	numWorkers := 1 //number of workers, prod: 1 for 10 urls, test: 1 for 1 url
	err := crawler.Crawl(urls, numWorkers)
	if err != nil {
		fmt.Println("Error in main at crawler:", err)
		return
	}
}
