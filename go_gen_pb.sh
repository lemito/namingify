protoc --proto_path=proto \
       --go_out=. --go_opt=module=github.com/lemito/namingify \
       --go-grpc_out=. --go-grpc_opt=module=github.com/lemito/namingify \
       proto/naming/naming.proto

protoc -I . --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` proto/naming/naming.proto
protoc -I . --cpp_out=. proto/naming/naming.proto
