#include <iostream>
#include "custom_op.h"

//User code
void myFCompute() {
  std::cout << "called myFCompute!!!" << std::endl;
}

int parseAttrs(char const** keys, char const** vals, int num) {
  std::cout << "parsing " << num << " attrs" << std::endl;
}

//Register Op
REGISTER_OP(sam)
.setFCompute(myFCompute)
.setParseAttrs(parseAttrs);
