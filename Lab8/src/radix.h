/*
 * Copyright (C) 2018 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 */

#include <vector>
#include <functional>

#define MAX_VALUES 128

#define NONE    0 // Default flag for on-wire datagram
#define LAST    1 // Identifies last datagram in batch
#define RESEND  2 // Request for a re-send of missing batches
                  // If set, VALUES contains the sequence numbers of missing batches

/*
 * On-wire datagram.
 *
 * All elements should be converted to network byte-order before transmission
 * and converted back to host byte-order on recepit.
 */
typedef struct message_t {
    unsigned int values[MAX_VALUES]; // Sorted or to-be sorted numbers if FLAG is NONE or LAST
                                     // Missing sequence numbers if FLAG is RESEND
    unsigned int num_values; // Number of valid entries in VALUES
    unsigned int sequence;   // Unique sequence number of trasmission batch, starting at zero
    unsigned int flag;       // One of NONE, LAST, RESEND
}
Message;

class RadixServer {
private:
  bool server_is_active = true;
public:
    void start(const int port, const unsigned int cores);
    void stop();
};

class RadixClient {
public:
    void msd(const char *hostname, const int port, std::vector<std::reference_wrapper<std::vector<unsigned int>>> &lists);
};
