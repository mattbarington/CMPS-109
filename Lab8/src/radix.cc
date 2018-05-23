/*
 * Copyright (C) 2018 Matthew B. Ovenden. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <arpa/inet.h>

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <semaphore.h>
#include <algorithm> //std::sort
#include "radix.h"

using std::vector;


static void ExitErr(std::string source, int err, int line) {
  std::cerr << source << ": '" << strerror(err) << "' error on line " << line << std::endl;
  exit(-1);
}

static void htonM(Message& m) {
  for (uint i = 0; i < m.num_values; i++) {
    m.values[i] = htonl(m.values[i]);
  }
  m.sequence = htonl(m.sequence);
  m.flag = htonl(LAST);
  m.num_values = htonl(m.num_values);
}

static void ntohM(Message& m) {
  m.sequence = ntohl(m.sequence);
  m.flag = ntohl(m.flag);
  m.num_values = ntohl(m.num_values);
  for (uint i = 0; i < m.num_values; i++) {
    m.values[i] = ntohl(m.values[i]);
  }
}

/**
 *  Receives Message from specified sender.
 *  Converts Message parameters from network to host byte order.
 */
static void recvMessage(Message& m, int sock, sockaddr_in &rem_addr) {
  socklen_t len = sizeof(rem_addr);
  int n = recvfrom(sock, &m, sizeof(m), 0, (struct sockaddr*) &rem_addr, &len);
  if (n < 0) {
    throw "Unable to Recv";
  }
  ntohM(m);
}

/**
 *  Converts Message from host to netork byte order.
 *  Sends Message through specified socket.
 */
static void sendMessage(Message& m, int sock, sockaddr_in& addr) {
  htonM(m);
  int n = sendto(sock, &m, sizeof(m), 0, (struct sockaddr*) &addr, sizeof(addr));
  if (n < 0) {
    throw "Unable to Send";
  }
}

/*
 * Start a UDP listener on PORT and accept lists of unsiged integers from
 * clients to be MSD RAdix sorted using no more that CORES cpu cores. After
 * sorting the lists are to be retiurned to the client.

 * UDP socket setup referenced from the one, the only :
 * https://classes.soe.ucsc.edu/cmps109/Spring18/SECURE/13.Distributed3.pdf
 */
void RadixServer::start(const int port, const unsigned int cores) {

  std::cout << "Setting up the server\n";

  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    ExitErr("Server", errno, __LINE__);
  }

  struct sockaddr_in server_addr;
  bzero((char*) &server_addr, sizeof(struct sockaddr_in));

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(port);

  if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
    ExitErr("Server", errno, __LINE__);
  }

  struct sockaddr_in remote_addr;
  // socklen_t len = sizeof(remote_addr);
  memset(&remote_addr, 0, sizeof(remote_addr));
  server_is_active = true;
  while (server_is_active) {

    Message m;
    recvMessage(m, sockfd, remote_addr);
    vector<uint> nums;
    for (uint i = 0; i < m.num_values; i++) {
      nums.push_back(m.values[i]);
    }
    std::sort(nums.begin(), nums.end(), [](uint a, uint b) {
      return std::to_string(a).compare(std::to_string(b)) < 0;
    });
    for (uint i = 0; i < nums.size(); i++) {
      m.values[i] = nums[i];
    }
    sendMessage(m, sockfd, remote_addr);
  }
  close(sockfd);

}

/*
 * Shutdown the server. Typically this will involve closing the server socket.
 */
void RadixServer::stop() {
  server_is_active = false;
}

/*
 * Send the contents of the lists contained with LISTS to the server on HIOSTNAME
 * listening on PORT in datagrams containing batches of unsigned integers. These
 * will be returned to you MSD Radix sorted and should be retied to the caller
 * via LISTS.
 */
void RadixClient::msd(const char *hostname, const int port, std::vector<std::reference_wrapper<std::vector<unsigned int>>> &lists) {
  std::cout << "Launching msd in the client\n";


  int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    ExitErr("Client", errno, __LINE__);
  }
  std::cout << "Resolving host\n";
  struct hostent* server = gethostbyname(hostname);
  if (server == NULL) {
    std::cerr << "Could not resolve server host name\n";
    exit(-1);
  }

  std::cout << "Initializing Connection\n";
  struct sockaddr_in remote_addr;
  bzero((char*) &remote_addr, sizeof(remote_addr));
  remote_addr.sin_family = AF_INET;
  bcopy((char*) server->h_addr, (char*) &remote_addr.sin_addr.s_addr, server->h_length);
  remote_addr.sin_port = htons(port);

  // socklen_t len = sizeof(remote_addr);

for (vector<uint>& list : lists) {
    vector<uint> nums;
    for (uint i : list){
      // std::cout << i << std::endl;
      nums.push_back(i);
    }
    std::sort(nums.begin(),nums.end(),[](uint a, uint b) {
      return std::to_string(a).compare(std::to_string(b)) < 0;
    });
    //------------  let's try to send a Message  --------------------------------
    Message m;
    memset(&m, 0, sizeof(m));
    m.flag = LAST;
    m.sequence = 0;
    m.num_values = 0;
    for (uint i : list) {
      m.values[m.num_values] = i;
      m.num_values ++;
    }
    // htonM(m);
    try {
      sendMessage(m, sockfd, remote_addr);
    } catch (const char* e) {
      ExitErr(e, errno, __LINE__);
    }
    for (uint i = 0; i < MAX_VALUES; i++) {
      m.values[i] = 0;
    }
   std::cout << "Values array set to 0s. Waiting for recv.......\n";

  //---------------  let's receive a messaage  ----------------------------

    try {
      recvMessage(m, sockfd, remote_addr);
    } catch (const char* e) {
      ExitErr(e, errno, __LINE__);
    }
    list.clear();
    for (uint i = 0; i < m.num_values; i++) {
      list.push_back(m.values[i]);
    }

  } // for list : lists
  close(sockfd);
}


//
// }
//



//
//

//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
