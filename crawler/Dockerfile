from golang:1.22

workdir /

copy go.mod go.sum ./

run go mod download && go mod verify

copy . .

run go build -o web-crawler

cmd ["./web-crawler"]