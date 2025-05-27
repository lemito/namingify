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
	switch in.Type {
	case pb.Methods_Camel2Snail:
		{
			log.Printf("Got %s\n", in.Lex)
			return &pb.Response{Lex: fmt.Sprintln("meow UwU this is annswer from c2s")}, nil
		}
	case pb.Methods_Snail2Camel:
		{
			log.Printf("Got %s\n", in.Lex)
			return &pb.Response{Lex: fmt.Sprintln("meow UwU this is annswer from s2c")}, nil
		}
	default:
		{
			log.Printf("Got %s\n", in.Lex)
			return nil, fmt.Errorf("no such case - %v", in.Type)
		}
	}
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
