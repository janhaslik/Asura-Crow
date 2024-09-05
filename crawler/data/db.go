package data

import (
	"context"
	"go.mongodb.org/mongo-driver/bson"
	"go.mongodb.org/mongo-driver/mongo"
	"go.mongodb.org/mongo-driver/mongo/options"
	"time"
)

const connectionString string = "mongodb://root:1234@localhost:27017"

// COLLECTION represents a MongoDB collection.
type COLLECTION struct {
	collection *mongo.Collection
}

/**
 * @brief Connect establishes a connection to the MongoDB database.
 *
 * @return *COLLECTION: A pointer to the COLLECTION object representing the MongoDB collection.
 * @return error: An error, if any.
 */
func Connect() (collection *COLLECTION, err error) {
	ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()

	// Connect to the MongoDB database
	client, err := mongo.Connect(ctx, options.Client().ApplyURI(connectionString))
	if err != nil {
		return nil, err
	}

	// Access the "AsuraCrow_DB" database and the "websites" collection
	database := client.Database("AsuraCrow_DB")
	websites := database.Collection("websites")

	return &COLLECTION{collection: websites}, nil
}

/**
 * @brief GetWebsiteUrls retrieves website URLs from the MongoDB collection.
 *
 * @param collection *COLLECTION: A pointer to the COLLECTION object representing the MongoDB collection.
 *
 * @return []string: A slice containing the retrieved website URLs.
 * @return error: An error, if any.
 */
func (collection *COLLECTION) GetWebsiteUrls() ([]string, error) {
	ctx, cancel := context.WithTimeout(context.Background(), 30*time.Second)
	defer cancel()

	// Query the MongoDB collection for all documents
	cur, err := collection.collection.Find(context.Background(), bson.M{})
	if err != nil {
		return nil, err
	}

	defer func(cur *mongo.Cursor, ctx context.Context) {
		err := cur.Close(ctx)
		if err != nil {
			// Handle error
		}
	}(cur, ctx)

	var websitesUrls []string
	// Iterate through the query results
	for cur.Next(ctx) {
		var result bson.M
		err := cur.Decode(&result)
		if err != nil {
			return nil, err
		}

		// Extract the "url" field from each document and append it to the slice
		websitesUrls = append(websitesUrls, result["url"].(string))
	}

	return websitesUrls, nil
}

func (collection *COLLECTION) InsertUrl(url string) error {
	ctx, cancel := context.WithTimeout(context.Background(), 30*time.Second)
	defer cancel()

	doc := bson.D{{Key: "url", Value: url}}

	_, err := collection.collection.InsertOne(ctx, doc)

	if err != nil {
		return err
	}

	return nil
}
