package main

import (
	"context"
	"flag"
	"fmt"
	"log"
	"net"

	pb "github.com/lemito/namingify/proto/naming"
	"google.golang.org/grpc"
)

var (
	port = flag.Int("port", 50051, "The server port")
)

type server struct {
	pb.UnimplementedNamingServiceServer
}

func (s *server) Convert(_ context.Context, in *pb.Request) (*pb.Response, error) {

	log.Printf("Got %s\n", in.Lex)
	return &pb.Response{Lex: fmt.Sprintf("meow UwU this is annswer from s2c and you recieve %s", in.Lex)}, nil
}

func main() {
	flag.Parse()

	conn, err := net.Listen("tcp", fmt.Sprintf(":%d", *port))
	if err != nil {
		log.Fatalf("failed to listen: %v", err)
	}
	serv := grpc.NewServer()
	pb.RegisterNamingServiceServer(serv, &server{})
	log.Printf("Start %v", conn.Addr().String())
	err = serv.Serve(conn)
	if err != nil {
		panic(err)
	}
}
