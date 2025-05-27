package main

import (
	"context"
	"flag"
	"fmt"
	"log"
	"time"

	pb "github.com/lemito/namingify/proto/naming"
	"google.golang.org/grpc"
	"google.golang.org/grpc/credentials/insecure"
)

const str = "snail_case_string"
const type_default = pb.Methods_Snail2Camel

var (
	addr   = flag.String("addr", "localhost:50051", "address")
	name   = flag.String("str", str, "String for convert")
	s_type = flag.Int("type", int(type_default), "Type convert")
)

func main() {

	flag.Parse()

	conn, err := grpc.NewClient(*addr, grpc.WithTransportCredentials(insecure.NewCredentials()))
	if err != nil {
		log.Fatalf(" %v", err)
	}
	defer conn.Close()
	client := pb.NewNamingServiceClient(conn)

	ctx, cancel := context.WithTimeout(context.Background(), time.Second)
	defer cancel()

	resp, err := client.Convert(ctx, &pb.Request{Type: pb.Methods(*s_type), Lex: *name})
	if err != nil {
		log.Fatalf(" %v", err)
	}

	fmt.Printf("Response is: %v", resp.GetLex())
}
