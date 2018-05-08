/*
 * Copyright (C) 2018 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 */
#include <vector>
#include <iostream>
#include <algorithm>
#include "radix.h"

using std::vector;
using std::string;

static int getDig(string &str, int place) {
  return ((unsigned) place < str.length() ? str[place] - 48 : 0);
}


/*
 * Just your baic insertion sort, grabbed from https://www.geeksforgeeks.org/insertion-sort/
 */
static void InsertionSort(vector<string>& strs) {
  int n = strs.size();
  int i, j;
   string key;
   for (i = 1; i < n; i++) {
       key = strs[i];
       j = i-1;

       while (j >= 0 && strs[j] > key) {
           strs[j+1] = strs[j];
           j = j-1;
       }
       strs[j+1] = key;
   }
}


void ParallelRadixSort::msd(std::vector<std::reference_wrapper<std::vector<unsigned int>>> &lists, unsigned int cores) {
  string pass = "blah blah blah";
  getDig(pass,2);
  vector<string> also;
  also.push_back(pass);
  InsertionSort(also);



  for (vector<unsigned int> list : lists) {
    vector<string> to_str;
    for (unsigned int i : list)
      to_str.push_back(std::to_string(i));
      std::sort(to_str.begin(),to_str.end());
  //  InsertionSort(to_str);
    for (unsigned int i = 0; i < list.size(); i++) {
      list[i] = (unsigned int) std::stoul(to_str[i]);
    }
    for (unsigned int i : list)
      std::cout << i << std::endl;
  }
}
