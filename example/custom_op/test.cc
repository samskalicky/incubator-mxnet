#include <iostream>
#include <map>
#include <string>
#include "custom_op.h"

#define NUM_IN 1
#define NUM_OUT 1

//User code
int myFCompute(std::map<std::string,std::string> attrs,
               std::vector<MXTensor> inputs, std::vector<MXTensor> outputs) {
  std::cout << "called myFCompute with :" << std::endl;
  std::cout << "\tattrs:    " << attrs.size() << std::endl;
  std::cout << "\tinputs:   " << inputs.size() << std::endl;
  std::cout << "\toutputs:  " << outputs.size() << std::endl;

  for(int i=0; i<NUM_OUT; i++) {
    int64_t cnt=0;
    for(int j=0; j<inputs[i].shape.size(); j++) {
      for(int k=0; k<inputs[i].shape[j]; k++) {
        outputs[i].getData<float>()[cnt] = inputs[i].getData<float>()[cnt] + 42;
        cnt++;
      }
    }
  }
  
  return 1; //no error
}

int parseAttrs(std::map<std::string,std::string> attrs,
               int* num_in, int* num_out) {
  std::cout << "parse attrs" << std::endl;

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
  std::cout << "infer type" << std::endl;
  for(int i=0; i<NUM_IN; i++) {
    outtypes[i] = intypes[i];
  }
  
  return 1; //no error
}

int inferShape(std::map<std::string,std::string> attrs, std::vector<std::vector<unsigned int>> &inshapes,
               std::vector<std::vector<unsigned int>> &outshapes) {
  std::cout << "infer shape" << std::endl;

  for(int i=0; i<NUM_OUT; i++) {
    for(int j=0; j<inshapes[i].size(); j++) {
      outshapes[i].push_back(inshapes[i][j]);
    }
  }

  return 1; //no error
}

//Register Op
REGISTER_OP(sam)
.setFCompute(myFCompute)
.setParseAttrs(parseAttrs)
.setInferType(inferType)
.setInferShape(inferShape);
