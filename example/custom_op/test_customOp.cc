
#include <dlfcn.h>
#include <iostream>
#include "custom_op.h"

/*
 * load shared object
 * returns handle to loaded object
 */
void* load_lib(const char* path) {
  void *handle;
  handle = dlopen(path, RTLD_LAZY);

  if (!handle) {
    std::cerr << "Error loading library: '" << path << "'\n" << dlerror() << std::endl;
    return 0;
  }
  return handle;
}

/*
 * get function out of object handle 
 */
void get_func(void* handle, void_ptr func, const char* name) {
  *func = (void_t) dlsym(handle, name);
  if(!func) {
    std::cerr << "Error getting function '" << name << "' from library\n" << dlerror() << std::endl;
  }
}

int main() {
  //load custom op library
  void *lib = load_lib("libtest.so");
  if(!lib) return 1;

  //get "get_size" function out of library
  get_size_t get_size;
  get_func(lib, (void_ptr)(&get_size), "_opRegSize");
  if(!get_size) return -1;

  //get "get_op" function out of library
  get_op_t get_op;
  get_func(lib, (void_ptr)(&get_op), (char*)"_opRegGet");
  if(!get_op) return -1;

  //loop over custom ops in the library
  int num_ops = get_size();
  for(int i=0; i<num_ops; i++) {
    char* name;
    fcomp_t fcomp = nullptr;
    //get op
    get_op(i,&name,&fcomp);
    std::cout << "got custom op: " << name << std::endl;
    //call fcompute function
    (*fcomp)();
  }

  return 0;
}
/*#################################################################################################*/
