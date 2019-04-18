#ifndef MXNET_LIBRARY_H_
#define MXNET_LIBRARY_H_

#include <dlfcn.h>
#include <iostream>
#include "mxnet/custom_op.h"
#include "mxnet/library.h"

void* load_lib(const char* path) {
  void *handle;
  handle = dlopen(path, RTLD_LAZY);

  if (!handle) {
    std::cerr << "Error loading library: '" << path << "'\n" << dlerror() << std::endl;
    return 0;
  }
  return handle;
}

void get_func(void* handle, void_ptr func, char* name) {
  *func = (void_t)dlsym(handle, name);
  if(!func) {
    std::cerr << "Error getting function '" << name << "' from accelerator library\n" << dlerror() << std::endl;
  }
}

#endif
