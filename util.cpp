#include <stdio.h>
#include <stdlib.h>
#include "test.cpp"
#define BUFFER_SIZE 100

void write_output(int *data, int n, int id, std::string file) {

  /* START TIMER */
  std::clock_t start;
  start = std::clock();

  char buffer[BUFFER_SIZE];
  std::ofstream f_valsort;
  std::ofstream f_readable;
  f_valsort.open(file);
  f_readable.open("readable"+file);
  for(int i = 0; i < n; i ++) {
    sprintf (buffer, "%i", data[i]);
    //itoa(data[i],buffer,10);
    f_valsort.write((char *)buffer, BUFFER_SIZE);
    f_readable << data[i] << "\n";
  }
  f_valsort.close(); 
  f_readable.close();
  double duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
  //printf("Took %f to write %i items\n", duration, n);
  test_output(data, n);
}


void write_output(std::vector<int> data, int id, std::string file) {

  /* START TIMER */
  std::clock_t start;
  start = std::clock();

  char buffer[BUFFER_SIZE];
  std::ofstream f_valsort;
  std::ofstream f_readable;
  f_valsort.open(file);
  f_readable.open("readable"+file);

  for(int i = 0; i < data.size(); i ++) {
    //itoa(data[i],buffer,10);
    sprintf (buffer, "%i", data[i]);
    f_valsort.write((char *)buffer, BUFFER_SIZE);
    f_readable << data[i] << "\n";
  }
  f_valsort.close(); 
  f_readable.close();
  double duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
  //printf("Took %f to write %i items\n", duration, n);
  test_output(data);
}


