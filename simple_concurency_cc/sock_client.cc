#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  std::cout << "Client start\n";

  constexpr char *SERV_IP = "127.0.0.1";
  constexpr int PORT = 8080;
  constexpr char *msg = "Meow UwU";
  constexpr size_t BUF_SIZ = 4096;

  int client_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (client_fd == -1) {
    std::cerr << "Err in creating socket\n";
    return 1;
  }

  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  if (inet_pton(AF_INET, SERV_IP, &server_addr.sin_addr) <= 0) {
    std::cerr << "Err in pton\n";
    return 1;
  }

  if (connect(client_fd, (const sockaddr *)&server_addr, sizeof(server_addr)) ==
      -1) {
    std::cerr << "Err in connect\n";
    return 1;
  }

  std::cout << "Client done to write!" << std::endl;

  if (write(client_fd, msg, strlen(msg)) == -1) {
    std::cerr << "Err in write\n";
    return 1;
  }

  std::cout << "Client write done and go out!" << std::endl;

  close(client_fd);

  return 0;
}
