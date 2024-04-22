package main

import (
	"fmt"
	"web-crawler/crawler"
)

func main() {
	/*conn, err := data.Connect()
	if err != nil {
		return
	}
	websiteUrls, err := conn.GetWebsiteUrls()

	if err != nil {
		return
	}*/
	websiteUrls := []string{"https://apple.com", "https://www.apple.com/at/", "https://mcshark.at"}
	numWorkers := 100 //number of workers, prod: 100, test: 1
	err := crawler.Crawl(websiteUrls, numWorkers)
	if err != nil {
		fmt.Println("Error in main at crawler:", err)
		return
	}
}
