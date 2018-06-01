#include <iostream>
#include <vector>
#include <cstring> //strlen

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

#define PASSLEN 4

static void ExitErr(std::string source, int err, int line) {
  std::cout << source << ": '" << strerror(err) << "' error on line " << line << std::endl;
  exit(-1);
}

static void encrypt(char* passwd, char* hash) {
  // std::cout << "psswd:" << passwd << std::endl;
  char salt[] = "$1";
  const char *const seedchars =
    "qwertyuiopasdfghjklzxcvbnm"
    "QWERTYUIOPLKJHGFDSAZXCVBNM"
    "1234567890";
  const int num_chars = strlen(seedchars);
  unsigned long seed[2];

  /* Generate a (not very) random seed */
  seed[0] = time(NULL);
  seed[1] = getpid() ^ (seed[0] >> 14 & 0x30000);

  /* Turn it into printable characters from ‘seedchars’. */
  for (int i = 0; i < 2; i++)
    salt[i] = seedchars[(seed[i/5] >> (i%5)*6) & 0x3f];

  strcpy(hash, crypt(passwd, salt));
  // std::cout << "hash: " << hash << std::endl;
}

static void htonm(Message& m) {
  m.num_passwds = htonl(m.num_passwds);
  m.port = htons(m.port);
}

static void ntohm(Message& m) {
  m.num_passwds = ntohl(m.num_passwds);
  m.port = ntohs(m.port);
}

static void send(Message& m) {
  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    ExitErr("Open sending socket", errno, __LINE__);
  }

  int ttl = 1;
  if (setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_TTL, (void*) &ttl, sizeof(ttl)) < 0) {
    ExitErr("Set Timeout", errno, __LINE__);
  }

  struct sockaddr_in multicastAddr;
  memset(&multicastAddr, 0, sizeof(multicastAddr));
  multicastAddr.sin_family = AF_INET;
  multicastAddr.sin_addr.s_addr = get_multicast_address();
  multicastAddr.sin_port = htons(get_multicast_port());

  htonm(m);
  int n = sendto(sockfd, &m, sizeof(m), 0, (struct sockaddr*) &multicastAddr, sizeof(multicastAddr));
  if (n < 0) {
    ExitErr("write", errno, __LINE__);
  }
  printf("Sent!\n");
  close(sockfd);
}


int main(int argc, char** argv) {
  if (argc < 3) {
    std::cerr << "Usage: ./server <num psswrds> password\n";
    exit(-1);
  }
  uint PORT = get_unicast_port();
  int num_passwords = atoi(argv[1]);
  char hash[13];
  encrypt(argv[2], hash);
  char thishost[128];
  gethostname(thishost, 128);
  std::cout << "thishost: " << thishost << std::endl;
  string cruzid = "movenden";
  Message message;
  memset(&message, 0, sizeof(message));
  strcpy(message.hostname, thishost);
  strcpy(message.cruzid, "movenden");
  message.port = PORT;

  for (int i = 0; i < num_passwords; i++) {
    strcpy(message.passwds[message.num_passwds++], hash);
  }
  // strcpy(message.passwds[message.num_passwds - 1], "lmao");

  send(message);

  int sockrecv = socket(AF_INET, SOCK_STREAM, 0);
  if (sockrecv < 0) {ExitErr("sockrecv", errno, __LINE__); }

  struct sockaddr_in server_addr;
  struct sockaddr_in crack_addr;
  memset(&crack_addr, 0, sizeof(crack_addr));
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);
  if (bind(sockrecv, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) ExitErr("bind",errno,__LINE__);
  listen(sockrecv,5);
  std::cout << "I am listening on port " << PORT << std::endl;

  socklen_t len = sizeof(crack_addr);
  int newsock = accept(sockrecv, (struct sockaddr*)&crack_addr, &len);
  if (newsock < 0) ExitErr("newsock", errno, __LINE__);
  memset(&message, 0, sizeof(message));
  std::cout << "Accepted connection. Waiting for response\n";

  if (read(newsock, &message, sizeof(message)) < 0) ExitErr("read",errno,__LINE__);

  ntohm(message);
  for (int i = 0; i < num_passwords; i++) {
    std::cout << message.passwds[i] << std::endl;
  }

  close(sockrecv);
  close(newsock);
}
