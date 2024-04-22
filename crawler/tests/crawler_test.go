package tests

import (
	"net/http"
	"net/http/httptest"
	"testing"
	"web-crawler/crawler"
)

func TestCleanContent(t *testing.T) {
	input := "This is a: test, of \"the\" 'cleaning' function."
	expected := "this is a test of the cleaning function"

	cleaned := crawler.CleanContent(input)

	if cleaned != expected {
		t.Errorf("cleanContent did not produce the expected result. Got: %s, Want: %s", cleaned, expected)
	}
}

func TestExtractStrings(t *testing.T) {
	input := "Line 1\n\n  Line 2\nLine 3\n"
	expected := "Line 1 Line 2 Line 3"

	extracted := crawler.ExtractStrings(input)

	if extracted != expected {
		t.Errorf("extractStrings did not produce the expected result. Got: %s, Want: %s", extracted, expected)
	}
}

func TestCrawlAndFetch(t *testing.T) {
	// start a local http server to handle requests during testing
	server := httptest.NewServer(http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
		_, err := w.Write([]byte("whats good"))
		if err != nil {
			t.Errorf("response writer did not work. Got: %s", err)
		}
	}))
	defer server.Close()

	err := crawler.Crawl([]string{"dasdasd"}, 1)
	if err == nil {
		t.Errorf("crawl did not return an error")
	}

	/*
		err = crawler.Crawl([]string{server.URL}, 1)
		if err != nil {
			t.Fail()
		}
	*/
}
