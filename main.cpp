#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <string>
#include <random>
#include <algorithm>
#include <parallel/algorithm>
#include <stdio.h>      /* printf */
#include <stdlib.h>
#include <cstdio>
#include <ctime>
#include <bitset>
#include <omp.h>
#include "read.cpp"
#include "generate_input.cpp"
#include "partition.cpp"
#include "sort.cpp"
#include "test.cpp"

// Initial version of full multicore system for sorting
// binary recoords only 

/* PARAMETERS */
int num_workers = 64;//omp_get_max_threads();
int num_samples = 64 * 1000;
int num_records = 64 * 156250; //1 GB
int num_records_per_worker = num_records/num_workers;
int num_partitions = num_workers;
std::string filename = "file";
std::string outfile = "outfile";
 
int main () {

  /* Set number of threads */
  omp_set_num_threads(num_workers);

  /* USE GENSORT TO GENERATE INPUT DATA*/
  generate_input(num_records, filename, num_workers);

  printf("INPUT GENERATION COMPLETE\n");
  double start = omp_get_wtime();


  /* READ GENERATE VALUES */
  std::vector<std::vector<int> > data; 
  data.resize(num_workers);
  read_data(data, num_workers, num_records_per_worker, filename);
  double t1 = omp_get_wtime() - start;
  std::cout << "READ INPUT " << t1 << std::endl;

  /* COMPUTE PARTITION VALUES */
  std::vector<int> partitions = pick_range_boundaries(data, num_samples, num_records, num_workers, num_partitions);
  double t2 = omp_get_wtime() - t1 - start;
  //for(int i = 0; i < partitions.size(); i ++) std::cout << partitions[i] << " ";
  //std::cout << std::endl; 
  std::cout<<  "SAMPLE AND PICK RANGES "<< t2 <<'\n';


  /* SEPERATE DATA INTO RANGE FILES */
  int records_per_worker = num_records/num_workers + 1;
  std::vector<std::vector<std::vector<int> > > range_partitions = output_ranges(data, partitions, filename, records_per_worker);
  double t3 = omp_get_wtime() - t2 - t1 - start;
  std::cout<<  "DIVIDE DATA INTO RANGES "<< t3 <<'\n';  

  /* SORT DATA */
  sort_ranges(range_partitions, num_workers, filename, outfile);
  double t4 = omp_get_wtime() - t2 - t3 - t1 - start;

  std::cout<<  "SORT AND WRITE OUTPUT "<< t4 <<'\n';
  /* RUN VALSORT TO VALIDATE SORT */



  double t = omp_get_wtime() - start;
  printf("Records: %i Samples: %i Workers: %i \n", num_records, num_samples, num_workers);
  std::cout<<"TOTAL "<< t <<'\n';
  std::cout << "Total without data reads" << (t -t1) << std::endl;
  std::cout << "data size " << sizeof(int) << '\n';
  // TESTS */
  //printf("\n\n ---------------------------------------------------\n TEST DATA \n ---------------------------------------------------\n");
  //int * data = new int[num_records];
  //read_file(data, filename + "0", num_records);
  //std::sort(data, data + num_records);
  //test_partitions(data, num_records, partitions);


  return 0;
}
