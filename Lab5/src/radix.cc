/*
 * Copyright (C) 2018 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 */

#include <iostream>
#include <thread>
#include <mutex>

//#include <functional>
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

static unsigned int getDig(unsigned int num, unsigned int place) {
  std::string str = std::to_string(num);
  return (str.length() > place ? str[place] - 48 : 0);
}

unsigned int RadixSort::maxLengthInt(const vector<unsigned int> &arr) {
  int max = 0;
  for (unsigned int i : arr) {
    int len = std::to_string(i).length();
    if (len > max) {
      max = len;
    }
  }
  return max;
}

void RadixSort::printall(vector<unsigned int> & arr) {
  for (unsigned int i : arr)
    cout << i << std::endl;
}

// static void nothing() {
//   for (int i = 3; i < 9; i++)
//     cout << i << std::endl;
// }

void RadixSort::msd(vector<unsigned int> &arr, unsigned int place) {
  if (maxLengthInt(arr) < place) return;
  // std::mutex m;
  // m.lock();
  vector<vector<unsigned int>> buckets(10);
  for (unsigned int i : arr) {
    int dig = getDig(i,place);
    buckets[dig].push_back(i);
  }
  arr.clear();
  for (vector<unsigned int> buck : buckets) {
    msd(buck, place + 1);
    for (unsigned int i : buck) {
      arr.push_back(i);
    }
  }
  // m.unlock();
}

void RadixSort::msd(vector<unsigned int> &arr) {

  #ifdef DB
  cout << "Entering MSD: here's the array: \n";
  printall(arr);
  #endif

  msd(arr,0);

  #ifdef DB
  cout << "Finishing up MSD: Here's the sorted array:\n";
  printall(arr);
  #endif
}

void joinAllThreads(vector<thread> &threads) {
  for (thread &t : threads) {
    t.join();
  }
}

void RadixSort::msd(vector<std::reference_wrapper<vector<unsigned int>>> &lists) {

  auto sort = [] (vector<unsigned int> &list){
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
      //  unsigned int ui = (unsigned int) std::stol(s);
        list.push_back((unsigned int) std::stol(s));
      }
    );
  };

  std::vector<thread> threads;
  int coresInUse = 0;
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
