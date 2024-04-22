package tests

import (
	"context"
	"testing"
	"time"

	"go.mongodb.org/mongo-driver/mongo"
	"go.mongodb.org/mongo-driver/mongo/options"
)

// tests the connection
func Test_Connect(t *testing.T) {
	connectionString := "mongodb://root:1234@localhost:27017"

	ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()
	_, err := mongo.Connect(ctx, options.Client().ApplyURI(connectionString))

	if err != nil {
		t.Fail()
	}
}
