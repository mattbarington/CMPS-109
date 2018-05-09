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

// static unsigned int maxLengthInt(const vector<unsigned int> &arr) {
//   int max = 0;
//   int len = 0;
//   for (unsigned int i : arr) {
//     len = std::to_string(i).length();
//     if (len > max) {
//       max = len;
//     }
//   }
//   return max;
// }
//
// static unsigned int maxLengthString(const vector<string> v) {
//   int max = 0;
//   int len = 0;
//   for (string str : v) {
//     len = str.length();
//     if (len > max) {
//       max = len;
//     }
//   }
//   return max;
// }
//
//
// static void printall(vector<unsigned int> & arr) {
//   for (unsigned int i : arr)
//     std::cout << i << std::endl;
// }
//

//
//
// void ParallelRadixSort::msd(vector<string>& arr, unsigned int d) {
//   if (maxLengthString(arr) <= d) return;
//   vector<vector<string>> buckets(10);
//   for (string& str : arr) {
//     unsigned int dig = getDig(str,d);
//     buckets[dig].push_back(str);
//   }
//   arr.clear();
//   for (vector<string> buck : buckets) {
//     msd(buck, d + 1);
//     for (string& str : buck) {
//       arr.push_back(str);
//     }
//   }
// }
//
// void ParallelRadixSort::msd(vector<string>& strs) {
//   msd(strs,0);
// }
//
// /*
//  * Just your baic good ol' insertion sort,
//  * grabbed from https://www.geeksforgeeks.org/insertion-sort/
//  */
// static void InsertionSort(vector<string>& strs) {
//   int n = strs.size();
//   int i, j;
//    string key;
//    for (i = 1; i < n; i++) {
//        key = strs[i];
//        j = i-1;
//
//        while (j >= 0 && strs[j] > key) {
//            strs[j+1] = strs[j];
//            j = j-1;
//        }
//        strs[j+1] = key;
//    }
// }
//
// void ParallelRadixSort::msd(vector<string>& list) {
//   vector<vector<string>> buckets(10);
//   vector<thread> threads;
//   for (string& str : list) {
//     buckets[(int) str.at(0) - 48].push_back(str);
//   }
//   list.clear();
//   for (vector<string> buck : buckets) {
//     //threads.push_back(thread {[&] {
//       std::sort(buck.begin(),buck.end());
//     // }});
//     // std::sort(buck.begin(), buck.end());
//     for (string& str : buck) {
//       list.push_back(str);
//     }
//   }
// }

// void ParallelRadixSort::stringy(string& str) {
//   std::cout << str << std::endl;
// }
//
// void ParallelRadixSort::something(int h) {
//   std::cout << (h+1) <<std::endl;
// }
//
// void ParallelRadixSort::nothing() {
//   std::cout << "nothing\n";
// }
//
// void ParallelRadixSort::vectStringy(vector<string> stings) {
//   for (string& s : stings) {
//     std::cout << s << std::endl;
//   }
// }
//
// void ParallelRadixSort::refvectstring(vector<string>& stringss) {
//   for (string& s : stringss) {
//     std::cout << s << std::endl;
//   }
// }

// void ParallelRadixSort::msd(vector<string>& list) {
//   vector<vector<string>> buckets(10);
//   vector<thread> threads;
//   for (string& str : list) {
//     buckets[(int) str.at(0) - 48].push_back(str);
//   }
//   list.clear();
//   for (vector<string> buck : buckets) {
//     //threads.push_back(thread {[&] {
//       std::sort(buck.begin(),buck.end());
//     // }});
//     // std::sort(buck.begin(), buck.end());
//     for (string& str : buck) {
//       list.push_back(str);
//     }
//   }
// }

static void sort(vector<string>& list) {
  vector<vector<string>> buckets(10);
  for (string& str : list) {
    buckets[ str.at(0) - 48].push_back(str);
  }
  list.clear();
  vector<thread> threads;
  for (vector<string>& buck : buckets) {
    threads.push_back(thread{[&buck] {std::sort(buck.begin(),buck.end());}});
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

static void msdInts(vector<unsigned int>& list) {
  vector<string> to_str;
  for (unsigned int i : list) {
    to_str.push_back(std::to_string(i));
  }
  sort(to_str);
  list.clear();
  for (string& str : to_str) {
    list.push_back((unsigned int) std::stoul(str));
  }
}

void ParallelRadixSort::msd(std::vector<std::reference_wrapper<std::vector<unsigned int>>>& lists, unsigned int cores) {
  vector<thread> threads;
  for (vector<unsigned int>& list : lists) {
    threads.push_back(thread{msdInts,std::ref(list)});
  }
  for (thread& t : threads) {
    t.join();
  }
  threads.clear();
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
