/*
 * Copyright (C) 2018 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 */

#include <vector>
#include <functional>

/*
 * Parallel Radix Sort with support for Most Significant Digit sorting only.
 *
 * If allowed to use ten cores or more, should sort a large set of randomly
 * selected unsigned integers apromixmately ten times faster than a single
 * threaded sort of the same random set.
 *
 * Whilst the choice of sorting algorithnm is a matter or personal choice,
 * a bucket sort where each bucket is sorted in a different thread will almost
 * certainly deliver the best results.
 */
class ParallelRadixSort {
private:
  // static void something(int h);
  // static void nothing();
  // static void stringy(std::string&);
  // static void vectStringy(std::vector<std::string>);
  // static void refvectstring(std::vector<std::string>&);
  // static void sort(std::vector<std::string>&);

  // static void msd(std::vector<std::string>&);
  // static void msd(std::vector<std::string>&, unsigned int);
public:
    /*
     * Perform an in-place Most Significant Digit Radix Sort on each list of
     * unsigned integers in LISTS using nore that CORES cpu cores.
     */
    void msd(std::vector<std::reference_wrapper<std::vector<unsigned int>>> &lists, const unsigned int cores);
};
