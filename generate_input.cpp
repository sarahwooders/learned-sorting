

void worker_generate_input(std::string filename, int n, int id){
    //MAKE SKEWED LATER WITH -s
    std::string command = "./gensort -b" + std::to_string(n*id) + " " + std::to_string(n) + " " + filename + std::to_string(id);
    //std::cout << command << std::endl;
    system(command.c_str());
}

void generate_input(int n, std::string filename, int num_workers){
  int num = n/num_workers; 
  int left = n - num * num_workers;
  #pragma omp parallel 
  {
    //#pragma omp for shared(num_workers, filename, num, left), private(i)
    //#pragma omp for

    //for(i = 0; i < num_workers; i ++) {
    int i = omp_get_thread_num();
    if(left > 0) {
      worker_generate_input(filename, num + 1, i);
      left --;
    }
    else {
      worker_generate_input(filename, num, i);
    }
  }
}


