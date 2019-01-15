
#ifndef MXNET_OPERATOR_ACC_OP_INL_H_
#define MXNET_OPERATOR_ACC_OP_INL_H_

#include <dmlc/parameter.h>
#include <vector>
#include <algorithm>
#include <utility>
#include "./mxnet_op.h"
#include "./operator_common.h"
#include "./elemwise_op_common.h"

namespace mxnet {
namespace op {

void AccOpForward(const nnvm::NodeAttrs& attrs,
                   const OpContext& ctx,
                   const std::vector<TBlob>& inputs,
                   const std::vector<OpReqType>& req,
                   const std::vector<TBlob>& outputs) {
  using namespace mxnet_op;
  using namespace mshadow;
  std::cout << "AccOp: " << inputs.size() << " inputs, " << outputs.size() << " outputs" << std::endl;
  AccExec fcomp = ctx.acc_func;
  if(!fcomp)
    LOG(ERROR) << "acc_func not set!";
  else
    fcomp(0,0);
}

}  // namespace op
}  // namespace mxnet

#endif  // MXNET_OPERATOR_ACC_OP_INL_H_
