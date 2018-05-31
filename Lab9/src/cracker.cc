#include <iostream>
#include <vector>
#include <cstring> //strlen
#include <thread>
#include <atomic>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "crack.h"

using std::string;
using std::vector;
using std::thread;

static void ExitErr(std::string source, int err, int line) {
  std::cout << source << " error: '" << strerror(err) << "' on line " << line << std::endl;
  exit(-1);
}

static void htonm(Message& m) {
  m.num_passwds = htonl(m.num_passwds);
  m.port = htons(m.port);
}

static void ntohm(Message& m) {
  m.num_passwds = ntohl(m.num_passwds);
  m.port = ntohs(m.port);
}

static void recv(Message& msg) {
  // int port = atoi(argv[1]);
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    ExitErr("Client", errno, __LINE__);
  }

  //create socket for multicast listening
  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(get_multicast_port());
  if (bind(sockfd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
    ExitErr("Binding port", errno, __LINE__);
  }

  struct ip_mreq multicastRequest;
  memset(&multicastRequest, 0, sizeof(multicastRequest));
  multicastRequest.imr_multiaddr.s_addr = get_multicast_address();
  multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);
  if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void*) &multicastRequest, sizeof(multicastRequest)) < 0) {
    ExitErr("setsockopt", errno, __LINE__);
  }
  int n = recvfrom(sockfd, &msg, sizeof(msg), 0, 0, 0);
  if (n < 0) { ExitErr("recv", errno, __LINE__); }
  ntohm(msg);
}

static void send(Message& m) {
  int socksend = socket(AF_INET, SOCK_STREAM, 0);
  if (socksend < 0) { ExitErr("setting socket", errno, __LINE__);}

  struct hostent *server = gethostbyname(m.hostname);
  if (server == NULL) { ExitErr("sendback TCP", errno, __LINE__); }
  std::cout << "hostname: " << m.hostname << std::endl;
  std::cout << "port: " << m.port << std::endl;

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  bcopy((char*)server->h_addr, (char*)&server_addr.sin_addr.s_addr, server->h_length);
  server_addr.sin_port = htons(m.port);

  printf("Try to connect:\nhostname: %s\nport# %d\n", m.hostname, m.port);

  if (connect(socksend, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) ExitErr("Connect", errno, __LINE__);

  if (write(socksend, &m, sizeof(m)) < 0) ExitErr("write back", errno, __LINE__);
}

int main(int argc, char** argv) {
  vector<thread> threads;
  Message msg;

  recv(msg);

  std::atomic<int> active_threads(0);
  char psswds[MAX_HASHES][5];
  for (uint i = 0; i < msg.num_passwds; i++) {
    std::cout << "Cracking " << msg.passwds[i] << std::endl;
    threads.push_back(thread(crack, std::ref(msg.passwds[i]), std::ref(psswds[i])));
  }
  for (thread& t : threads) {
    t.join();
  }
  threads.clear();
  for (uint i = 0; i < msg.num_passwds; i++) {
    std::cout << "packaging " << psswds[i] << std::endl;
    strcpy(msg.passwds[i],psswds[i]);
  }

//--------------  setup tcp connection to return passwords  --------------------------

  send(msg);

}