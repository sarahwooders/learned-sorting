// read a file into memory
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <cmath>


void read_gensort(int arr[], int n, std::string filename) {
  std::ifstream is (filename, std::ifstream::binary);
  if (is) {
    
    std::ofstream output;
    //output.open ("gensort.csv");
    is.seekg (0, is.end);
    int length = is.tellg();
    is.seekg (0, is.beg);
    
    char * buffer = new char [100];
    //int * buffer = new int [100];
    // allocate memory:
    int i = 0;
    std::cout << "len " << length << std::endl;
    while(i < length - 100) {
        is.read (buffer, 100);
        //std::cout.write (buffer, 100);
        int myInt = std::abs(*(int*)&buffer[0]);
        //std::cout << myInt << " ";
        //output << myInt << "\n";
        arr[i/100] = myInt;
        i += 100;
    }
    //std::cout << std::endl;

    // read data as a block:
    //is.read (buffer,length);
    output.close();

    is.close();

    // print content:
    //std::cout.write (buffer,length);

    delete[] buffer;
  }
}

void write(int * data, std::string filename) {
  std::ofstream fout(filename, std::ios_base::trunc | std::ios_base::out | std::ios_base::binary);

  fout.close();

}

//int main(int argc, char **argv) {
//  int N;
//   if (argc != 2 || sscanf(argv[1], "%d", &N) == -1) {
//       std::cerr << "n missing\n";
//       return 111;
//   }
//   int arr[N];
//   read(arr, N);
//}
