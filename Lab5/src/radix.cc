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

RadixSort::RadixSort(const unsigned int cores) {
  
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
        unsigned int ui = (unsigned int) std::stol(s);
        list.push_back(ui);
      }
    );
  };




  std::vector<thread> m;
  for (std::reference_wrapper<vector<unsigned int>> list : lists) {
    m.push_back(thread(sort, list));
  }
  for (auto &thread : m) {
    thread.join();
  }

// for (std::reference_wrapper<vector<unsigned int>> list : lists) {
//   sort(list.get());
// }

  //
  // for (unsigned int j : lists[0].get())
  //   cout << j << std::endl;
  //
  // // thread t1{ [&] {
  // //    msd(lists[0].get());
  // //  }};
  // //
  // //
  // // thread* t2 = new thread { [&] {
  // //   msd(lists[1].get());
  // // }};
  //
  // int x = 0;
  // auto something = [lists, &x] {
  //   cout << "\nsize: ayy lmao : " << lists.size() << std::endl;
  //   cout << "\nLet's give nothing a try ayy lmao\n";
  //   nothing();
  //   cout << lists[0].get()[x] << std::endl;
  // };
  //
  //
  // //thread t3{something};
  // int y = 0;
  // thread t3 = thread {[lists, &y] {
  //   cout << "\nsize: ayy lmao : " << lists.size() << std::endl;
  //   cout << "\nLet's give nothing a try ayy lmao\n";
  //   nothing();
  //   cout << lists[0].get()[y] << std::endl;
  // //  msd(lists[0].get());
  // }};
  //
  // t3.join();
  //
  // thread t4 = thread{ [&] {
  //   msd(lists[0].get());
  //   }
  // };
  // t4.join();
  //
  // t4 = thread{ [&] {
  //   msd(lists[0].get());
  //   }
  // };
  // t4.join();


  // thread* t0 =  new thread {[&] {
  //   msd(lists[0]);
  //   }
  // };
  // // t0.join();
  //
  // thread* t1 = new thread {[&] {
  //   msd(lists[1]);
  //   }
  // };
  // // t1.join();
  //
  //
  // thread* t2 = new thread {[&] {
  //   msd(lists[2]);
  //   }
  // };
  // // t2.join();
  //
  // thread* t3 = new thread {[&] {
  //   msd(lists[3]);
  //   }
  // };
  // // t3.join();
  //
  // thread* t4 = new thread {[&] {
  //   msd(lists[4]);
  //   }
  // };
  // // t4.join();
  //
  //
  // thread* t5 = new thread {[&] {
  //   msd(lists[5]);
  //   }
  // };
  // // t5.join();
  //
  //
  // thread* t6 = new thread {[&] {
  //   msd(lists[6]);
  //   }
  // };
  // // t6.join();
  //
  //
  // thread* t7 = new thread {[&] {
  //   msd(lists[7]);
  //   }
  // };
  // // t7.join();
  //
  //
  // thread* t8 = new thread {[&] {
  //   msd(lists[8]);
  //   }
  // };
  // // t8->join();
  //
  // thread* t9 = new thread {[&] {
  //   msd(lists[9]);
  //   }
  // };
  //
  // vector<thread*> threds;
  // threds.push_back(t0);
  // threds.push_back(t1);
  // threds.push_back(t2);
  // threds.push_back(t3);
  // threds.push_back(t4);
  // threds.push_back(t5);
  // threds.push_back(t6);
  // threds.push_back(t7);
  // threds.push_back(t8);
  // threds.push_back(t9);
  // for (thread* t : threds)  {
  //   t->join();
  // }



  //array of pointers


  // thread* t[lists.size()];
  // for (unsigned int i = 0; i < lists.size(); i++) {
  //   t[i] = new thread {[&] { msd(lists[i]);}};
  //   t[i]->join();
  // }
  //t[0]->join();
//  t[1]->join();

  // for (unsigned int i = 0; i < lists.size(); i++) {
  //   t[i]->join();
  // }

  // t0.join();
  // t1.join();
  // t2.join();
  // t3.join();
  // t4.join();
  // t5.join();
  // t6.join();
  // t7.join();
  // t8->join();
  // t9->join();


  // vector<thread*> threadPointers;
  // for (unsigned int i = 0; i < lists.size(); i++) {
  //   thread* p = new thread {[&] { msd(lists[i]);} };
  //   threadPointers.push_back(p);
  // }

  //threadPointers[0]->join();
  //threadPointers[1]->join();

  #ifdef DB
  cout << "Lists.size = " << lists.size() <<". So there should be " << lists.size() << " threads\n";
  #endif


  //reference bug: The created thread falls out of scope and is removed and the
  // end of the for-loop

  //
  // thread t;
  // vector<std::reference_wrapper<thread>> threads;
  // for (std::reference_wrapper<vector<unsigned int>> listRef : lists) {
  //   t = thread {[&] { msd(listRef.get()); } };
  //   threads.push_back(t);
  //   //t.join();
  // //  threads[threads.size()-1].get().join();
  //   #ifdef DB
  //   cout << "Threads.size = " << threads.size() << ". There are " << threads.size() << " threads...\n";
  //   #endif
  // }
  //
  // for (std::reference_wrapper<thread> thread : threads) {
  //   thread.get().join();
  // }


  //vector of pointers Pointers feel like they should work, but hikey dont.
  //... idk why

  //
  // vector<thread*> threadPointers;
  // for (std::reference_wrapper<vector<unsigned int>> listRef : lists) {
  // //for (unsigned int i = 0; i < lists.size(); i++) {
  //   thread * p = new thread {[&] { msd(listRef.get()); } };
  //   threadPointers.push_back(p);
  //   // p->join();
  //   //threadPointers[threadPointers.size() - 1]->join();
  // }
  // for (thread* thread : threadPointers) {
  //   thread->join();
  //   delete thread;
  // }

  #ifdef DB

  int i = 0;
  for (vector<unsigned int> list : lists) {
    cout << std::endl << "List # " << i++ << std::endl;
    printall(list);
  }
  #endif



  //
  // threads[0].get().join();
  // threads[1].get().join();
  // threads[2].get().join();

  //for (std::reference_wrapper<thread> threadRef : threads)
    //threadRef.get().join();

}
