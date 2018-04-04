#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <string>
#include <random>
#include <algorithm>
#include <stdio.h>      /* printf */
#include <stdlib.h>
#include "read.cpp"


// Initial version of full multicore system for sorting
// binary recoords only 


void generate_input_data(std::string filename, int n, int id){
    std::string command = "./gensort -s " + std::to_string(n) + " " + filename + std::to_string(id);
    system(command.c_str());
}

void call_generate_input_data(int n, std::string filename, int num_workers){
  int num = n/num_workers; 
  int left = n - num * num_workers;
  for(int i = 0; i < num_workers; i ++) {
    if(left > 0) {
      generate_input_data(filename, num + 1, i);
      left --;
    }
    else {
      generate_input_data(filename, num, i);
    }
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
    for(int i = 0; i < n; i ++) {
        std::cout << data[i] << std::endl;
    }
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
  num_samples: number of records PER worker
  num_samples: number of samples PER worker (total = num_workers * num_samples)
*/
std::vector<int> pick_range_boundaries(int num_samples, int num_records, int num_workers, int num_pertitions, std::string filename) {
   
   int * all_samples = new int[num_samples]; 
   int index = 0; 

   int samples_per_worker = num_samples/num_workers;
   int records_per_worker = num_records/num_workers;

   // Collect samples from all files/workers
   for(int i = 0; i < num_workers; i ++) {
     std::string file = filename + std::to_string(id);
     int * data = new int[records_per_worker];
     int * samples = new int[samples_per_worker];
 
     // Read the file for the process
     read_file(data, file, records_per_worker);
 
     //Collect the samples from the data
     collect_samples(data, samples, records_per_worker, samples_per_worker);

     // Add to total samples array  
     for(int j = 0; j < num_samples; j ++) {
       all_samples[index] = samples[j];
       index++;
     }
   }
  
   // sort total samples
   std::sort(all_samples, all_samples + num_samples);

   // Determine partition values
   std::vector<int> partitions; 
   int partition_size = num_samples/num_partitions;
   for(int i = 0; i < num_samples; i += partition_size) {
      partitions.push_back(all_samples[i];
   }
   
   return partitions;
}

void create_range_files(int num_samples, int num_records, std::string filename, int id) {
  std::string file = filename + std::to_string(id);
  int * data = new int[num_records];
  int * samples = new int[num_samples];

  // Read the file for the process
  read_file(data, file, num_records); 
 
  //Collect the samples from the data
  collect_samples(data, samples, num_records, num_samples);

   //write samples
  std::string output_file = file + std::to_string(id);
  write(samples, output_file);

}

  

int main () {
  int n = 100;
  std::string filename = "file";
  int * data = new int[n];
  call_generate_input_data(n, filename, 1);
  read_file(data, filename, n);
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
