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
#include <omp.h>
#include "read.cpp"


// Initial version of full multicore system for sorting
// binary recoords only 


void generate_input_data(std::string filename, int n, int id){
    std::string command = "./gensort -s " + std::to_string(n) + " " + filename + std::to_string(id);
    std::cout << command << std::endl;
    system(command.c_str());
}

void call_generate_input_data(int n, std::string filename, int num_workers){
  int num = n/num_workers; 
  int left = n - num * num_workers;
  #pragma omp parallel 
  {
    //#pragma omp for shared(num_workers, filename, num, left), private(i)
    //#pragma omp for

    //for(i = 0; i < num_workers; i ++) {
    int i = omp_get_thread_num();
    if(left > 0) {
      generate_input_data(filename, num + 1, i);
      left --;
    }
    else {
      generate_input_data(filename, num, i);
    }
    std::cout << "Worker " << i << " generating " << num << std::endl;
  }
}


/* Shuffle all numbers and take first subset_size of them - NOT FAST */
void generate_random_subset(int * indices, int subset_size, int n) {
  int * ind = new int[n];
  for(int i = 0; i < n; i ++) ind[i] = i;
 
  std::random_shuffle(ind, ind + n);
  for(int i = 0; i < subset_size; i ++) indices[i] = ind[i];
}
    

void read_file(int * data, std::string filename, int n) {
    printf("reading file %s\n", filename.c_str());
    read_gensort(data, n, filename);
    //for(int i = 0; i < n; i ++) {
    //    std::cout << data[i] << " ";
    //}
    //std::cout << std::endl << std::endl << "READ DONE" << std::endl;
}

void collect_samples(int * data, int * samples, int data_n, int samples_n) {
  int * sample_indices = new int[data_n];
  generate_random_subset(sample_indices, samples_n, data_n); // do I need to do this every time?
 
  for(int i = 0; i < samples_n; i ++) {
    int index = sample_indices[i];
    samples[i] = data[index];
  }
}

/*
*/
std::vector<int> pick_range_boundaries(int num_samples, int num_records, int num_workers, int num_partitions, std::string filename) {
   
   int * all_samples = new int[num_samples]; 

   int samples_per_worker = num_samples/num_workers;
   int records_per_worker = num_records/num_workers + 1; //PADDING

   // Collect samples from all files/workers

   //#pragma omp parallel
   //#pragma omp for
   //#pragma omp parallel for
   #pragma omp parallel
   {
   //for(int i = 0; i < num_workers; i ++) {
     int i = omp_get_thread_num();
     std::string file = filename + std::to_string(i);
     int * data = new int[records_per_worker];
     int * samples = new int[samples_per_worker];
 
     // Read the file for the process
     read_file(data, file, records_per_worker);
 

     printf("COLLECTING SAMPLES\n"); 
     //Collect the samples from the data
     collect_samples(data, samples, records_per_worker, samples_per_worker);

     printf("ADDING TO ALL SAMPLES\n");
     // Add to total samples array  
     for(int j = 0; j < num_samples; j ++) {
       all_samples[i*samples_per_worker + j] = samples[j];
     }
   }
   printf("DONE PARALLEL");
  
   // sort total samples
   __gnu_parallel::sort(all_samples, all_samples + num_samples);
   //std::sort(all_samples, all_samples + num_samples);

   // Determine partition values
   std::vector<int> partitions; 
   int partition_size = num_samples/num_partitions;
   for(int i = partition_size; i < num_samples; i += partition_size) {
      std::cout << " HERE " << i << " " ;
      partitions.push_back(all_samples[i]);
   }
   
   return partitions;
}

void test_partitions(int * sorted_data, int n, std::vector<int> partitions) { 

    int p_index = 0;
    int p_size = partitions.size() + 1;
    std::cout << "SIZE " << p_size << " " << partitions.size() << std::endl;
    int * count = new int[p_size]; 
    for(int i = 0; i < p_size; i ++) count[i] = 0;
    for(int i = 0; i < n; i ++) {
      while(sorted_data[i] > partitions[p_index] && p_index < partitions.size()) {
        p_index++;
        std::cout << "index " << p_index << " " << partitions[p_index] << std::endl;
      }
     

      count[p_index]++;
    }
    std::cout << "PARTITION COUNTS " << std::endl;
    for(int i = 0; i < p_index; i ++) std::cout << count[i] << " " << std::endl;
}

int main () {
  std::clock_t start;
  double duration;
  start = std::clock();

  int num_records = 10000;
  int num_workers = omp_get_max_threads();
  int num_samples = 1000;
  int num_partitions = num_workers;
  std::string filename = "file";
  call_generate_input_data(num_records, filename, num_workers);

  std::cout << "DATA GENERATED" << std::endl;
  std::vector<int> partitions = pick_range_boundaries(num_samples, num_records, num_workers, num_partitions, filename);


  std::cout << std::endl << "PARTITIONS \n";
  for(int i = 0; i < partitions.size(); i ++) std::cout << partitions[i] << " ";
  std::cout << std::endl; 

  duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
  std::cout<<"printf: "<< duration <<'\n';


  /* TESTS */
  int * data = new int[num_records];
  read_file(data, filename + "0", num_records);
  std::sort(data, data + num_records);
  test_partitions(data, num_records, partitions);

  
  //std::ifstream is ("file.dat", std::ifstream::binary);
  //if (is) {
  //  // Determine the file length
  //  is.seekg(0, std:ios_base::end);
  //  std::size_t size=is.tellg();
  //  is.seekg(0, std::ios_base::beg);
  //  // Create a vector to store the data
  //  std::vector<float> v(size/sizeof(float));
  //  // Load the data
  //  is.read((char*) &v[0], size);
  //  // Close the file
  //  is.close();
  //}

  return 0;
}
