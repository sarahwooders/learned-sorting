// spreadsort float functor sorting example.
//
//  Copyright Steven Ross 2009.
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/sort for library home page.

// Caution: this file contains Quickbook markup as well as code
// and comments, don't change any of the special comment markups!

#include <boost/sort/spreadsort/spreadsort.hpp>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

using namespace boost::sort::spreadsort;

//[float_functor_types
#define CAST_TYPE int
#define KEY_TYPE float
//] [/float_functor_types]


//[float_functor_datatypes
//struct float {
//    KEY_TYPE key;
//    std::string data;
//};
//] [/float_functor_datatypes]


//[float_functor_rightshift
// Casting to an integer before bitshifting
struct rightshift {
  int operator()(const float &x, const unsigned offset) const {
    return float_mem_cast<KEY_TYPE, CAST_TYPE>(x) >> offset;
  }
};
//] [/float_functor_rightshift]

//[float_functor_lessthan
struct lessthan {
  bool operator()(const float &x, const float &y) const {
    return x < y;
  }
};
//] [/float_functor_lessthan]

// Pass in an argument to test std::sort
// Note that this converts NaNs and -0.0 to 0.0, so that sorting results are
// identical every time
void float_radix(float arr[], int n) {
  float_sort(arr, arr + n, rightshift(), lessthan());
}
