#include <iostream>
#include <map>
#include <string>
#include "custom_op.h"
#include <dlpack/dlpack.h>
#include <assert.h>

#define NUM_IN 2
#define NUM_OUT 1
#define BLK 32

//User code
int myFCompute(std::map<std::string,std::string> attrs,
               DLTensor* inputs, int num_inp, DLTensor* outputs, int num_out) {
  std::cout << "called myFCompute with :" << std::endl;
  std::cout << "\tattrs:    " << attrs.size() << std::endl;
  std::cout << "\tinputs:   " << num_inp << std::endl;
  std::cout << "\toutputs:  " << num_out << std::endl;

  for(int i=0; i<NUM_OUT; i++) {
    int64_t cnt=0;
    for(int j=0; j<inputs[i].ndim; j++) {
      for(int k=0; k<inputs[i].shape[j]; k++) {
        ((float*)outputs[i].data)[cnt] = ((float*)outputs[i].data)[cnt] + 42;
        cnt++;
      }
    }
  }
}

int parseAttrs(std::map<std::string,std::string> attrs,
               int* num_in, int* num_out) {

  if(attrs.find("myParam") == attrs.end()) {
    std::cout << "Missing param 'myParam'" << std::endl;
    return 0;
  }

  *num_in = NUM_IN;
  *num_out = NUM_OUT;
  
  return 1; //no error
}

int inferType(std::map<std::string,std::string> attrs, std::vector<int> &intypes,
              std::vector<int> &outtypes) {
  for(int i=0; i<NUM_IN; i++) {
    outtypes[i] = intypes[i];
  }
  
  return 1; //no error
}

int inferShape(std::map<std::string,std::string> attrs, std::vector<std::vector<unsigned int>> &inshapes,
               std::vector<std::vector<unsigned int>> &outshapes) {

  unsigned n = inshapes[0][0];
  unsigned k = inshapes[0][1];
  unsigned kk = inshapes[1][0];
  unsigned m = inshapes[1][1];
  assert(k == kk);
  outshapes[0].push_back(n);

  outshapes[0].push_back(m);

  // for(int i=0; i<NUM_OUT; i++) {
  //   for(int j=0; j<inshapes[i].size(); j++) {
  //     outshapes[i].push_back(inshapes[i][j]);
  //   }
  // }

  return 1; //no error
}

//Register Op
REGISTER_OP(sam)
.setFCompute_cpu(myFCompute)
.setParseAttrs(parseAttrs)
.setInferType(inferType)
.setInferShape(inferShape);
