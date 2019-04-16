#ifndef MXNET_LIBRARY_H_
#define MXNET_LIBRARY_H_

#include <dlfcn.h>
#include <iostream>

void* load_lib(const char* path);
void get_func(void* handle, void** func, char* name);

#endif
