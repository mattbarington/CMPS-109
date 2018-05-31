#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <crypt.h>
#include <cstring>
#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>

using std::vector;
using std::thread;

int main(int argc, char** argv) {
  char salt[] = "$1";
  const char *const seedchars =
    "qwertyuiopasdfghjklzxcvbnm"
    "QWERTYUIOPLKJHGFDSAZXCVBNM"
    "1234567890";
    // "0123456789ABCDEFGHIJKLMNOPQRST"
    // "UVWXYZabcdefghijklmnopqrstuvwxyz";
  const int num_chars = strlen(seedchars);
  unsigned long seed[2];

  /* Generate a (not very) random seed.
     You should do it better than this... */
  seed[0] = time(NULL);
  seed[1] = getpid() ^ (seed[0] >> 14 & 0x30000);

  /* Turn it into printable characters from ‘seedchars’. */
  for (int i = 0; i < 2; i++)
    salt[i] = seedchars[(seed[i/5] >> (i%5)*6) & 0x3f];

  /* Read in the user’s password and encrypt it. */
  char* user_passwd = getpass("Password:");
  //char* h = crypt(user_passwd, salt);
  char hash[13];
  strcpy(hash, crypt(user_passwd, salt));

  /* Print the results. */
  std::cout << "User Passwd: " << user_passwd << std::endl;
  std::cout << "hash:" << hash << std::endl;


  vector<thread> threads;
  std::atomic<bool> cracked(false);
  std::mutex lock;
  for (int a = 0; a < num_chars; a++) {
    threads.push_back(thread{ [&lock, a, hash, &cracked, &seedchars] {
      char password[5];
      if (cracked) return;
      for (int b = 0; b < num_chars; b++) {
        for (int c = 0; c < num_chars; c++) {
          for (int d = 0; d < num_chars; d++) {
            password[0] = seedchars[a];
            password[1] = seedchars[b];
            password[2] = seedchars[c];
            password[3] = seedchars[d];
            if (strcmp(crypt(password, hash), hash) == 0) {
              lock.lock();
              cracked = true;
              strcpy(hash, password);
              lock.unlock();
            }
    }}}};);
  char password[5];
  for (int a = 0; a < num_chars; a++) {
    for (int b = 0; b < num_chars; b++) {
      for (int c = 0; c < num_chars; c++) {
        for (int d = 0; d < num_chars; d++) {
          password[0] = seedchars[a];
          password[1] = seedchars[b];
          password[2] = seedchars[c];
          password[3] = seedchars[d];
          char* newhash = crypt(password, hash);
          if (strcmp(newhash, hash) == 0) {
            std::cout << "We found it! pasword is " << password << std::endl;
            return 0;
          }
        }
      }
    }
  }
  return -1;
}
