package naming_client

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

var (
	addr = flag.String("addr", "localhost:50051", "address")
	// name   = flag.String("str", str, "String for convert")
	// s_type = flag.Int("type", int(type_default), "Type convert")
)

func Convert(lex string, opt int) (string, error) {

	flag.Parse()

	conn, err := grpc.NewClient(*addr, grpc.WithTransportCredentials(insecure.NewCredentials()))
	if err != nil {
		log.Fatalf(" %v", err)
		return "", err
	}
	defer conn.Close()
	client := pb.NewNamingServiceClient(conn)

	ctx, cancel := context.WithTimeout(context.Background(), time.Second)
	defer cancel()

	resp, err := client.Convert(ctx, &pb.Request{Lex: lex, Type: pb.Types(opt)})
	if err != nil {
		log.Fatalf(" %v", err)
		return "", err
	}

	return fmt.Sprintf("Response is: %v", resp.GetLex()), nil
}
