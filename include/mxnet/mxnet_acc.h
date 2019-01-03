#include <cstdlib>

typedef int (FCompute)(int, void*);

extern "C" void getAccName(char*);
extern "C" FCompute* getFCompute(const char*);
extern "C" void releaseAll();
extern "C" void free(void*);
extern "C" void directFree(void*);
extern "C" void* alloc(std::size_t size);
