package data

import (
	"context"
	"go.mongodb.org/mongo-driver/bson"
	"go.mongodb.org/mongo-driver/mongo"
	"go.mongodb.org/mongo-driver/mongo/options"
	"time"
	_ "time"

	_ "go.mongodb.org/mongo-driver/mongo"
	_ "go.mongodb.org/mongo-driver/mongo/options"
	_ "go.mongodb.org/mongo-driver/mongo/readpref"
)

var connectionString string = "mongodb://root:1234@localhost:27017"

type COLLECTION struct {
	collection *mongo.Collection
}

func Connect() (collection *COLLECTION, err error) {
	ctx, cancel := context.WithTimeout(context.Background(), 10*time.Second)
	defer cancel()
	client, err := mongo.Connect(ctx, options.Client().ApplyURI(connectionString))

	if err != nil {
		return nil, err
	}

	database := client.Database("AsuraCrow_DB")
	websites := database.Collection("websites")

	return &COLLECTION{collection: websites}, nil
}

func (collection *COLLECTION) GetWebsiteUrls() ([]string, error) {
	ctx, cancel := context.WithTimeout(context.Background(), 30*time.Second)
	defer cancel()

	cur, err := collection.collection.Find(context.Background(), bson.M{})
	if err != nil {
		return nil, err
	}

	defer func(cur *mongo.Cursor, ctx context.Context) {
		err := cur.Close(ctx)
		if err != nil {

		}
	}(cur, ctx)

	var websitesUrls []string
	for cur.Next(ctx) {
		var result bson.M
		err := cur.Decode(&result)
		if err != nil {
			return nil, err
		}

		websitesUrls = append(websitesUrls, result["url"].(string))
	}

	return websitesUrls, nil
}
