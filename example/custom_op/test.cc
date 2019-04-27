#include <iostream>
#include <map>
#include <string>
#include "custom_op.h"

#define NUM_IN 1
#define NUM_OUT 1

//User code
void myFCompute(const char* const* keys, const char* const* vals, int num,
                const long int** inshapes, int* indims, void** indata, int* intypes,
                const long int** outshapes, int* outdims, void** outdata, int* outtypes) {
  std::cout << "called myFCompute with " << num << " attrs" << std::endl;
}

int parseAttrs(const char* const* keys, const char* const* vals, int num,
               int* num_in, int* num_out) {
  std::cout << "parse attrs" << std::endl;
  std::map<std::string,std::string> params;
  for(int i=0; i<num; i++) {
    params[std::string(keys[i])]=std::string(vals[i]);
  }

  if(params.find("myParam") == params.end()) {
    std::cout << "Missing param 'myParam'" << std::endl;
    return 0;
  }

  *num_in = NUM_IN;
  *num_out = NUM_OUT;
  
  return 1;
}

int inferType(const char* const* keys, const char* const* vals, int num,
               int* intypes, int* outtypes) {
  std::cout << "infer type" << std::endl;
  for(int i=0; i<NUM_IN; i++) {
    outtypes[i] = intypes[i];
  }
  
  return 1;
}

int inferShape(mxAlloc_t mx_alloc, void* ptr, const char* const* keys, const char* const* vals, int num,
               unsigned int** inshapes, int* indims, unsigned int*** outshapes, int** outdims) {
  std::cout << "infer shape" << std::endl;
  *outdims = (int*)mx_alloc(ptr,NUM_OUT*sizeof(int));
  *outshapes = (unsigned**)mx_alloc(ptr,NUM_OUT*sizeof(unsigned*));

  for(int i=0; i<NUM_OUT; i++) {
    (*outdims)[i] = indims[i];
    (*outshapes)[i] = (unsigned*)mx_alloc(ptr,indims[i]*sizeof(unsigned));
    for(int j=0; j<indims[i]; j++) {
      (*outshapes)[i][j] = inshapes[i][j];
    }
  }
  
  return 1;
}

//Register Op
REGISTER_OP(sam)
.setFCompute(myFCompute)
.setParseAttrs(parseAttrs)
.setInferType(inferType)
.setInferShape(inferShape);
