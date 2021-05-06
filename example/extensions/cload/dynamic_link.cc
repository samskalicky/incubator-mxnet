#include <iostream>
#include "mxnet/c_api.h"

int MXLoadLib(const char *path, unsigned verbose);

int main(int argc, char* argv[]) {
  if(argc < 2) {
    std::cout << "usage: main <path to lib.so>" << std::endl;
    return 1;
  }
  
  std::cout << "loading library from " << argv[1] << std::endl;
  int ret = MXLoadLib(argv[1],1);
  std::cout << "done" << std::endl;
  return 0;
}
