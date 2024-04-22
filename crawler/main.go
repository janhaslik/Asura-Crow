package main

import (
	"fmt"
	"web-crawler/crawler"
	"web-crawler/data"
)

func main() {
	conn, _ := data.Connect()

	websiteUrls, err := conn.GetWebsiteUrls()
	if err != nil {
		return
	}

	numWorkers := 100 //number of workers, prod: 100, test: 1
	err = crawler.Crawl(websiteUrls, numWorkers)
	if err != nil {
		fmt.Println("Error in main at crawler:", err)
		return
	}
}
