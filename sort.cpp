#include "util.cpp"
 

//void sort_ranges(std::vector<std::vector<std::vector<int> > > & range_partitions, int num_workers, std::string filename, std::string outfile) {
//
// for(int a  = 0; a < range_partitions.size(); a ++) {
//   printf("\n Worker %i \n", a);
//   for(int b = 0; b < range_partitions[a].size(); b ++) {
//     printf(" %i", range_partitions[a][b].size());
//   }
// }
// #pragma omp parallel
// {
//   
//   double start = omp_get_wtime();
//   int i = omp_get_thread_num();
//
//   std::string out = outfile + std::to_string(i);
//   //std::ofstream f;
//   //f.open(out);
//
//
//   std::priority_queue<std::pair<int, int> > q;
//   int indices[num_workers];
//   std::vector<int> data_vec;
//   for(int x = 0; x < num_workers; x ++) {
//      indices[x] = 0;
//      if(indices[x] < range_partitions[x][i].size()) {
//        q.push(std::make_pair(range_partitions[x][i][0], x));
//      }
//   }
//   int max_size = 0;
//   int data_count = 0;
//   while(!q.empty()) {
//      std::pair<int,int> p = q.top();
//      data_count ++;
//      if(q.size() > max_size) max_size = q.size();
//      q.pop();
//      int val = p.first;
//      //f << val << "\n";
//      int worker = p.second;
//      indices[worker]++;
//      if(indices[worker] < range_partitions[worker][i].size()){
//        q.push(std::make_pair(range_partitions[worker][i][indices[worker]], worker));
//      }
//   }
//   //f.close();
//   double end = omp_get_wtime();
//   printf("Write time: %f \n", end - start);
//   printf("Data : %i %i \n", max_size, data_count);
// }
//}

void learned_sort_ranges(std::vector<std::vector<std::vector<int> > > & range_partitions, int num_workers, std::string filename, std::string outfile) {
 #pragma omp parallel default(none) shared(range_partitions, outfile, num_workers)
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
   double copytime = omp_get_wtime() - start; 
   std::sort(data, data + n);
   //radix_sort(data, 0, n, 24, 0);
   double sort_done = omp_get_wtime();
   printf("Thread %i data size %i copy time %f sorting time: %f\n", i, data_vec.size(), copytime, sort_done - copytime - start);
   //printf("WORKER %i PROCESSED %i ITEMS\n", i, n);
   std::string out = outfile + std::to_string(i);
   write_output(data, n, i, out);
   double writing_done = omp_get_wtime();
   //printf("Write time: %f \n", writing_done - sort_done);
 }
}

void sort_ranges(std::vector<std::vector<std::vector<int> > > & range_partitions, int num_workers, std::string filename, std::string outfile) {
 #pragma omp parallel default(none) shared(range_partitions, outfile, num_workers)
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
   double copytime = omp_get_wtime() - start; 
   std::sort(data, data + n);
   //radix_sort(data, 0, n, 24, 0);
   double sort_done = omp_get_wtime();
   printf("Thread %i data size %i copy time %f sorting time: %f\n", i, data_vec.size(), copytime, sort_done - copytime - start);
   //printf("WORKER %i PROCESSED %i ITEMS\n", i, n);
   std::string out = outfile + std::to_string(i);
   write_output(data, n, i, out);
   double writing_done = omp_get_wtime();
   //printf("Write time: %f \n", writing_done - sort_done);
 }
}


