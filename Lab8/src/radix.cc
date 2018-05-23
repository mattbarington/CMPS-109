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
  socklen_t len = sizeof(remote_addr);
  memset(&remote_addr, 0, sizeof(remote_addr));
  server_is_active = true;
  while (server_is_active) {

    Message m;
    int n = recvfrom(sockfd, &m, sizeof(m), 0, (struct sockaddr*) &remote_addr, &len);
    if (n < 0) {
      ExitErr("Server",errno,__LINE__);
    }
    m.sequence = ntohl(m.sequence);
    m.flag = ntohl(m.flag);
    m.num_values = ntohl(m.num_values);
    std::cout << "Let's check message values:\n";
    std::cout << "Sequence number: " << m.sequence << "\nFlag: " << m.flag  << "\nnum_vals: " << m.num_values << std::endl;
    for (uint i = 0; i < m.num_values; i++) {
      m.values[i] = ntohl(m.values[i]);
      std::cout << m.values[i] << std::endl;
    }
    vector<uint> nums;
    for (uint i = 0; i < m.num_values; i++) {
      nums.push_back(m.values[i]);
    }
    std::sort(nums.begin(), nums.end(), [](uint a, uint b) {
      return std::to_string(a).compare(std::to_string(b)) < 0;
    });
    for (uint i = 0; i < m.num_values; i++) {
      m.values[i] = htonl(nums[i]);
    }
    m.sequence = htonl(m.sequence);
    m.flag = htonl(LAST);
    m.num_values = htonl(m.num_values);

    n = sendto(sockfd, &m, sizeof(m), 0, (struct sockaddr*) &remote_addr, len);

  }
  close(sockfd);

}

/*
 * Shutdown the server. Typically this will involve closing the server socket.
 */
void RadixServer::stop() {
  server_is_active = false;
    // std::cout << "Get STOPPY\n";
    // throw "not implemented";
}

// static bool addToMessage(uint i, Message& m) {
//   if (m.num_values < MAX_VALUES) {
//     m.values[m.num_values] = i;
//     m.num_values++;
//     return true;
//   }
//   return false;
// }

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

  socklen_t len = sizeof(remote_addr);

  vector<uint> nums;
  // srand(time(0));
  // for (int i = 0; i < 15; i++) {
  //   nums.push_back((rand() % 999 ));
  // }
  vector<uint> nums2;
  for (uint i : lists[0].get()) {
    nums.push_back(i);
    nums2.push_back(i);
  }

  std::sort(nums2.begin(), nums2.end(), [] (uint a, uint b) {
    return std::to_string(a).compare(std::to_string(b)) < 0;
  });

  // int num = -1;
  int n;

  //------------  let's try to send a Message  --------------------------------
  Message m;
  memset(&m, 0, sizeof(m));
  m.flag = LAST;
  m.sequence = 0;
  m.num_values = 0;
  for (uint i : nums) {
    m.values[m.num_values] = i;
    m.num_values ++;
  }
  std::cout << "Let's check message values:\n";
  std::cout << "Sequence number: " << m.sequence << "\nFlag: " << m.flag  << "\nnum_vals: " << m.num_values << std::endl;
  // for (uint i = 0; i < m.num_values; i++) {
  //   std::cout << m.values[i] << std::endl;
  // }
  std::cout << "Converting Values to network byte order.....\n";
  for (uint i = 0; i < m.num_values; i++) {
    m.values[i] = htonl(m.values[i]);
  }
  m.flag = htonl(m.flag);
  m.sequence = htonl(m.sequence);
  m.num_values = htonl(m.num_values);

  std::cout << "SENDERR\n";
  n = sendto(sockfd, &m, sizeof(m), 0, (struct sockaddr*) &remote_addr, len);
  if (n < 0 ) {
    ExitErr("Client", errno, __LINE__);
  }

  for (uint i = 0; i < MAX_VALUES; i++) {
    m.values[i] = 0;
  }
 std::cout << "Values array set to 0s. Waiting for recv\n.......";

//---------------  let's receive a messaage  ----------------------------

  n = recvfrom(sockfd, &m, sizeof(m), 0, (struct sockaddr*) &remote_addr, &len);
  //std::cout << "byte's received: " << n <<std::endl;
  if (n < 0 ) {
    ExitErr("Client", errno, __LINE__);
  }
  //std::cout << "Converting values back from network byte order....\n";
  for (uint i = 0; i < m.num_values; i++) {
    m.values[i] = ntohl(m.values[i]);
  }
  //std::cout << "Sequence number: " << m.sequence << "\nFlag: " << m.flag  << "\nnum_vals: " << m.num_values << std::endl;
  m.flag = ntohl(m.flag);
  m.sequence = ntohl(m.sequence);
  // m.num_values = ntohl(m.num_values);
  //std::cout << "Sequence number: " << m.sequence << "\nFlag: " << m.flag  << "\nnum_vals --- vs. --- num2_vals: " << m.num_values << std::endl;

  lists[0].get().clear();
  for (uint i = 0; i < m.num_values; i++) {
    // std::cout << m.values[i] << " --- " << nums2[i] << std::endl;
    lists[0].get().push_back(m.values[i]);
  }


  // -----------  let's use a vector of message_t  --------------------
  // vector<Message> messages;
  // messages.push_back(Message());
  // // int num_values = 0;
  // for (uint i : nums) {
  //   if (!addToMessage(i, messages.back())) {
  //     messages.push_back(Message());
  //     addToMessage(i,messages.back());
  //   }
  //   // put into a message.
  // }
  // int seq = 0;
  // for (Message& m : messages) {
  //   m.sequence = seq;
  //   seq++;
  //   m.flag = NONE;
  // }
  // messages.back().flag = LAST;
  //
  // for (Message& m : messages) {
  //   std::cout << "numVals: " << m.num_values << std::endl;
  //   std::cout << "sequence: " << m.sequence << std::endl;
  //   std::cout << "Flag: " << m.flag << std::endl;
  //   for (int i = 0; i < MAX_VALUE; i++) {
  //     std::cout << m.values[i] << ", ";
  //   }
  // }


  // for (int i : nums) {
  //   n = sendto(sockfd, &i, sizeof(int), 0, (struct sockaddr*) &remote_addr, len);
  //   if (n < 0 ) {
  //     ExitErr("Client", errno, __LINE__);
  //   }
  // }
  // num = 0;
  // n = sendto(sockfd, &num, sizeof(int), 0, (struct sockaddr*) &remote_addr, len);
  // if (n < 0 ) {
  //   ExitErr("Client", errno, __LINE__);
  // }
  // std::cout << "client receiving\n";
  // n = recvfrom(sockfd, &num, sizeof(int), 0, (struct sockaddr*) &remote_addr, &len);
  // if (n < 0) {
  //   ExitErr("Client", errno, __LINE__);
  // }
  // for (;;) {
  //   n = recvfrom(sockfd, &num, sizeof(int), 0, (struct sockaddr*) &remote_addr, &len);
  //   if (n < 0) {
  //     ExitErr("Client", errno, __LINE__);
  //   }
  //   std::cout << num << std::endl;
  // }

  close(sockfd);
  return;
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
