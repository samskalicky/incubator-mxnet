
typedef int (FCompute)(int, void*);

extern "C" void getAccName(char*);
extern "C" FCompute* getFCompute(const char*);
