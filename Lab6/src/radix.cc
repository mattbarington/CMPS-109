/*
 * Copyright (C) 2018 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 */
#include <vector>
#include <iostream>
#include <algorithm>
#include <thread>
#include <semaphore.h>
#include <atomic>
#include "radix.h"

using std::vector;
using std::string;
using std::thread;

// static int getDig(string &str, int place) {
//   return ((unsigned) place < str.length() ? str.at(place) - 48 : 0);
// }
//
// static void printall(vector<string>& v) {
//   for (string s : v) {
//     std::cout << s << std::endl;
//   }
// }



static void sort(vector<string>& list, unsigned int cores) {
  vector<vector<string>> buckets(10);
  for (string& str : list) {
    buckets[ str.at(0) - 48].push_back(str);
  }
  list.clear();
  vector<thread> threads;

  //semaphore initialization
  sem_t *cores_available = sem_open("cores_available", cores);
  //sem_init(cores_available, 0, cores);


  for (vector<string>& buck : buckets) {
    //sema down
    sem_wait(cores_available);
    threads.push_back(thread{[&buck,&cores_available] {std::sort(buck.begin(),buck.end());
      //semaphore up
      sem_post(cores_available);
    }});
  }
  for (thread& t : threads) {
    t.join();
  }
  list.clear();
  for (vector<string>& V : buckets) {
    for (string& s : V) {
      list.push_back(s);
    }
  }
}

static void msdInts(vector<unsigned int>& list, unsigned int cores) {
  vector<string> to_str;
  for (unsigned int i : list) {
    to_str.push_back(std::to_string(i));
  }
  sort(to_str, cores);
  list.clear();
  for (string& str : to_str) {
    list.push_back((unsigned int) std::stoul(str));
  }
}

static void copyTo(vector<unsigned int>& intList, vector<string>& strList) {
  strList.clear();
  for (unsigned int i : intList) {
    strList.push_back(std::to_string(i));
  }
}

static void copyTo(vector<string>& strList, vector<unsigned int>& intList) {
  intList.clear();
  for (string& s : strList) {
    intList.push_back((unsigned int) std::stoul(s));
  }
}

void ParallelRadixSort::msd(std::vector<std::reference_wrapper<std::vector<unsigned int>>>& lists, unsigned int cores) {
  for (vector<unsigned int>& list : lists) {
    vector<string> to_str;
    copyTo(list,to_str);

    vector<vector<string>> buckets(10);
    for (string& str : to_str) {
      buckets[ str.at(0) - 48].push_back(str);
    }

    vector<thread> threads;
    for (vector<string>& buck : buckets) {
      std::sort(buck.begin(),buck.end());
      // threads.push_back(thread{[&buck] {std::sort(buck.begin(),buck.end());
      // }});
    }
    for (thread& t : threads) {
      t.join();
    }
    to_str.clear();
    for (vector<string>& buck : buckets) {
      for (string& s : buck) {
        to_str.push_back(s);
      }
    }

    copyTo(to_str,list);
    //
    // for (unsigned int i : list) {
    //   std::cout << i << std::endl;
    // }

    // using semaphores: if semaphore flag is allowing more threads,
    //  then create another thread.
    // // threads.push_back(thread{msdInts,std::ref(list)});
    // msdInts(list, cores);
  }
  // for (thread& t : threads) {
  //   t.join();
  // }
  // threads.clear();
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
// //
//
//
//
//
//
//
// //
//
//
//
//
//
//
// //
//
//
//
//
//
//
// //
//
//
//
//
//
//
// //
//
//
//
//
//
//
//
