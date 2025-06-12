#include <iostream>
#include <memory>
#include <string>
#include <string_view>

#include "absl/flags/flag.h"
#include "absl/flags/parse.h"
#include "absl/strings/str_format.h"

#include <grpcpp/ext/proto_server_reflection_plugin.h>
#include <grpcpp/grpcpp.h>
#include <grpcpp/health_check_service_interface.h>

#include "proto/naming/naming.grpc.pb.h"
#include "proto/naming/naming.pb.h"

#include "tables.h"
#include <sstream>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using naming::NamingService;
using naming::Request;
using naming::Response;
using naming::Types;

ABSL_FLAG(uint16_t, port, 50051, "Server port for the service");

class NamingServiceImpl final : public NamingService::Service {
  Status Convert(ServerContext *context, const Request *request,
                 Response *response) {
    std::ostringstream res;
    std::string_view in(request->lex());
    char fakeStr[] = {0, 0};

    switch (request->type()) {
    case Types::GOST_1687671: {
      for (const auto &elem : in) {
        fakeStr[0] = elem;
        std::string_view key(fakeStr);
        if (table_GOST_1687671.end() != table_GOST_1687671.find(key)) {
          res << table_GOST_1687671.find(key).operator*().second;
        } else {
          res << elem;
        }
      }
    } break;
    case Types::ISO_91995: {
      for (const auto &elem : in) {
        fakeStr[0] = elem;
        std::string_view key(fakeStr);
        if (table_ISO_91995.end() != table_ISO_91995.find(key)) {
          res << table_ISO_91995.find(key).operator*().second;
        } else {
          res << elem;
        }
      }
    } break;
    case Types::US_GOSDEP: {
      for (const auto &elem : in) {
        fakeStr[0] = elem;
        std::string_view key(fakeStr);
        if (table_US_GOSDEP.end() != table_US_GOSDEP.find(key)) {
          res << table_US_GOSDEP.find(key).operator*().second;
        } else {
          res << elem;
        }
      }
    } break;
    case Types::RU_ForeignPass: {
      for (const auto &elem : in) {
        fakeStr[0] = elem;
        std::string_view key(fakeStr);
        if (table_RU_ForeignPass.end() != table_RU_ForeignPass.find(key)) {
          res << table_RU_ForeignPass.find(key).operator*().second;
        } else {
          res << elem;
        }
      }
    } break;
    default: {
      return Status::CANCELLED;
    } break;
    }
    response->set_lex(res.str());
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
