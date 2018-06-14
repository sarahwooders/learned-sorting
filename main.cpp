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

// Initial version of full multicore system for sorting
// binary recoords only 

/* PARAMETERS */
int num_workers = 10;//omp_get_max_threads();
int num_samples = num_workers * 10000;
int num_records = num_workers * 1562500; //1 GB
int num_records_per_worker = num_records/num_workers;
int num_partitions = num_workers;
std::string filename = "file";
std::string outfile = "outfile";

enum sorting_mode{NONE, UNIFORM};
sorting_mode mode = NONE;
 
int main () {


  /* Set number of threads */
  omp_set_num_threads(num_workers);

  /* USE GENSORT TO GENERATE INPUT DATA*/
  generate_input(num_records, filename, num_workers);

  printf("INPUT GENERATION COMPLETE\n");
  double start = omp_get_wtime();

  double t_input_generation, t_sample_generation, t_compute_partitions, t_model, t_partition_data, t_sort;


  /* READ GENERATE VALUES */
  t_input_generation = omp_get_wtime();
  std::vector<std::vector<int> > data; 
  data.resize(num_workers);
  read_data(data, num_workers, num_records_per_worker, filename);
  t_input_generation = omp_get_wtime() - t_input_generation;

  /* GENERTE SAMPLES */
  t_sample_generation = omp_get_wtime();
  int * samples = generate_samples(data, num_samples, num_records, num_workers);
  t_sample_generation = omp_get_wtime() - t_sample_generation;

  /* COMPUTE PARTITION VALUES */
  t_compute_partitions = omp_get_wtime();
  std::vector<int> partitions = pick_range_boundaries(samples, num_samples, num_partitions);
  t_compute_partitions = omp_get_wtime() - t_compute_partitions;


  if(mode != NONE) {

    /* GENERATE MODEL */
    t_model = omp_get_wtime();
    UniformModel model = generate_model(samples, num_samples, num_records);
    t_model = omp_get_wtime() - t_model;

    /* SEPERATE DATA INTO RANGE FILES */
    t_partition_data = omp_get_wtime();
    int records_per_worker = num_records/num_workers + 1;
    std::vector<std::vector<std::vector<int> > > range_partitions = output_ranges(data, partitions, filename, records_per_worker, model);
    t_partition_data = omp_get_wtime() - t_partition_data;

    /* SORT DATA */
    t_sort = omp_get_wtime();
    sort_ranges(range_partitions, num_workers, filename, outfile);
    t_sort = omp_get_wtime() - t_sort;

  } else {

    t_model = 0;

    /* SEPERATE DATA INTO RANGE FILES */
    t_partition_data = omp_get_wtime();
    int records_per_worker = num_records/num_workers + 1;
    std::vector<std::vector<std::vector<int> > > range_partitions = output_ranges(data, partitions, filename, records_per_worker);
    t_partition_data = omp_get_wtime() - t_partition_data;

    /* SORT DATA */
    t_sort = omp_get_wtime();
    sort_ranges(range_partitions, num_workers, filename, outfile);
    t_sort = omp_get_wtime() - t_sort;
  }
  
  printf("READ INPUT %f \n", t_input_generation);
  printf("SAMPLE GENERATION %f\n", t_sample_generation);
  printf("COMPUTER PARTITIONS %f\n", t_compute_partitions); 
  printf("GENERATE MODEL %f\n", t_model);
  printf("DIVIDE DATA INTO RANGES %f \n", t_partition_data);
  printf("SORT AND WRITE OUTPUT %f \n", t_sort);  


  /* RUN VALSORT TO VALIDATE SORT */

  double t = omp_get_wtime() - start;
  printf("Records: %i Samples: %i Workers: %i \n", num_records, num_samples, num_workers);
  std::cout<<"TOTAL "<< t <<'\n';
  std::cout << "data size " << sizeof(int) << '\n';
  // TESTS */
  //printf("\n\n ---------------------------------------------------\n TEST DATA \n ---------------------------------------------------\n");
  //int * data = new int[num_records];
  //read_file(data, filename + "0", num_records);
  //std::sort(data, data + num_records);
  //test_partitions(data, num_records, partitions);


  return 0;
}
