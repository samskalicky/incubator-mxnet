/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "custom_op-inl.h"

namespace mxnet {
namespace op {

NNVM_REGISTER_OP(CustomOp)
.describe(R"code(

CustomOp

)code" ADD_FILELINE)
.set_attr_parser(AttrParser)
.set_attr<mxnet::FInferShape>("FInferShape", InferShape)
.set_attr<nnvm::FInferType>("FInferType", InferType)
.set_attr<FCompute>("FCompute<cpu>", Forward)
.set_num_inputs([](const NodeAttrs& attrs){
    const CustomOpParam& params = nnvm::get<CustomOpParam>(attrs.parsed);
    return params.num_in;
  })
.set_num_outputs([](const NodeAttrs& attrs){
    const CustomOpParam& params = nnvm::get<CustomOpParam>(attrs.parsed);
    return params.num_out;
  })
.add_argument("data", "NDArray-or-Symbol[]", "Input data for the custom operator.")
.add_argument("op_type", "string", "Name of the custom operator. ");

}  // namespace op
}  // namespace mxnet
