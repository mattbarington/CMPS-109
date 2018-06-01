#include <iostream>
#include <vector>
#include <cstring> //strlen
#include <thread>
#include <atomic>
#include <mutex>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <crypt.h>
#include "crack.h"
// #include "cptl_stl.h"

using std::string;
using std::vector;
using std::thread;

#define PASSLEN 4
#define OLAF 0
#define THOR 1
#define GROL 2
#define GRAC 3

#define MASTER "olaf"


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

static bool iamGraculus() {
  char hostname[32];
  gethostname(hostname, 32);
  if (strcmp(hostname, "graculus") == 0)
    std::cout << "I AM GRACULUS\n";
  return strcmp(hostname, "graculus") == 0;
}

static bool iamGrolliffe() {
  char hostname[32];
  gethostname(hostname, 32);
  if (strcmp(hostname, "grolliffe") == 0)
    std::cout << "I AM GROLLIFFE\n";
  return strcmp(hostname, "grolliffe") == 0;
}

static bool iamOlaf() {
  char hostname[32];
  gethostname(hostname, 32);
  if (strcmp(hostname, "olaf") == 0)
    std::cout << "I AM OLAF\n";
  return strcmp(hostname, "olaf") == 0;
}

static bool iamThor() {
  char hostname[32];
  gethostname(hostname, 32);
  if (strcmp(hostname, "thor") == 0)
    std::cout << "I AM THOR\n";
  return strcmp(hostname, "thor") == 0;
}


