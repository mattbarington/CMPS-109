/*
 * Copyright (C) 2018 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 */

#include <iostream>
#include <thread>
#include <algorithm> // for_each
#include <string>
#include "radix.h"

using std::cout;
using std::thread;
using std::string;
using std::vector;
//#define DB

RadixSort::RadixSort(const unsigned int cores) : CORES(cores) {

}

void RadixSort::msd(vector<std::reference_wrapper<vector<unsigned int>>> &lists) {
  auto sort = [&] (vector<unsigned int> &list){
    vector<string> int_to_strings;
    std::for_each( list.begin(), list.end(),
      [&int_to_strings] (unsigned int &i) {
        int_to_strings.push_back(std::to_string(i));
      }
    );
    list.clear();
    std::sort(int_to_strings.begin(),int_to_strings.end());
    std::for_each( int_to_strings.begin(),  int_to_strings.end(),
      [&list] (string &s) {
        list.push_back((unsigned int) std::stol(s));
      }
    );
  };


  int coresInUse = 0;
  std::vector<thread> threads;
  for (std::reference_wrapper<vector<unsigned int>> list : lists) {
    if (coresInUse >= CORES) {
      for (thread &thread : threads) {
        thread.join();
      }
      threads.clear();
      coresInUse = 0;
    }
    threads.push_back(thread(sort, list));
    coresInUse++;
  }
  for (thread &thread : threads) {
    thread.join();
  }
}
