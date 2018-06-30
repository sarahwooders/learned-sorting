

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

void test_output(std::vector<int> & data) {
    for(int i = 1; i < data.size(); i ++) {
        if(data[i] < data[i-1]) {
            printf("Value %i at position %i out of order", data[i], i);
        }
    }
}

void test_output(int *data, int n) {
    for(int i = 1; i < n; i ++) {
        if(data[i] < data[i-1]) {
            printf("Value %i at position %i out of order", data[i], i);
        }
    }
}
