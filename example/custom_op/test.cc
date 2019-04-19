#include <iostream>
#include <map>
#include <string>
#include "custom_op.h"

//User code
void myFCompute(char const** keys, char const ** vals, int num) {
  std::cout << "called myFCompute with " << num << " attrs" << std::endl;
}

int parseAttrs(char const** keys, char const** vals, int num) {
  std::cout << "parsing " << num << " attrs" << std::endl;
  std::map<std::string,std::string> params;
  for(int i=0; i<num; i++) {
    params[std::string(keys[i])]=std::string(vals[i]);
  }

  if(params.find("myParam") == params.end()) {
    std::cout << "Missing param 'myParam'" << std::endl;
    return 0;
  }
  return 1;
}

//Register Op
REGISTER_OP(sam)
.setFCompute(myFCompute)
.setParseAttrs(parseAttrs);
