/*
 * Copyright (C) 2018 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without
 * the express written permission of the copyright holder.
 */

#include <vector>
#include <functional>

/*
 * Simple multi-threaded Radix Sort with support for Most Significant Digit
 * sorting only.
 */
class RadixSort {
private:
    void msd(std::vector<unsigned int> &arr, unsigned int);
    void msd(std::vector<unsigned int> &arr);
    static unsigned int maxLengthInt(const std::vector<unsigned int> &arr);
    static void printall(std::vector<unsigned int> & arr);
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
    void msd(std::vector<std::reference_wrapper<std::vector<unsigned int>>> &lists);
};
