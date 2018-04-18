// read a file into memory
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <cmath>
#include <sstream>
#include <vector>


void read_gensort(std::vector<int> & arr, int n, std::string filename) {
  std::ifstream is (filename, std::ifstream::binary);
  if (is) {
    //output.open ("gensort.csv");
    is.seekg (0, is.end);
    int length = is.tellg();
    is.seekg (0, is.beg);
    
    char * buffer = new char [100];
    //int * buffer = new int [100];
    // allocate memory:
    int count = 0;
    int i = 0;
    while(i <= length - 100) {
        is.read (buffer, 100);
        //std::cout.write (buffer, 100);
        int myInt = std::abs(*(int*)&buffer[0]);
        if(n <= i/100) printf("Sizes %i %i \n", i/100, n); 
        arr[i/100] = myInt;
        i += 100;
        count++;
    }
    //std::cout << std::endl;

    // read data as a block:
    //is.read (buffer,length);
    is.close();

    // print content:
    //std::cout.write (buffer,length);

    //delete[] buffer;
  }
}

void read_data(std::vector<std::vector<int> > & data, int num_workers, int n, std::string file) {
  #pragma omp parallel shared(data)
  {
    
    int i = omp_get_thread_num();
    std::string filename = file + std::to_string(i);
    data[i].resize(n);
    read_gensort(data[i], n, filename);
  }
}


std::vector<int> read_txt(std::string filename) {
  std::vector<int> out;
  std::ifstream file(filename,std::ios::in);
  if (file.good()) {
    std::string str;
    while(getline(file, str)) {
      std::istringstream ss(str);
      int num;
      ss >> num;
      out.push_back(num);
    }
  }
  return out;
}

//void write(int * data, std::string filename) {
//  std::ofstream fout(filename, std::ios_base::trunc | std::ios_base::out | std::ios_base::binary);
//
//  fout.close();
//
//}

//int main(int argc, char **argv) {
//  int N;
//   if (argc != 2 || sscanf(argv[1], "%d", &N) == -1) {
//       std::cerr << "n missing\n";
//       return 111;
//   }
//   int arr[N];
//   read(arr, N);
//}
