#ifndef MXNET_LIBRARY_H_
#define MXNET_LIBRARY_H_

#include <dlfcn.h>
#include <iostream>
#include "mxnet/custom_op.h"

void* load_lib(const char* path);
void get_func(void* handle, void_ptr func, char* name);

#endif
