/*
 * Copyright (C) 2018 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 */

#include <iostream>
#include <thread>
#include <unistd.h>

//#include <functional>
#include <algorithm> // for_each

#include <string>
#include <chrono>
#include <condition_variable>


#include "radix.h"

using std::cout;
using std::thread;
using std::string;
using std::vector;
//#define DB

#include <ctime>


std::string asString(const std::chrono::system_clock::time_point& tp)
{
// convert to system time:
std::time_t t = std::chrono::system_clock::to_time_t(tp);
std::string ts = std::ctime(&t);// convert to calendar time
ts.resize(ts.size()-1); // skip trailing newline
return ts;
}


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

unsigned int RadixSort::maxLengthInt(const vector<string> &arr) {
  int max = 0;
  for (string str : arr) {
    int len = str.length();
    if (len > max) {
      max = len;
    }
  }
  return max;
}

// static void printall(vector<unsigned int> & arr) {
//   for (unsigned int i : arr)
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

int getDig(string str, int place) {
  if ((unsigned) place < str.length()) {
  //cout << place << "th digit of " << str << " is " << (str[place] - 48) << std::endl;
  return str[place] - 48;
} else
  return 0;
}

void RadixSort::msdStrings(vector<string> &list, unsigned int place) {
  if (maxLengthInt(list) < place) return;
  vector<vector<string>> buckets(10);
  for (string str : list) {
    int dig = getDig(str, place);
    buckets[dig].push_back(str);
  }
  list.clear();
  for (vector<string> bucket : buckets) {
    msdStrings(bucket, place + 1);
    for (string str : bucket) {
      list.push_back(str);
    }
  }
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
    // msdStrings(int_to_strings, 0);
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

//
// std::vector<thread> threads;
// for (std::reference_wrapper<vector<unsigned int>> list : lists) {
//   threads.push_back(thread { [&] {
//       // static int cores_in_use = 0;
//       std::unique_lock<std::mutex> guard(mtx);
//       cores_available.wait(guard, [&] {return cores_in_use < CORES;});
//       cores_in_use ++;
//       #ifdef DB
//        ms = std::chrono::duration_cast<std::chrono::milliseconds >(
//                std::chrono::system_clock::now().time_since_epoch()
//              );
//              std::cout << "Core start: " << cores_in_use << " " << ms.count() << std::endl;
//       #endif
//
//       sort(list);
//
//       #ifdef DB
//        ms = std::chrono::duration_cast<std::chrono::milliseconds >(
//                std::chrono::system_clock::now().time_since_epoch()
//              );
//              std::cout << "Core end: " << cores_in_use << " " << ms.count() << std::endl;
//       #endif
//       cores_in_use --;
//       cores_available.notify_all();
//   }});
//   threads.back().join();
//   threads.pop_back();
//   }
//   for (thread& t: threads) {
//     t.join();
//   }
//   #ifdef DB
//   for (vector<unsigned int> v : lists)
//     printall(v);
//   #endif




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