static bool iamMaster() {
  char hostname[32];
  gethostname(hostname, 32);
  if (strcmp(hostname, MASTER) == 0)
    std::cout << "I AM Master\n";
  return strcmp(hostname, "MASTER") == 0;
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
  close(sockfd);
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

  htonm(m);

  if (connect(socksend, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) ExitErr("Connect", errno, __LINE__);

  if (write(socksend, &m, sizeof(m)) < 0) ExitErr("write back", errno, __LINE__);
  // close(socksend);
}

static void threadedCrack(char* hash){
  std::cout << "decrypting: " << hash << std::endl;
  const char *const seedchars =
    "qwertyuiopasdfghjklzxcvbnm"
    "QWERTYUIOPLKJHGFDSAZXCVBNM"
    "1234567890";
  const int num_chars = strlen(seedchars);
  char password[5];
  struct crypt_data data[1] = {0};
  for (int a = 0; a < num_chars; a++) {
    for (int b = 0; b < num_chars; b++) {
      for (int c = 0; c < num_chars; c++) {
        for (int d = 0; d < num_chars; d++) {
          password[0] = seedchars[a];
          password[1] = seedchars[b];
          password[2] = seedchars[c];
          password[3] = seedchars[d];
          char* newhash = crypt_r(password, hash, data);
          if (strcmp(newhash, hash) == 0) {
            strcpy(hash, password);
            std::cout << "password: " << password << std::endl;
            return;
          }
        }
      }
    }
  }
  std::cout << "nothing was found :(\n";
};

static void crack(Message& msg) {
  vector<thread> threads;
  for (uint i = 0; i < msg.num_passwds; i++) {
    threads.push_back(thread(threadedCrack, std::ref(msg.passwds[i])));
  }
  for (thread& t : threads) {
    t.join();
  }
}

static int start(int section, int size) {
  std::cout << "size : " << size << std::endl;
  return section * ((size) / 4);
}

static int end(int section, int size) {
  std::cout << "size : " << size << std::endl;
  return (section + 1) * (size) / 4;
}

static void olafCrack(Message& msg) {
  int s = start(OLAF, msg.num_passwds);
  int e = end(OLAF, msg.num_passwds);
  std::cout << "olaf range: " << s<< ", " << e;
  vector<thread> threads;
  for (int i = s; i < e; i++) {
    std::cout << "\nOlaf decrypting msg[ " << i << " ] : " << msg.passwds[i] << std::endl;
    threads.push_back(thread(threadedCrack, std::ref(msg.passwds[i])));
    //threadedCrack(msg.passwds[i]);
  }
  for (thread& t : threads) { t.join(); }
}

static void thorCrack(Message& msg) {
  int s = start(THOR, msg.num_passwds);
  int e = end(THOR, msg.num_passwds);
  std::cout << "thor range: " << s << ", " << e;
  vector<thread> threads;
  for (int i = s; i < e; i++) {
    std::cout << "\nThor decrypting msg[ " << i << " ] : " << msg.passwds[i] << std::endl;
    threads.push_back(thread(threadedCrack, std::ref(msg.passwds[i])));
    // threadedCrack(msg.passwds[i]);
  }
  for (thread& t : threads) { t.join(); }
}

static void grolliffeCrack(Message& msg) {
  int s = start(GROL, msg.num_passwds);
  int e = end(GROL, msg.num_passwds);
  std::cout << "grolliffe range: " << s << ", " << e;
  vector<thread> threads;
  for (int i = s; i < e; i++) {
    std::cout << "\ngrolliffe decrypting msg[ " << i << " ] : " << msg.passwds[i] << std::endl;
    threads.push_back(thread(threadedCrack, std::ref(msg.passwds[i])));
    // threadedCrack(msg.passwds[i]);
  }
  for (thread& t : threads) { t.join(); }
}

static void graculusCrack(Message& msg) {
  int s = start(GRAC, msg.num_passwds);
  int e = end(GRAC, msg.num_passwds);
  std::cout << "graculus range: " << s << ", " << e;
  vector<thread> threads;
  for (int i = s; i < e; i++) {
    std::cout << "\ngraculusCrack decrypting msg[ " << i << " ] : " << msg.passwds[i] << std::endl;
    threads.push_back(thread(threadedCrack, std::ref(msg.passwds[i])));
    // threadedCrack(msg.passwds[i]);
  }
  for (thread& t : threads) { t.join(); }
}

static void splitCrack(Message& msg) {
  if (iamOlaf()){
    olafCrack(msg);
  } else if (iamThor()) {
    thorCrack(msg);
  } else if (iamGrolliffe()) {
    grolliffeCrack(msg);
  } else if (iamGraculus()) {
    graculusCrack(msg);
  }
}

static void listen4merge(Message& msg) {
  std::cout << "Master, Olaf, is listening 4 merge\n";
  int sockrecv = socket(AF_INET, SOCK_STREAM, 0);
  if (sockrecv < 0) {ExitErr("sockrecv", errno, __LINE__); }
  struct sockaddr_in server_addr;
  struct sockaddr_in crack_addr;
  memset(&crack_addr, 0, sizeof(crack_addr));
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(get_unicast_port());
  if (bind(sockrecv, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) ExitErr("bind",errno,__LINE__);
  listen(sockrecv,5);
  std::cout << "I am listening on port " << get_unicast_port() << std::endl;

  Message m;
  socklen_t len = sizeof(crack_addr);
  int gotapackfrom = 0;
  for (int i = 0; i < 3; i++) {
    std::cout << "passwords:\n";
    for (uint p = 0; p < msg.num_passwds; p++) {
      std::cout << msg.passwds[p] << std::endl;
    }

    int newsock = accept(sockrecv, (struct sockaddr*)&crack_addr, &len);
    if (newsock < 0) ExitErr("newsock", errno, __LINE__);
    memset(&m, 0, sizeof(m));
    std::cout << "Accepted connection. Waiting for response\n";
    if (read(newsock, &m, sizeof(m)) < 0) ExitErr("read",errno,__LINE__);
    ntohm(m);
    if (strcmp(m.hostname,"graculus") == 0) {
      gotapackfrom++;
      std::cout << "Received packet from graculus\n";
      for (int p = start(GRAC, m.num_passwds); p < end(GRAC, m.num_passwds); p++) {
        strcpy(msg.passwds[p], m.passwds[p]);
      }
    } else if (strcmp(m.hostname,"thor") == 0) {
      std::cout << "Received packet from thor\n";
      for (int p = start(THOR, m.num_passwds); p < end(THOR, m.num_passwds); p++) {
        strcpy(msg.passwds[p], m.passwds[p]);
      }
    } else if (strcmp(m.hostname, "grolliffe") == 0) {
      gotapackfrom++;
      std::cout << "Received packet from grolliffe\n";
      for (int p = start(GROL, m.num_passwds); p < end(GROL, m.num_passwds); p++) {
        strcpy(msg.passwds[p], m.passwds[p]);
      }
    }
    close(newsock);
    if ( gotapackfrom == 2 ) break;
  }
  close(sockrecv);
}

static void send2MASTER(Message& msg) {

  int socksend = socket(AF_INET, SOCK_STREAM, 0);
  if (socksend < 0) { ExitErr("setting socket", errno, __LINE__);}

  struct hostent *server = gethostbyname(MASTER);
  if (server == NULL) { ExitErr("sendback TCP", errno, __LINE__); }
  std::cout << "hostname: " << MASTER << std::endl;
  std::cout << "port: " << get_unicast_port() << std::endl;

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  bcopy((char*)server->h_addr, (char*)&server_addr.sin_addr.s_addr, server->h_length);
  server_addr.sin_port = htons(get_unicast_port());

  printf("Try to connect:\nhostname: %s\nport# %d\n", MASTER, get_unicast_port());
  gethostname(msg.hostname, MAX_HOSTNAME_LEN);
  htonm(msg);

  if (connect(socksend, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) ExitErr("Connect", errno, __LINE__);

  if (write(socksend, &msg, sizeof(msg)) < 0) ExitErr("write back", errno, __LINE__);
}

int main(int argc, char** argv) {
  if (iamGraculus() || iamGrolliffe() || iamThor() || iamOlaf())
    std::cout << " goteem\n";
  Message msg;
  recv(msg);
  std::cout << "ami master?\n";
  thread merger;
  if (iamOlaf()) {
    merger = thread(listen4merge, std::ref(msg));
    std::cout << "let's get mergy\n";
    //listen4merge(msg);
  }

  splitCrack(msg);
  // olafCrack(msg);
  // thorCrack(msg);
  // grolliffeCrack(msg);
  // graculusCrack(msg);
  // crack(msg);
  if (!iamOlaf()) {
    send2MASTER(msg);
  }


//--------------  setup tcp connection to return passwords  --------------------------
  if (iamOlaf()) {
    for (uint p = 0; p < msg.num_passwds; p++) {
      std::cout << msg.passwds[p] << std::endl;
    }
    merger.join();
    send(msg);
  }

  std::cout << "let's make a thread pool\n";


}
