
#include <string>

typedef int (FCompute)(int, void*);

std::string getAccName() {
  return std::string("myacc");
}

extern "C" FCompute* getFCompute(std::string) {
  return 0;
}
