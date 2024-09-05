package main

import (
	"fmt"
	"sync"
	"time"
	"web-crawler/crawler"
	"web-crawler/data"
)

/**
 * @brief Main function of the Asura Crow crawler to connect to the database and start the web crawling process every 5 minutes.
 */
func main() {
	// Ticker to trigger the crawling process every 5 minutes
	ticker := time.NewTicker(time.Second * 5)
	defer ticker.Stop()

	var wg sync.WaitGroup
	wg.Add(1) // Dummy entry to the wait group to prevent main from exiting

	// Goroutine to perform crawling at regular intervals
	go func() {
		defer wg.Done()
		for range ticker.C {
			performCrawling()
		}
	}()

	// Block main from exiting
	wg.Wait()
}

/**
 * @brief Function to perform the web crawling process.
 */
func performCrawling() {
	// Connect to the database
	conn, err := data.Connect()
	if err != nil {
		fmt.Println("Error connecting to database:", err)
		return
	}

	// Get website URLs from the database
	websiteUrls, err := conn.GetWebsiteUrls()
	if err != nil {
		fmt.Println("Error retrieving website URLs:", err)
		return
	}

	// Number of concurrent workers for crawling (prod: 100, test: 1)
	numWorkers := 100

	// Start crawling the retrieved URLs
	err = crawler.Crawl(websiteUrls, numWorkers)
	if err != nil {
		fmt.Println("Error in crawler:", err)
	}
}
