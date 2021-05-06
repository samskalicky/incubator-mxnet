#include <iostream>
#include <dlfcn.h>
#include "mxnet/c_api.h"

typedef int (*MXLoadLib_t)(const char *path, unsigned verbose);

int main(int argc, char* argv[]) {
  if(argc < 3) {
    std::cout << "usage: main <path to lib.so> <path to libmxnet.so>" << std::endl;
    return 1;
  }

  // get libmxnet.so
  void *handle;
  handle = dlopen(argv[2], RTLD_LAZY);
  if (!handle) {
    std::cerr << "Unable to load libmxnet.so" << std::endl;
    return 1;
  }
  MXLoadLib_t MXLoadLib;
  MXLoadLib = (MXLoadLib_t) dlsym(handle, "MXLoadLib");
  
  std::cout << "loading library from " << argv[1] << std::endl;
  int ret = MXLoadLib(argv[1],1);
  std::cout << "done" << std::endl;
  return 0;
}
