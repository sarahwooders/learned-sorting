#include "radix/radix.cc"
#include "models.h"

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


//USE TO IMPROVE PARTITIONING
int binary_search(std::vector<int> & arr, int x, int start, int guess)
{

    int l = start;
    int r = arr.size() - 1;

    if(guess > arr[r]) r = guess; //overshot estimate
    else l = guess;  //undershot estimate

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
        if (arr[m] > x)
            r = m - 1;
        // If x is smaller, ignore right half
        else
            l = m + 1;
    }

    if(r > 0 && arr[r-1] < x) return -1;

    for(int i = 0; i < arr.size(); i ++) {
      if(arr[i + 1] <= arr[r]) r ++;
    }
 
    // if we reach here, then element was
    // not present
    return r;
}

/* Generate a given number of random samples form the data */
int * generate_samples(std::vector<std::vector<int> > data, int num_samples, int num_records, int num_workers) {

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
   return all_samples;
}


/*
Return vector of range boundary values
*/
std::vector<int> pick_range_boundaries(int * samples, int samples_n, int num_partitions) {
   // Determine partition values
   std::vector<int> partitions; 
   int partition_size = samples_n/num_partitions;
   for(int x = partition_size; x < samples_n; x += partition_size) {
      partitions.push_back(samples[x]);
   }
   //for(int i = 0; i < partitions.size(); i ++) printf("part %i\n",partitions[i]);
   return partitions;
}


UniformModel generate_model(int * samples, int samples_n, int n) {
  UniformModel m;
  int min = samples[0];
  int max = samples[0];
  for(int i = 0; i < samples_n; i ++) {
    if(samples[i] < min) min = samples[i];
    if(samples[i] > max) max = samples[i];
  }
  m.min = min;
  m.max = max;
  m.n = n;
  printf("MODEL GENERATED: min %i max %i n %i \n", min, max, n);
  return m;
}


/*
  Partition data into ranges for each worker. 
  USES MODEL
 */ 
std::vector<std::vector<int> >  worker_output_ranges(std::vector<int> data, std::vector<int> partitions, std::string filename, UniformModel model) {
   int i = omp_get_thread_num();
   double ptime = omp_get_wtime();

  int num_files = partitions.size() + 1;
  std::vector<std::vector<int> > v;
  v.resize(num_files);

  int * wdata = &data[0];
  //sort(wdata, n);
  double sort_time = omp_get_wtime();
  radix_sort(wdata, 0, data.size(), 24, 9);
  sort_time = omp_get_wtime() - sort_time;

  int index = 0;
  int last = 0;
  double shared_time = omp_get_wtime();
  
  
  // Does v need initializaton... ?  
  for(int j = 0; j < data.size(); j ++) {
    int est_pos = model.get_position(data[j]);  
    int part_pos = (partitions.size() * est_pos)/model.n;
    v[part_pos].push_back(data[j]); 
  }
  shared_time = omp_get_wtime() - shared_time;
  //printf("assignment time %f\n", t3); 
  printf("Thread %i data size %i Parallel time %f shared time %f sort time %f \n", i, data.size(), omp_get_wtime() - ptime, shared_time, sort_time);
 
  //Close all open files
  //for(auto& outfile : outfiles) outfile.close();
  return v;
} 


std::vector<std::vector<int> >  worker_output_ranges(std::vector<int> data, std::vector<int> partitions, std::string filename) {
   int i = omp_get_thread_num();
   double ptime = omp_get_wtime();

  int num_files = partitions.size() + 1;
  std::vector<std::vector<int> > v;
  v.resize(num_files);

  int * wdata = &data[0];
  //sort(wdata, n);
  double sort_time = omp_get_wtime();
  radix_sort(wdata, 0, data.size(), 24, 9);
  sort_time = omp_get_wtime() - sort_time;

  int index = 0;
  int last = 0;
  double shared_time = omp_get_wtime();
  //for(int index = 0; index < partitions.size(); index ++) {
  //  //estimate location of the partition point
  //  int guess = data.size() * (index/partitions.size());
  //  int upper_bound = binary_search(data, partitions[index], last, guess);
  //  std::vector<int> part(upper_bound-last);
  //  part.assign(data.begin() + last, data.begin() + upper_bound);
  //  last = upper_bound;
  //  v[index] = part;
  //}
  for(int j = 0; j < data.size(); j ++) {

    //int index = binarySearch(partitions, data[i][j]);
    //printf("Search: index %i data %i\n", index, data[i][j]);
    //while(index < partitions.size() && partitions[index] < data[i][j]) { //CAN BE MADE FASTER (binary search) 
    //  index ++;
    //}

    if((index < partitions.size() && wdata[j] >= partitions[index]) || j == data.size() - 1) {
       std::vector<int> part;
       part.assign(data.begin() + last, data.begin() + j);
       v[index] = part;
       last = j; 
       while (index < partitions.size() && wdata[j] >= partitions[index]) index ++;
     }
  }
  shared_time = omp_get_wtime() - shared_time;
    //printf("assignment time %f\n", t3); 
    printf("Thread %i data size %i Parallel time %f shared time %f sort time %f \n", i, data.size(), omp_get_wtime() - ptime, shared_time, sort_time);
 
   //Close all open files
   //for(auto& outfile : outfiles) outfile.close();
   return v;
} 

std::vector<std::vector<std::vector<int> > > output_ranges(std::vector<std::vector<int> > & data, std::vector<int> & partitions, std::string filename, int records_per_worker) {

 double sqtime = omp_get_wtime();
 int num_workers = data.size();
 
 std::vector<std::vector<std::vector<int> > > range_partitions;
 range_partitions.resize(num_workers);

  printf("partitions \n");
 for(int x = 0; x < partitions.size(); x++) {
   printf("%i ", partitions[x]);
  }
  printf("\n");


 sqtime = omp_get_wtime() - sqtime;
 #pragma omp parallel shared(range_partitions)
 {
    int i = omp_get_thread_num();
    range_partitions[i] = worker_output_ranges(data[i], partitions, filename);
 }
  printf("Ranges seq time %f \n ", sqtime);
  return range_partitions;
}

std::vector<std::vector<std::vector<int> > > output_ranges(std::vector<std::vector<int> > & data, std::vector<int> & partitions, std::string filename, int records_per_worker, UniformModel model) {

 double sqtime = omp_get_wtime();
 int num_workers = data.size();
 
 std::vector<std::vector<std::vector<int> > > range_partitions;
 range_partitions.resize(num_workers);

  printf("partitions \n");
 for(int x = 0; x < partitions.size(); x++) {
   printf("%i ", partitions[x]);
  }
  printf("\n");


 sqtime = omp_get_wtime() - sqtime;
 #pragma omp parallel shared(range_partitions)
 {
    int i = omp_get_thread_num();
    range_partitions[i] = worker_output_ranges(data[i], partitions, filename, model);
 }
  printf("Ranges seq time %f \n ", sqtime);
  return range_partitions;
}

