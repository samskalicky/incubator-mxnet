#include "mxnet_acc.h"
#include <string>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <map>

std::map<void*,void*> arrays;

/*
 * Return name of accelerator
 */
extern "C" void getAccName(char* s) {
  std::string name = "myacc";
  strcpy(s,name.c_str());
}

extern "C" void analyzeGraph(const char* json) {
  std::string graph(json);
  std::cout << graph << std::endl;
}

/*
 * Memory Management functions
 */
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

/*
 * Data Movement functions
 */
extern "C" int copyTo(void* dst, void* src, size_t size) {
  return 0;
}
extern "C" int copyFrom(void* dst, void* src, size_t size) {
  return 0;
}
extern "C" int copyBetween(void* dst, void* src, size_t size) {
  return 0;
}

/*
 * Operator FCompute functions
 */
int exp(int a, void* b);
int broadcast_add(int a, void* b);

extern "C" AccExec getFCompute(const char* name) {
  std::string fname(name);
  if(fname.compare("exp")==0)
    return &exp;
  else if(fname.compare("broadcast_add")==0)
    return &broadcast_add;
  else
    return 0;
}

int exp(int a, void* b) {
  std::cout << "exp" << std::endl;
  return 0;
}

int broadcast_add(int a, void* b) {
  std::cout << "add" << std::endl;
  return 0;
}
