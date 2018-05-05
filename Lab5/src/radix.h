/*
 * Copyright (C) 2018 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 */

#include <vector>
#include <functional>
#include <condition_variable>
/*
 * Simple multi-threaded Radix Sort with support for Most Significant Digit
 * sorting only.
 */
class RadixSort {
private:
    // static void msd(std::vector<unsigned int> &arr, unsigned int);
    // static void msd(std::vector<unsigned int> &arr);
    // static void msdStrings(std::vector<std::string> &, unsigned int);
    // static unsigned int maxLengthInt(const std::vector<std::string> &arr);
    // static unsigned int maxLengthInt(const std::vector<unsigned int> &arr);
  //  void printall(std::vector<unsigned int> & arr);
    const int CORES;
    // std::condition_variable cores_available;
    // std::mutex mtx;
public:
    /*
     * Create a multi-threaded RadiX Sort restricted to using no more than
     * CORES processor cores.
     */
    RadixSort(const unsigned int cores);

    /*
     * Perform an in-place Most Significant Digit Radix Sort on each list of
     * unsigned integers in LISTS.
     */
    void msd(std::vector<std::reference_wrapper<std::vector<unsigned int> > > &lists);
};
