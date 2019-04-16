
#include <dlfcn.h>

#include "custom_op.h"

int main() {
  OpRegistry::get()->list();
  std::cout << "---------------------" << std::endl;

  void *h = dlopen("libtest.so", RTLD_LAZY);
  OpRegistry* (*global_get)();
  *(void**)(&global_get) = dlsym(h, "GlobalGet");
  OpRegistry* reg = global_get();
  reg->list();
  return 0;
}
/*#################################################################################################*/
