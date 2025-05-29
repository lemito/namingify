#include <iostream>
#include <memory>
#include <string>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_format.h"

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "proto/naming/naming.grpc.pb.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using naming::Methods;
using naming::NamingService;
using naming::Request;
using naming::Response;

ABSL_FLAG(uint16_t, port, 50051, "Server port for the service");

class NamingServiceImpl final : public NamingService::Service {
  Status Convert(ServerContext *context, const Request *request,
                 Response *response) {
    switch (request->type()) {
    case naming::Methods::Camel2Snail: {
      auto resp = absl::StrFormat("meow Uwu %s, but was %s!", "Camel2Snail",
                                  request->lex());
      response->set_lex(resp);
    } break;
    case naming::Methods::Snail2Camel: {
      auto resp = absl::StrFormat("meow Uwu %s, but was %s!", "Snail2Camel",
                                  request->lex());
      response->set_lex(resp);
    }
    default: {
      return Status::CANCELLED;
    } break;
    }
    return Status::OK;
  }
};

void RunServer(uint16_t port) {
  std::string server_address = absl::StrFormat("0.0.0.0:%d", port);
  NamingServiceImpl service;

  grpc::EnableDefaultHealthCheckService(true);
  grpc::reflection::InitProtoReflectionServerBuilderPlugin();
  ServerBuilder builder;

  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());

  builder.RegisterService(&service);

  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Server started " << server_address << std::endl;

  server->Wait();
}

int main(int argc, char **argv) {
  std::cout << "meow\n";
  absl::ParseCommandLine(argc, argv);
  RunServer(absl::GetFlag(FLAGS_port));
  return 0;
}