/*
Copyright 2011 Erik Gorset. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of
      conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice, this list
      of conditions and the following disclaimer in the documentation and/or other materials
      provided with the distribution.

      THIS SOFTWARE IS PROVIDED BY Erik Gorset ``AS IS'' AND ANY EXPRESS OR IMPLIED
      WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
      FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL Erik Gorset OR
      CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
      CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
      SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
      ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
      NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
      ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

      The views and conclusions contained in the software and documentation are those of the
      authors and should not be interpreted as representing official policies, either expressed
      or implied, of Erik Gorset.
*/

#include <iostream>
#include <algorithm>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/time.h>
#include <vector>
#include <parallel/algorithm>
//#include <boost/range/algorithm.hpp>
//#include <boost/sort/spreadsort/integer_sort.hpp>
#include "radix.cpp"
#include "radix3.cpp"
#include "wordpress-radix.c"
//#include "float_sort.cpp"
#include "fast_radix.cpp"
#include "read.cpp"
#define DATA_TYPE int


void print(int arr[], int n) {
    for(int i = 0; i < n; i ++) {
        cout << arr[i] << ", ";
    }
    cout << std::endl;
}

void print(std::vector<DATA_TYPE> arr, int n) {
    for(int i = 0; i < n; i ++) {
        cout << arr[i] << ", ";
    }
    cout << std::endl;
}



void learnedSort(DATA_TYPE arr[], DATA_TYPE res[], int n) { //assume res initialized -1

  std::vector<DATA_TYPE> other;

  int temp[n] = {-1};

  for(int i = 0; i < n; i ++) {
     // std::cout << i << std::endl;
      int index = (int)(arr[i]);
      if(temp[index] < 0) {
        temp[index] = arr[i];
      } else {
         other.push_back(arr[i]);
      }
  }

  int i1 = 0;
  int i2 = 0;
  int i = 0;
  while(i1 < n || i2 < n) {
    if(i1 < n && i2 >= n) {
      res[i] = temp[i1];
      i1 ++;
    }
    else if(i1 >= n && i2 < n){
      res[i] = other[i2];
      i2 ++;
    }
    else {
      if(temp[i1] < other[i2]) {
         res[i] = temp[i1];
         i1 ++;
      } else {
         res[i] = other[i2];
         i2 ++;
      }
    }
    i ++;
  }
}

void learnedSort(std::vector<DATA_TYPE> & arr, std::vector<DATA_TYPE> & res, int n) {
  
  int max_index = n;
  //for(int i = 0; i < arr.size(); i ++) {
  //    if((int)(arr[i]) > max_index) max_index = (int)(arr[i]);
  //}

  int *extra = (int *)malloc(sizeof(int) * n);
  //int extra[n + 1] = {0};
  //std::vector<int> temp(max_index + 1);

  //for(int i = 0; i < n; i ++) res[i] = -1;
  
  for(int i = 0; i < n; i ++) {
      int val = (int)(arr[i]);
      extra[val]++;
  }

  int index = 0;
  for(int i = 0; i < n; i ++) {
      for(int j = 0; j < extra[i]; j ++) {
          res[index] = i;
          index ++;
      }
  }
  std::cout << "inter ";
  //print(res, res.size());

  //int offset = 0;
  //for(int i = 0; i < res.size(); i ++) {
  //    if(res[i] < 0) offset++;
  //    else res[i - offset] = res[i];
  // }
  //res.resize(n);
}
//void radix(std::vector<int> & arr, int n) {
//    boost::sort::spreadsort::integer_sort(arr.begin(), arr.end());
//
//}
//
//void radix(std::vector<float> & arr, int n) {
//    //float_radix(arr, n);
//    boost::sort::spreadsort::float_sort(arr.begin(), arr.end());
//}




//will have duplicates
void generate_random(std::vector<DATA_TYPE> & arr, int n, int range) {
    for(int i = 0; i < n; i ++) {
        arr[i] = (DATA_TYPE)(rand() % range);
    }
}
void generate_random(DATA_TYPE arr[], int n, int range) {
    for(int i = 0; i < n; i ++) {
        arr[i] = (DATA_TYPE)(rand() % range);
    }
}
//wont have duplicates
void generate_sequential(std::vector<DATA_TYPE> & arr, int n) {
    for(int i = 0; i < n; i ++) {
        arr[i] = (DATA_TYPE)(i);
    }
}


int main(int argc, char **argv) {

    bool duplicates = false;
    int num_remove = 0;

    if (sizeof(int) != 4) {
        std::cerr << "Ooops. sizeof(int) != 4\n";
        return 1111;
    }

    int N;
    if (argc != 2 || sscanf(argv[1], "%d", &N) == -1) {
        std::cerr << "n missing\n";
        return 111;
    }

    std::cout << "TEST " << N << "---------------" << std::endl;
    DATA_TYPE *array = (DATA_TYPE *)malloc(sizeof(DATA_TYPE) * N);
    //DATA_TYPE *res = (DATA_TYPE *)malloc(sizeof(DATA_TYPE) * N);
    //std::vector<DATA_TYPE> array(N + num_remove);
    std::vector<DATA_TYPE> res(N + num_remove);

    for (int i=0; i<4; ++i) {
        int n = N;
        srand(1);
        for(int i = 0; i < res.size(); i ++) res[i] = -1;
        srand ( time(NULL) );
        read_gensort(array, n);
        //generate_random(array, n, n);
        //std::cout << "size " << array.size() << std::endl;
        //print(array, n);
        //std::random_shuffle(array.begin(), array.end());
        //array.resize(array.size() - num_remove);
        //print(array, array.size());

        struct timeval start, end;
        time_t mtime, seconds, useconds;   
        gettimeofday(&start, NULL);
        switch(i) {
        case 0: std::sort(array, array + n); break;
        //case 1: qsort(array, N, sizeof(int), intcmp); break;
        case 1: radix_sort(array, 0, N, 24); break;
        //case 1: radix_sort_3(array, n); break;
        //case 1: radix(array, N); break;
        case 2: omp_lsd_radix_sort(array, N);
        case 3: __gnu_parallel::sort(array, array + N);
        //case 2: msd_radix_sort(array, array + N); break;
        //case 2: boost::sort::spreadsort::integer_sort(array, array + N); break;
        //case 2: learnedSort(array, res, N); break;
        }
        gettimeofday(&end, NULL);
        n = N - 1;
        //print(res, N);
        while (n --> 0) {
            if (i != 2 && array[n] > array[n+1]) {
                std::cerr << array[n] << " sorting failed\n";
                return -1;
            }
            else if(i == 2 && res[n] > res[n+1]) {
                std::cerr << "sorting failed\n";
                return -1;
            }
        }

        seconds  = end.tv_sec  - start.tv_sec;
        useconds = end.tv_usec - start.tv_usec;
        mtime = seconds * 1000000 + useconds;
        switch(i) {
        case 0: std::cout << "std::sort " << mtime << "\n"; break;
        case 1: std::cout << "radix " << mtime << "\n"; break;
        case 2: std::cout << "parallel radix " << mtime << "\n"; break;
        case 3: std::cout << "parallel sort" << mtime << "\n"; break;
        }
    }
    std::cout << std::endl << std::endl;

    return 0;
}
