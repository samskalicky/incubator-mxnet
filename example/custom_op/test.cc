#include <iostream>
#include <map>
#include <string>
#include "custom_op.h"
#include <assert.h>

#define NUM_IN 2
#define NUM_OUT 1
#define BLK 32

void gemm(double* A, double* B, double* C, unsigned n, unsigned k, unsigned m) {
  unsigned i,j,kk;
  for (i=0;i<n;i++) {
    for (j=0;j<m;j++) {
      C[i*m+j] = 0;
      for (kk=0;kk<k;kk++) {
        C[i*m+j] += A[i*k+kk] * B[kk*m+j];
      }
    }
  }
  //for(unsigned i=0; i<n; i++) {
    //for(unsigned j=0; j<m; j++) {
      //C[i*m+j] = 0;
      //for(unsigned kk=0; kk<k; kk++) {
        //C[i*m+j] += A[i*k+kk] * B[kk*m+j];
      //}
    //}
  //}
  // std::cout << "in here" << std::endl;
  // unsigned ii, kkk, jj, i, kk, j;
  // #pragma omp parallel for
  // for(ii=0;ii<n;ii=ii+BLK){
  //     for(kkk=0;kkk<k;kk=kkk+BLK){
  //         for(jj=0;jj<m;jj=jj+BLK){
  //             int icond = ii+BLK;
  //             for(i=ii;i<icond;i++){
  //                 int kcond = kkk+BLK;
  //                 for(kk=kkk;kk<kcond;kk++){
  //                     double temp = A[i*k+kk];
  //                     int jcond = jj+BLK;
  //                     for(j=jj;j<jcond;j=j+2){
  //                             C[i*m+j] += temp*B[kk*m+j];
  //                             C[i*m+j+1] += temp*B[kk*m+j+1];
  //                             //C[i][j+2] += temp*B[k][j+2];
  //                             //C[i][j+3] += temp*B[k][j+3];
  //                     }
  //                 }
  //             }
  //         }
  //     }
  // }
}

//User code
int myFCompute(std::map<std::string,std::string> attrs,
               std::vector<MXTensor> inputs, std::vector<MXTensor> outputs) {

  auto input1 = inputs[0].getData<double>();
  auto input2 = inputs[1].getData<double>();
  auto output = outputs[0].getData<double>();
  unsigned n = inputs[0].shape[0];
  unsigned k = inputs[0].shape[1];
  unsigned m = inputs[1].shape[1];

  gemm(input1, input2, output, n, k, m);


  // for(int i=0; i<NUM_OUT; i++) {
  //   int64_t cnt=0;
  //   for(int j=0; j<inputs[i].shape.size(); j++) {
  //     for(int k=0; k<inputs[i].shape[j]; k++) {
  //       outputs[i].getData<float>()[cnt] = inputs[i].getData<float>()[cnt] + 42;
  //       cnt++;
  //     }
  //   }
  // }
  
  return 1; //no error
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
