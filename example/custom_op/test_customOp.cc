
#include <dlfcn.h>
#include <iostream>
#include "custom_op.h"

/*
 * load shared object
 * returns handle to loaded object
 */
void* load_lib(const char* path) {
  void *handle;
  dlerror();
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
  dlerror();
  *func = (void_t) dlsym(handle, name);
  if(!func) {
    std::cerr << "Error getting function '" << name << "' from library, error: '" << dlerror() << "'" << std::endl;
  }
}

int main() {
  //load custom op library
  void *lib = load_lib("libtest.so");
  if(!lib) {
    std::cerr << "Error loading library" << std::endl;
        return 1;
  }

  //get "get_size" function out of library
  get_size_t get_size = nullptr;
  get_func(lib, (void_ptr)(&get_size), "_opRegSize");
  if(get_size == 0) {
    std::cerr << "Error getting get_size function from library" << std::endl;
    return -1;
  }
  
  //get "get_op" function out of library
  get_op_t get_op = nullptr;
  get_func(lib, (void_ptr)(&get_op), (char*)"_opRegGet");
  if(get_op == 0) {
        std::cerr << "Error getting get_op function from library" << std::endl;
        return -1;
  }

  //loop over custom ops in the library
  int num_ops = get_size();
  for(int i=0; i<num_ops; i++) {
    char* name;
    fcomp_t fcomp = nullptr;
    parseAttrs_t parse = nullptr;
    inferType_t infer = nullptr;
    inferShape_t shape = nullptr;
    //get op
    get_op(i,&name,&fcomp,&parse,&infer,&shape);
    std::cout << "got custom op: " << name << std::endl;
    //call fcompute function
    (*fcomp)(nullptr,nullptr,0,
             nullptr,nullptr,nullptr,nullptr,
             nullptr,nullptr,nullptr,nullptr);
  }

  return 0;
}
/*#################################################################################################*/
