/*
 * Copyright (C) 2018 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 */
#include <vector>
#include <iostream>
#include <algorithm> //std::sort
#include <thread>
#include <semaphore.h>
#include <atomic>
#include <mutex>
#include <cmath> // std::floor, std::ceil
#include <unistd.h> //usleep
#include "radix.h"
#include "cptl_stl.h"

using std::vector;
using std::string;
using std::thread;

void ParallelRadixSort::msd(std::vector<std::reference_wrapper<std::vector<unsigned int>>>& lists, unsigned int cores) {
  for (vector<unsigned int>& list : lists) {

    vector<thread> threads;

// --------------  translate ints to strings (sequential)  ------------------------
  //
  // #ifdef SEQUENTIAL
  //   translate(list,to_str);
  // #endif

// --------------  translate ints to strings (parallel)  ------------------------

// #ifdef PARALLEL
//     for (uint p = 0; p < cores; p++) {
//       uint s = std::ceil(p * ((size)/nCores));
//       uint e = std::floor((p+1) * ((size)/nCores));
//       threads.push_back(thread(intsToStrings, std::ref(list),std::ref(to_str),s,e));
//     }
//     for (thread& t : threads) {
//       t.join();
//     }
//     threads.clear();
// #endif



    vector<vector<string>> buckets(100);
// --------------  separate into buckets (sequential)  -------------------------

    // for (string& str : to_str) {
    //   buckets[ (str.at(0) - 48) * 10 + (str.at(1) - 48)].push_back(str);
    //   //buckets[ str.at(0) - 48 ].push_back(str);
    // }
    // int count = 0;
    // for (vector<string>& buck : buckets) {
    //   if (buck.size() == 0)
    //     count ++;
    // } std::cout << count << " buckets are empty" <<std::endl;

// --------------  separate into string buckets (parallel)  ------------------------

    std::mutex bucketLocks[buckets.size()];
    std::atomic<int> smallest(123456789);
    double r = list.size() / (double) cores;
    for (uint x = 0; x < cores; x++) {
      uint s = x * r;
      uint e = ((x + 1) * r) - 1;
      threads.push_back(thread { [&list, &buckets, &bucketLocks, s, e] {
        for (uint i = s; i <= e; i++) {
          string str = std::to_string(list[i]);
          uint home = (str.at(0) - 48) * 10 + (str.at(1) - 48);
          bucketLocks[home].lock();
          buckets[home].push_back(str);
          bucketLocks[home].unlock();
        }
      }});
    }
    // for (thread& t : threads) {
    //   t.join();
    // }
    // threads.clear();

    // std::cout << "----------------  before redistribution ------------------\n";
    // printf("----- %d elements\n", (int)list.size());
    // for (uint i = 0; i < buckets.size(); i++) {
    //   std::cout << "Bucket [" << i << "].size() = " << buckets[i].size() << std::endl;
    // }

    // uint sm = 123456789;
    // for_each(buckets.begin(), buckets.end(), [&sm] (vector<string> b) { if (b.size() > 0 && b.size() < sm) sm = b.size();} );
    // for (int i = 1; i < 10; i++) {
    //   if (buckets[i].size() > 5 * sm) {
    //     for (string str : buckets[i]) {
    //       buckets[(i * 10) + (str.at(1) - 48)].push_back(str);
    //     }
    //     buckets[i].clear();
    //   }
    // }
    // std::cout << "----------------  after redistribution ------------------\n";
    // int count = 0;
    // for (uint i = 0; i < buckets.size(); i++) {
    //   std::cout << "Bucket [" << i << "].size() = " << buckets[i].size() << std::endl;
    //   count += buckets[i].size();
    // }
    // printf("----- %d elements\n", count);


// --------------  sort each bucket (sequential)  --------------------------------------------
  // #ifdef SEQUENTIAL
  //   for (vector<string>& bucket : buckets) {
  //     std::sort(bucket.begin(), bucket.end());
  //   }
  // #endif
                    //faster
// --------------  sort each bucket (parallel)  --------------------------------------------


    // sem_t cores_available;
    // sem_init(&cores_available, 1, cores);
    // for (vector<string>& buck : buckets) {
    //   if (buck.size() > 1) {
    //     sem_wait(&cores_available);
    //     threads.push_back(thread{[&buck,&cores_available] {
    //       std::sort(buck.begin(),buck.end());
    //       sem_post(&cores_available);
    //     }});
    //   }
    // }
    // for (thread& t : threads) {
    //   t.join();
    // }
    // threads.clear();
    sem_t cores_available;
    sem_init(&cores_available, 1, cores);
    int start_idx[buckets.size()];
    start_idx[0] = 0;
    for (uint bucket = 1; bucket < buckets.size(); bucket++) {
      start_idx[bucket] = start_idx[bucket - 1] + buckets[bucket - 1].size();
    }
    for (uint i = 1; i < buckets.size(); i++) {
      if (buckets[i].size() > 0){




        sem_wait(&cores_available);
        threads.push_back(thread { [&list, &buckets, &start_idx, &bucketLocks, &cores_available, i] {
          std::sort(buckets[i].begin(), buckets[i].end());
          for (uint j = 0; j < buckets[i].size(); j++) {
            list[ start_idx[i] + j ] = (uint) std::stoul(buckets[i][j]);
          }
          sem_post(&cores_available);
        } });
      }
    }
    for (thread& t : threads) {
      t.join();
    }
  //  threads.clear();


            //slower
//--------  put buckets back in vector (sequential)  -----------------------------------------

//     //to_str.clear();
//     list.clear();
//     for (vector<string>& buck : buckets) {
//       for (string& s : buck) {
//         list.push_back((uint) std::stoul(s));
// //        to_str.push_back(s);
//       }
//     }

//--------  put backets back in integer vector (parallel) -----------------------------------------
    //
    // int start_idx[buckets.size()];
    // start_idx[0] = 0;
    // for (uint bucket = 1; bucket < buckets.size(); bucket++) {
    //   start_idx[bucket] = start_idx[bucket - 1] + buckets[bucket - 1].size();
    // }
    // for (uint i = 0; i < buckets.size(); i++) {
    //   if (buckets[i].size() == 0) continue;
    //   sem_wait(&cores_available);
    //   threads.push_back(thread { [&buckets, &start_idx, &list, &cores_available, i] {
    //     for (uint j = 0; j < buckets[i].size(); j++) {
    //       list[start_idx[i] + j] = (uint) std::stoul(buckets[i][j]);
    //     }
    //     sem_post(&cores_available);
    //   }});
    // }
    // for (thread& t : threads) {
    //   t.join();
    // }
    // threads.clear();
    //

//--------  translate back to integers (sequential)  -----------------------------------------
  // #ifdef SEQUENTIAL
  //   translate(to_str, list);
  // #endif

//--------  translate back to integers (parallel)  -----------------------------------------

  // #ifdef PARALLEL
  //   for (uint p = 0; p < cores; p++) {
  //     uint s = std::ceil(p * ((size)/nCores));
  //     uint e = std::floor((p+1) * ((size)/nCores));
  //     threads.push_back(thread(stringsToInts, std::ref(to_str), std::ref(list), s, e));
  //   }
  //   for (thread& t : threads) {
  //     t.join();
  //   }
  // #endif
  }
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
