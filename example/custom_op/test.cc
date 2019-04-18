#include <iostream>
#include "custom_op.h"

//User code
void myFCompute() {
  std::cout << "called myFCompute!!!" << std::endl;
}
REGISTER_OP(sam)
.setFCompute(myFCompute);
