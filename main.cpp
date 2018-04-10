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
    //MAKE SKEWED LATER WITH -s
    std::string command = "./gensort -b" + std::to_string(n*id) + " " + std::to_string(n) + " " + filename + std::to_string(id);
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
    read_gensort(data, n, filename);
}

/*
Function for selecting random subset of data (DO NOT USE)
*/
void collect_samples(int * data, int * samples, int data_n, int samples_n) {
  int * sample_indices = new int[data_n];
  generate_random_subset(sample_indices, samples_n, data_n); // do I need to do this every time?
 
  for(int i = 0; i < samples_n; i ++) {
    int index = sample_indices[i];
    samples[i] = data[index];
  }
}

/*
Return vector of range boundary values
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
 
     //Collect the samples from the data
     //collect_samples(data, samples, records_per_worker, samples_per_worker);

     // Add to total samples array  
     for(int j = 0; j < samples_per_worker; j ++) {
       //printf("data index %i %i %i %i\n", i, j, data[j], i*samples_per_worker + j);
       all_samples[i*samples_per_worker + j] = data[j];
       //all_samples[i*samples_per_worker + j] = samples[j];
     }
   }
  
   // sort total samples
   __gnu_parallel::sort(all_samples, all_samples + num_samples);
   //std::sort(all_samples, all_samples + num_samples);

   // Determine partition values
   std::vector<int> partitions; 
   int partition_size = num_samples/num_partitions;
   for(int i = partition_size; i < num_samples; i += partition_size) {
      partitions.push_back(all_samples[i]);
   }
   //for(int i = 0; i < partitions.size(); i ++) printf("part %i\n",partitions[i]);
   
   return partitions;
}

void output_ranges(std::vector<int> partitions, std::string filename, int records_per_worker) {

 #pragma omp parallel
 {
   int i = omp_get_thread_num();
   std::string file = filename + std::to_string(i);


   //Read data (SHOULD NOT HAVE TO DO THIS AGIAN! REMOVE!
   read_file(data, file, records_per_worker);
 

   // Write data in sepearte partitions to seperate files
   int num_files = partitions.size() + 1;
   ofstream *outfiles = new ofstream [num_files];
   for(int x = 0; x < num_files; x++) {
     ofstream outfile;
     std::string outfile_name = filename + std::to_string(i) + "-" + std::to_string(x);  
     outfile.open(outfile_name);
     out_files[x] = outfile; 
   }

   for(int i = 0; i < records_per_worker; i ++) {
     int index = 0;
     while(index < partitions.size() && partitions[index] < data[i]) { //CAN BE MADE FASTER (binary search) 
       index ++;
     }
     outfiles[index] << data[i] << "\n";
   }
   //Close all open files
   for(ofstream outfile : outfiles) outfile.close();
  }
}

void sort(int *data, int n, int lower, int upper) {
   //NEED TO DO RANGE PARTITION
   std::sort(data, data + n);
}

void write_output(int *data, int n, int id, std::string filename) {
  std::string file = filename + std::to_string(id);
  ofstream f;
  f.open(file);
  for(int i = 0; i < n; i ++) f << data[n] << "\n";
  f.close(); 
}

void validate(int num_workers, std::string filename) {
  std::string command;
  std::string cat = "";
  for(int i = 0; i < num_workers; i ++) {
    command = "valsort -o " + filename + std::to_string(i) + ".sum " + filename + std::to_string(i) + ".dat";
    system(command.c_str());
    cat += filename + std::to_string(i) + ".sum "
  }
  command = "cat " + cat + "> all.sum";
  system(command.c_str());
  command = "valsort -s all.sum";
  system(command.c_str());
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
      }
      count[p_index]++;
    }
    std::cout << "PARTITION COUNTS " << std::endl;
    for(int i = 0; i < p_index; i ++) printf("worker%i load: %i \n", i, count[i]);
}

int main () {

  /* PARAMETERS */
  int num_records = 1000000;
  int num_workers = omp_get_max_threads();
  int num_samples = 10000;
  int num_partitions = num_workers;
  std::string filename = "file";
 
  /* USE GENSORT TO GENERATE INPUT DATA*/
  call_generate_input_data(num_records, filename, num_workers);

  /* START TIMER */
  std::clock_t start;
  double duration;
  start = std::clock();

  /* COMPUTE PARTITION VALUES */
  std::vector<int> partitions = pick_range_boundaries(num_samples, num_records, num_workers, num_partitions, filename);

  //for(int i = 0; i < partitions.size(); i ++) std::cout << partitions[i] << " ";
  //std::cout << std::endl; 

  /* SEPERATE DATA INTO RANGE FILES */
  int records_per_worker = num_records/num_workers + 1;
  output_ranges(partitions, filename, records_per_worker);

  duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
  std::cout<<"printf: "<< duration <<'\n';


  /* TESTS */
  printf("\n\n ---------------------------------------------------\n TEST DATA \n ---------------------------------------------------\n");
  int * data = new int[num_records];
  read_file(data, filename + "0", num_records);
  std::sort(data, data + num_records);
  test_partitions(data, num_records, partitions);


  return 0;
}
