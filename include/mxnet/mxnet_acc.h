
#include <string>

typedef int (FCompute)(int, void*);

extern "C" std::string getAccName();
extern "C" FCompute* getFCompute(std::string);
