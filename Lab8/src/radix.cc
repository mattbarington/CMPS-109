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
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <semaphore.h>
#include <algorithm> //std::sort
#include "radix.h"

using std::vector;

// static void ExitErr(std::string source, int err, int line) {
//   std::cerr << source << ": '" << strerror(err) << "' error on line " << line << std::endl;
//   exit(-1);
// }

static void ExitErr(const char* source, int err, int line) {
  printf("%s: '%s' error on line %d\n",source, strerror(err), line);
}

static void htonM(Message& m) {
  for (uint i = 0; i < m.num_values; i++) {
    m.values[i] = htonl(m.values[i]);
  }
  m.sequence = htonl(m.sequence);
  m.flag = htonl(m.flag);
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
    std::cout << "Unable to recv in recvMessage\n";
    throw "Unable to Recv";
  }
  ntohM(m);
}

/**
 *  Converts Message from host to netork byte order.
 *  Sends Message through specified socket.
 *  Note: Message m is purposefully a non-referece, because it's data members will
 *  be converted to network byte order, and we want it to maintain host byte ordering
 *  for future use.
 */
static void sendMessage(Message m, int sock, sockaddr_in& addr) {
  htonM(m);
  int n = sendto(sock, &m, sizeof(m), 0, (struct sockaddr*) &addr, sizeof(addr));
  if (n < 0) {
    std::cout << "Unable to send in sendMessage\n";
    throw "Unable to Send";
  }
  //ntohM(m);
}

static bool vacancies(Message& m) {
  return m.num_values < MAX_VALUES;
}

static void pushItRealGood(Message& m, uint i) {
  if (vacancies(m)) {
    m.values[m.num_values] = i;
    m.num_values ++;
  } else {
    std::cout << "There is no room in here!\n";
    throw "No Room In Message";
  }
}

static void sendMessages(vector<Message>& messages, int sock, sockaddr_in& addr) {
  for (Message& message : messages) {
    try {
      sendMessage(message, sock, addr);
    } catch (const char* e) {
      ExitErr(e, errno, __LINE__);
    }
  }
  // last message is the LAST message
  messages.back().flag = LAST;
}

static vector<uint> missingSequenceNumbers(vector<Message>& messages) {
  vector<uint> missingThings;
  std::sort(messages.begin(), messages.end(), [] (Message& a, Message& b) {
    return a.sequence < b.sequence;
  });
  uint expt = 0;
  uint i = 0;
  while (i < messages.size()) {
    if (messages[i].sequence == expt) {
      i++;
      expt++;
    } else {
      missingThings.push_back(expt++);
    }
  }
  for (uint i : missingThings) {
      std::cout << i << std::endl;
  }
  return missingThings;
}


static void recvMessages(vector<Message>& messages, int sock, sockaddr_in& addr) {
  vector<Message> buffer;
  Message m;
  int rc = -1;
  do {
    try {
      recvMessage(m, sock, addr);
    } catch (const char* e) {
      ExitErr(e, errno, __LINE__);
    }
    if (m.flag == RESEND) {
      sendMessages(messages, sock, addr);
      m.flag = NONE;
      buffer.clear();
    } else {
      buffer.push_back(m);
    }

  } while (m.flag == NONE);

  if (rc == 0) {
    std::cout << "TIMEOUT BOI\n";
    exit(0);
  }

  std::sort(buffer.begin(), buffer.end(), [] (Message& a, Message& b) {
    return a.sequence < b.sequence;
  });

  uint expt = 0;
  uint i = 0;
  vector<uint> missingThings;
  while (i < buffer.size()) {
    if (buffer[i].sequence == expt) {
      i++;
      expt++;
    } else {
      missingThings.push_back(expt++);
    }
  }

  if (missingThings.size() > 0) {
    Message resend;
    resend.num_values = 0;
    resend.sequence = 0;
    resend.flag = RESEND;
    for (uint missing : missingThings) {
      pushItRealGood(resend, missing);
    }
    sendMessage(resend, sock, addr);
    do {
      try {
        recvMessage(m, sock, addr);
      } catch (const char* e) {
        ExitErr(e, errno, __LINE__);
      }
      buffer.push_back(m);
    } while (m.flag == NONE);
  }

  std::sort(buffer.begin(), buffer.end(), [] (Message& a, Message& b) {
    return a.sequence < b.sequence;
  });
  messages.clear();
  for (Message& message : buffer) {
    messages.push_back(message);
  }
}

