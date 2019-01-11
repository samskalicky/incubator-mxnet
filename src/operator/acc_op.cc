#include "./acc_op-inl.h"

namespace mxnet {
namespace op {

  NNVM_REGISTER_OP(acc_op)
  .describe("code(description)" ADD_FILELINE)
  .set_num_inputs(1)
  .set_num_outputs(1)
  .set_attr<nnvm::FListInputNames>("FListInputNames",
      [](const NodeAttrs& attrs) {
          return std::vector<std::string>{"data"};
      })
  .set_attr<FCompute>("FCompute<cpu>", AccOpForward);
  
}  // namespace op
}  // namespace mxnet
