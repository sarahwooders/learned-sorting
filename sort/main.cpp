#include <vector>
#include <parallel/algorithm>
#include <iostream>
#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include<iostream>
#include <ctime>
/* Code from https://www.geeksforgeeks.org/radix-sort/
*/
using namespace std;
 

class Timer
{
public:
    Timer() { clock_gettime(CLOCK_REALTIME, &beg_); }

    double elapsed() {
        clock_gettime(CLOCK_REALTIME, &end_);
        return end_.tv_sec - beg_.tv_sec +
            (end_.tv_nsec - beg_.tv_nsec) / 1000000000.;
    }

    void reset() { clock_gettime(CLOCK_REALTIME, &beg_); }

private:
    timespec beg_, end_;
};


// A utility function to get maximum value in arr[]
int getMax(int arr[], int n)
{
    int mx = arr[0];
    for (int i = 1; i < n; i++)
        if (arr[i] > mx)
            mx = arr[i];
    return mx;
}
 
// A function to do counting sort of arr[] according to
// the digit represented by exp.
void countSort(int arr[], int n, int exp)
{
    int output[n]; // output array
    int i, count[10] = {0};
 
    // Store count of occurrences in count[]
    for (i = 0; i < n; i++)
        count[ (arr[i]/exp)%10 ]++;
 
    // Change count[i] so that count[i] now contains actual
    //  position of this digit in output[]
    for (i = 1; i < 10; i++)
        count[i] += count[i - 1];
 
    // Build the output array
    for (i = n - 1; i >= 0; i--)
    {
        output[count[ (arr[i]/exp)%10 ] - 1] = arr[i];
        count[ (arr[i]/exp)%10 ]--;
    }
 
    // Copy the output array to arr[], so that arr[] now
    // contains sorted numbers according to current digit
    for (i = 0; i < n; i++)
        arr[i] = output[i];
}
 
// The main function to that sorts arr[] of size n using 
// Radix Sort
void radixsort(int arr[], int n)
{
    // Find the maximum number to know number of digits
    int m = getMax(arr, n);
 
    // Do counting sort for every digit. Note that instead
    // of passing digit number, exp is passed. exp is 10^i
    // where i is current digit number
    for (int exp = 1; m/exp > 0; exp *= 10)
        countSort(arr, n, exp);
}
 
// A utility function to print an array
void print(int arr[], int n)
{
    for (int i = 0; i < n; i++)
        cout << arr[i] << " ";
}
 
// Driver program to test above functions
int main()
{

    int size = 100000; //0 - radix fails, 00 - quicksort fails
    int a1[size];
    int a2[size];
    for(int i = 0; i < size; i ++) {
      int x = rand() % 10 + 1;

      //int x = i;
      a1[i] = x;
      a2[x] = x;
    }


    int n = sizeof(a1)/sizeof(a1[0]);
    Timer tmr;
    radixsort(a1, n);
    double t = tmr.elapsed();
    std::cout << "Radix sort " <<  t << std::endl;
    //print(a2, n);

    n = sizeof(a2)/sizeof(a2[0]);
    tmr.reset();
    __gnu_parallel::sort(a2, a2 + n);
    t = tmr.elapsed();
    cout << "\nQuicksort " << t << std::endl;
    //print(a2, n);



    //vector<int> v(100);

    //for(int i = 0; i < v.size(); i ++) {
    //  v[i] = rand() % 100 + 1;
    //}

    //// ...
    //// Explicitly force a call to parallel sort.
    //tmr.reset();
    //__gnu_parallel::sort(v.begin(), v.end());
    //t = tmr.elapsed();
    //std::cout << t << std::endl;

  return 0;
}
