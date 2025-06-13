#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  std::cout << "Server start\n";

  constexpr int PORT = 8080;
  constexpr size_t BUF_SIZ = 4096;
  static std::byte BUF[BUF_SIZ];

  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (-1 == server_fd) {
    std::cerr << "Err in creating socket\n";
    return 1;
  }

  sockaddr_in server_addr{};
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  if (bind(server_fd, (sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    std::cerr << "Err in bind\n";
    return 1;
  }

  if (listen(server_fd, 1) == -1) {
    std::cerr << "Err in listen\n";
    return 1;
  }

  std::cout << "Server done on port = " << PORT << std::endl;

  sockaddr_in client_addr{};
  socklen_t client_addr_len = sizeof(client_addr);
  int client_fd = accept(server_fd, (sockaddr *)&client_addr, &client_addr_len);
  if (client_fd == -1) {
    std::cerr << "Err in client accept\n";
    return 1;
  }

  std::cout << "Client connected" << std::endl;

  ssize_t bread = read(client_fd, BUF, BUF_SIZ - 1);
  if (-1 == bread) {
    std::cerr << "Err in reading from client\n";
    return 1;
  }

  BUF[bread] = std::byte('0');
  std::cout << "Server got this msg = " << (char *)BUF << std::endl;

  close(client_fd);
  close(server_fd);

  return 0;
}
