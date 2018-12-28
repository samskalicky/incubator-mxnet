#include <string>
#include <map>

#include "../../include/mxnet/base.h"

std::map<int,mxnet::AccContext> mxnet::Context::acc_map;
std::map<std::string,int> mxnet::Context::acc_names;
