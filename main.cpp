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
#include "generate_input.cpp"


// Initial version of full multicore system for sorting
// binary recoords only 

/* Shuffle all numbers and take first subset_size of them - NOT FAST */
void generate_random_subset(int * indices, int subset_size, int n) {
  int * ind = new int[n];
  for(int i = 0; i < n; i ++) ind[i] = i;
 
  std::random_shuffle(ind, ind + n);
  for(int i = 0; i < subset_size; i ++) indices[i] = ind[i];
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
std::vector<int> pick_range_boundaries(std::vector<std::vector<int> > & data, int num_samples, int num_records, int num_workers, int num_partitions) {
   
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
 
     // Add to total samples array  
     for(int j = 0; j < samples_per_worker; j ++) {
       //printf("data index %i %i %i %i\n", i, j, data[j], i*samples_per_worker + j);
       all_samples[i*samples_per_worker + j] = data[i][j];
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

int binarySearch(std::vector<int> & arr, int x)
{
    int l = 0;
    int r = arr.size() - 1;
    while (l <= r)
    {
        int m = l + (r-l)/2;
 
        // Check if x is present at mid
        if (arr[m] >= x && m == 0) {
            //std::cout << "Reruning A" << m << std::endl;
            return m;
        }

        if(arr[m] <= x && m == arr.size() - 1) {
            //std::cout << "Reruning B" << m + 1 << std::endl;
            return m + 1;
        }
    
        if(arr[m] >= x && arr[m - 1] <= x)  {
            //std::cout << "Reruning C "<< arr[m] << " " << arr[m - 1] << " " << x  << " " << m << std::endl;
            return m;
        }
     
     
 
        // If x greater, ignore left half
        if (arr[m] < x)
            l = m + 1;
 
        // If x is smaller, ignore right half
        else
            r = m - 1;
    }
 
    // if we reach here, then element was
    // not present
    return -1;
}

std::vector<std::vector<std::vector<int> > > output_ranges(std::vector<std::vector<int> > & data, std::vector<int> & partitions, std::string filename, int records_per_worker) {

 int num_files = partitions.size() + 1;
 int num_workers = data.size();
 
 std::vector<std::vector<std::vector<int> > > range_partitions;
 range_partitions.resize(num_workers);

 for(auto& v : range_partitions) v.resize(num_files);

 #pragma omp parallel shared(range_partitions)
 {
   int i = omp_get_thread_num();

   //std::vector<int> worker_data = data[i];

   // Write data in sepearte partitions to seperate files
   //std::ofstream outfiles[num_files];
   //for(int x = 0; x < num_files; x++) {
   //  //std::ofstream outfile;
   //  std::string outfile_name = filename + std::to_string(i) + "-" + std::to_string(x);  
   //  //outfile.open(outfile_name);
   //  outfiles[x].open(outfile_name); 
   //}

  std::clock_t start;
  double duration;
  start = std::clock();

  std::vector<std::vector<int> > v;
  v.resize(num_files);
  
   for(int j = 0; j < records_per_worker; j ++) {

     int index = binarySearch(partitions, data[i][j]);
     //printf("Search: index %i data %i\n", index, data[i][j]);
     //while(index < partitions.size() && partitions[index] < data[i][j]) { //CAN BE MADE FASTER (binary search) 
     //  index ++;
     //}

     v[index].push_back(data[i][j]); 
   }
    double t2 = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
    //printf("loop time %f \n", t2);
    for(int x = 0; x < v.size(); x ++) range_partitions[i][x] = v[x];
    double t3 = ( std::clock() - start ) / (double) CLOCKS_PER_SEC - t2;
    //printf("assignment time %f\n", t3);
 
   //Close all open files
   //for(auto& outfile : outfiles) outfile.close();
  }

  return range_partitions;
}

void sort(int *data, int n) {
  
   //NEED TO DO RANGE PARTITION
   double start = omp_get_wtime();
   std::sort(data, data + n);
   printf("Sorting time %f\n", start - omp_get_wtime());
}

void write_output(int *data, int n, int id, std::string file) {

  /* START TIMER */
  std::clock_t start;
  start = std::clock();

  std::ofstream f;
  f.open(file);
  for(int i = 0; i < n; i ++) {
    f << data[i] << "\n";
  }
  f.close(); 
  double duration = ( std::clock() - start ) / (double) CLOCKS_PER_SEC;
  //printf("Took %f to write %i items\n", duration, n);
}

void sort_ranges(std::vector<std::vector<std::vector<int> > > & range_partitions, int num_workers, std::string filename, std::string outfile) {
 #pragma omp parallel
 {
   
   double start = omp_get_wtime();
   int i = omp_get_thread_num();
   std::vector<int> data_vec;
   for(int x = 0; x < num_workers; x ++) {
     //std::string file = filename + std::to_string(x) + "-" + std::to_string(i);
     //std::vector<int> range_data = read_txt(file);
     data_vec.reserve(data_vec.size() + (range_partitions[x][i]).size());
     data_vec.insert(data_vec.end(), range_partitions[x][i].begin(), range_partitions[x][i].end());
     
     //for(int r : range_data) data_vec.push_back(r);
   }
   int n = data_vec.size();
   int * data = &data_vec[0];
 
   sort(data, n);

   double sort_done = omp_get_wtime();
   printf("Total sorting time: %f\n", sort_done - start);
   //printf("WORKER %i PROCESSED %i ITEMS\n", i, n);
   std::string out = outfile + std::to_string(i);
   write_output(data, n, i, out);
   double writing_done = omp_get_wtime();
   printf("Write time: %f \n", writing_done - sort_done);
 }
}

void validate(int num_workers, std::string filename) {
  std::string command;
  std::string cat = "";
  for(int i = 0; i < num_workers; i ++) {
    command = "valsort -o " + filename + std::to_string(i) + ".sum " + filename + std::to_string(i) + ".dat";
    system(command.c_str());
    std::cout << command << std::endl;
    cat += filename + std::to_string(i) + ".sum ";
  }
  command = "cat " + cat + "> all.sum";
  system(command.c_str());
  std::cout << command << std::endl;
  command = "valsort -s all.sum";
  system(command.c_str());
  std::cout << command << std::endl;
}
    

void test_partitions(int * sorted_data, int n, std::vector<int> partitions) { 

    int p_index = 0;
    int p_size = partitions.size() + 1;
    //std::cout << "SIZE " << p_size << " " << partitions.size() << std::endl;
    int * count = new int[p_size]; 
    for(int i = 0; i < p_size; i ++) count[i] = 0;
    for(int i = 0; i < n; i ++) {
      while(sorted_data[i] > partitions[p_index] && p_index < partitions.size()) {
        p_index++;
      }
      //printf("Less than %i\n", sorted_data[i]);
      count[p_index]++;
    }
    std::cout << "PARTITION COUNTS " << std::endl;
    for(int i = 0; i < p_index; i ++) printf("worker%i load: %i \n", i, count[i]);
}

int main () {

  /* PARAMETERS */
  int num_workers = omp_get_max_threads();
  int num_samples = num_workers * 10000;
  int num_records = num_workers * 1562500; //10 GB
  int num_records_per_worker = num_records/num_workers;
  int num_partitions = num_workers;
  std::string filename = "file";
  std::string outfile = "outfile";
 
  /* USE GENSORT TO GENERATE INPUT DATA*/
  generate_input(num_records, filename, num_workers);
    std::cout << std::endl;

  printf("INPUT GENERATION COMPLETE\n");
  /* START TIMER */
  //std::clock_t start;
  //double duration;
  //start = std::clock();
  //double t1 = 0;
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
  //for(int i = 0; i < range_partitions.size(); i ++) {
  //  std::cout << "WORKER" << i << std::endl;
  //  for(int j = 0; j < range_partitions[i].size(); j ++) {
  //    std::cout << "PARTITION " << j << std::endl;
  //    for(int k = 0; k < range_partitions[i][j].size(); k ++) {
  //         std::cout << range_partitions[i][j][k] << " ";
  //    }
  //    std::cout << std::endl;
  //  }
  //  std::cout << std::endl;
  //}
  std::cout<<  "DIVIDE DATA INTO RANGES "<< t3 <<'\n';  

  /* SORT DATA */
  sort_ranges(range_partitions, num_workers, filename, outfile);
  double t4 = omp_get_wtime() - t2 - t3 - t1 - start;

  std::cout<<  "SORT AND WRITE OUTPUT "<< t4 <<'\n';
  /* RUN VALSORT TO VALIDATE SORT */



  double t = omp_get_wtime() - start;
  printf("Records: %i Samples: %i Workers: %i \n", num_records, num_samples, num_workers);
  std::cout<<"TOTAL "<< t <<'\n';

  /* TESTS */
  //printf("\n\n ---------------------------------------------------\n TEST DATA \n ---------------------------------------------------\n");
  //int * data = new int[num_records];
  //read_file(data, filename + "0", num_records);
  //std::sort(data, data + num_records);
  //test_partitions(data, num_records, partitions);


  return 0;
}