/**
 *  Accepts vector of unsigned integers to be put into packets.
 *  Returns a vector of packets, each full except for the last.
 */
static vector<Message> packaged(vector<uint> nums) {
  vector<Message> messages;
  messages.push_back(Message());
  memset(&messages.back(), 0, sizeof(Message));
  uint sequence = 0;
  for (uint i : nums) {
    if (vacancies(messages.back())) {
      pushItRealGood(messages.back(),i);
    } else {
      messages.push_back(Message());
      memset(&messages.back(), 0, sizeof(Message));
      messages.back().sequence = ++sequence;
      pushItRealGood(messages.back(),i);
    }
  }
  messages.back().flag = LAST;
  return messages;
}

static void sendIntegers(vector<uint>& nums, int sock, sockaddr_in& addr) {
  for (Message& message : packaged(nums)) {
    try {
      sendMessage(message, sock, addr);
    } catch (const char* e) {
      ExitErr(e, errno, __LINE__);
    }
    //memset(&message.values, 0, sizeof(uint) * MAX_VALUES);
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
  memset(&remote_addr, 0, sizeof(remote_addr));

  server_is_active = true;
  while (server_is_active) {
    vector<uint> nums;
    vector<Message> buffer;
// ---- recvMessages takes a Message buffer, and stores recved messages in &buffer
    recvMessages(buffer, sockfd, remote_addr);

    for (Message& message : buffer) {
      for (uint i = 0; i < message.num_values; i++) {
        nums.push_back(message.values[i]);
      }
    }

    std::sort(nums.begin(), nums.end(), [](uint a, uint b) {
      return std::to_string(a).compare(std::to_string(b)) < 0;
    });

    sendIntegers(nums, sockfd, remote_addr);
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
  socklen_t len = sizeof(remote_addr);
  bzero((char*) &remote_addr, sizeof(remote_addr));
  remote_addr.sin_family = AF_INET;
  bcopy((char*) server->h_addr, (char*) &remote_addr.sin_addr.s_addr, server->h_length);
  remote_addr.sin_port = htons(port);

  for (vector<uint>& list : lists) {
    //Create vector of messages
    vector<Message> messages = packaged(list);
//     uint sequence = 0;
//     messages.push_back(Message());
//     memset(&messages.back(), 0, sizeof(Message));
// //------------------  Datagram / Message creation  -----------------------------
//     for (uint i : list) {
//       if (vacancies(messages.back())) {
//         pushItRealGood(messages.back(),i);
//       } else {
//         messages.push_back(Message());
//         memset(&messages.back(), 0, sizeof(Message));
//         messages.back().sequence = ++sequence;
//         pushItRealGood(messages.back(),i);
//       }
//     }
//     // last message is the LAST message
//     messages.back().flag = LAST;
//------------------  Messages away!  ------------------------------------------


    sendMessages(messages, sockfd, remote_addr);


/**
 *  setsockopt referenced from stackOverflow:
 * https://stackoverflow.com/questions/4181784/how-to-set-socket-timeout-in-c-when-making-multiple-connections
 * and
 * https://linux.die.net/man/3/setsockopt
 */
//------------------  This is broken maybe...  -------------------------------
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*) &timeout, sizeof(timeout)) < 0) {
      ExitErr("Client", errno, __LINE__);
    }
    int rc = -1;
    Message m;
    vector<Message> buffer;

    do {
      timeout.tv_sec = 3;
      timeout.tv_usec = 0;
      rc = recvfrom(sockfd, &m, sizeof(m), 0, (struct sockaddr*) &remote_addr, &len);
      if (rc < 0) {
        std::cout << "Timeout\n";
        std::cout << "Clearing buffer\n";
        buffer.clear();
        std::cout << "Resending all messages again\n";
        sendMessages(messages, sockfd, remote_addr);
      } else {
        ntohM(m);
        buffer.push_back(m);
      }

    } while (m.flag != LAST);

    if (missingSequenceNumbers(buffer).size() > 0) {
      buffer.clear();
      sendMessages(messages, sockfd, remote_addr);
      do {
        timeout.tv_sec = 3;
        timeout.tv_usec = 0;
        rc = recvfrom(sockfd, &m, sizeof(m), 0, (struct sockaddr*) &remote_addr, &len);
        if (rc < 0) {
          std::cout << "Timeout\n";
          std::cout << "Clearing buffer\n";
          buffer.clear();
          std::cout << "Resending all messages again\n";
          //sendMessage(messages.back(), sockfd, remote_addr);
          sendMessages(messages, sockfd, remote_addr);
        } else {
          ntohM(m);
          buffer.push_back(m);
        }

      } while (m.flag != LAST);
    }
    list.clear();
    for (Message& message : buffer) {
      for (uint i = 0; i < message.num_values; i++) {
        list.push_back(message.values[i]);
      }
    }


    // fd_set readfds;
    // FD_ZERO(&readfds);
    // FD_SET(sockfd, &readfds);
    // struct timeval tv;
    // int rc = -1;
    // Message m;
    // Message resend;
    // memset(&resend,0,sizeof(resend));
    // uint expt = 0;
    // do {
    //   FD_SET(sockfd, &readfds);
    //   tv.tv_sec = 2;
    //   rc = select(sockfd + 1, &readfds, 0, 0, &tv);
    //   if (rc == 0) {
    //     std::cout << "Timeout\n";
    //     pushItRealGood(resend, expt);
    //     sendMessage(resend, sockfd, remote_addr);
    //     // sendMessages(messages, sockfd, remote_addr);
    //     // messages.clear();
    //     continue;
    //   }
    //
    //   if (read(sockfd, &m, sizeof(m)) < 0) {
    //     std::cout << "Silently exiting\n"; exit(-1);
    //   }
    //   ntohM(m);
    //   while (expt < m.sequence) {
    //     std::cout << "Missing datagram " << expt << std::endl;
    //     pushItRealGood(resend, expt++);
    //   }
    //   expt++;
    //
    //   std::cout << "Received sequence: " << m.sequence << std::endl;
    //
    //   messages.push_back(m);
    // } while (m.flag != LAST);
    //
    // if (resend.num_values > 0) {
    //   resend.flag = RESEND;
    //   sendMessage(resend, sockfd, remote_addr);
    // }
    //
    // do {
    //   recvMessage(m, sockfd, remote_addr);
    // } while (m.flag == NONE);
    //
    // std::sort(messages.begin(), messages.end(), [] (Message& a, Message& b) {
    //   return a.sequence < b.sequence;
    // });
    //
    // list.clear();
    // for (Message& message : messages) {
    //   for (uint i = 0; i < message.num_values; i++) {
    //     list.push_back(message.values[i]);
    //   }
    // }
    // return;








//------------------  Let's see what we've got!  -------------------------------
    // std::cout << "Values array set to 0s. Waiting for recv.......\n";
    // //recvMessages takes vector of messages, and then stores received messages in &messages
    // std::cout << "Sockfd: " << sockfd << std::endl;
    // //recvMessages(messages, sockfd, remote_addr);
    // std::cout << "Sockfd: " << sockfd << std::endl;
    // list.clear();
    // for (Message& message : messages) {
    //   for (uint i = 0; i < message.num_values; i++) {
    //     list.push_back(message.values[i]);
    //   }
    // }
  } // for list : lists
  close(sockfd);
}


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
//
//
