#include "mxnet_acc.h"
#include <string>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <map>

std::map<void*,void*> arrays;

extern "C" void getAccName(char* s) {
  std::string name = "myacc";
  strcpy(s,name.c_str());
}

extern "C" FCompute* getFCompute(const char* name) {
  return 0;
}

extern "C" void releaseAll() {
  std::map<void*,void*>::iterator it;
  for (it=arrays.begin(); it!=arrays.end(); ++it) {
    free(it->first);
  }
  arrays.clear();
}
extern "C" void free(void* p) {
  free(p);
  arrays.erase(p);
}
extern "C" void directFree(void* p) {
  free(p);
  arrays.erase(p);
}
extern "C" void* alloc(std::size_t size) {
  void* p = malloc(size);
  arrays[p]=p;
  return p;
}
