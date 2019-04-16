
#include "custom_op.h"

//User code
void myFCompute() {}
REGISTER_OP(sam)
.setFCompute(myFCompute);
