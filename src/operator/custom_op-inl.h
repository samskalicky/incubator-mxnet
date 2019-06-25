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

#ifndef MXNET_OPERATOR_CUSTOMOP_INL_H_
#define MXNET_OPERATOR_CUSTOMOP_INL_H_

#include <dmlc/parameter.h>
#include <vector>
#include <algorithm>
#include <utility>
#include "./mxnet_op.h"
#include "./operator_common.h"
#include "mxnet/custom_op.h"
#include <dlpack/dlpack.h>

namespace mxnet {
namespace op {
  
struct CustomOpParam {
  std::string op_type;
  std::vector<const char*> keys;
  std::vector<const char*> vals;
  int num_in, num_out;
  CustomOp* op;
};

void AttrParser(NodeAttrs* attrs) {
  attrs->parsed = CustomOpParam();
  CustomOpParam& params = nnvm::get<CustomOpParam>(attrs->parsed);
  //initialize num inputs/outputs
  params.num_in = 0;
  params.num_out = 0;

  //parse attrs
  for (auto& p : attrs->dict) {
    if (p.first == "op_type") {
      params.op_type = p.second;
    }
    else {
      params.keys.push_back(p.first.c_str());
      params.vals.push_back(p.second.c_str());
    }
  }

  //check that op_type was specified, required
  CHECK(!params.op_type.empty()) << "Required argument `op_type` is missing.";
  //check that operator has been registered
  CHECK(OpRegistry::get()->hasOp(params.op_type)) << "Cannot find custom operator '" << params.op_type << "'";
  //get the op from registry
  params.op = OpRegistry::get()->op(params.op_type);
  //call parse attributes function
  CHECK(OpRegistry::get()->_callParseAttrs(params.op_type.c_str(),
                                           params.keys.data(),
                                           params.vals.data(),
                                           params.keys.size(),
                                           &params.num_in,
                                           &params.num_out)
        ) << "Error parsing params for custom operator '" << params.op_type << "'";
}

bool InferType(const NodeAttrs& attrs,
               std::vector<int> *in_type,
               std::vector<int> *out_type) {
  const CustomOpParam& params = nnvm::get<CustomOpParam>(attrs.parsed);

  //prepare in/out types, by copying values
  std::vector<int> intypes;
  std::vector<int> outtypes;
  for (int i = 0; i < params.num_in; ++i) {
    intypes.push_back((*in_type)[i]);
  }
  for (int i = 0; i < params.num_out; ++i) {
    outtypes.push_back((*out_type)[i]);
  }

  //call infer types function
  CHECK(OpRegistry::get()->_callInferType(params.op_type.c_str(),
                                          params.keys.data(),
                                          params.vals.data(),
                                          params.keys.size(),
                                          intypes.data(),
                                          intypes.size(),
                                          outtypes.data(),
                                          outtypes.size())
        ) << "Error inferring types for custom operator '" << params.op_type << "'";

  //check and assign types from custom op
  for (int i = 0; i < params.num_in; ++i) {
    TYPE_ASSIGN_CHECK(*in_type, i, intypes[i]);
  }
  for (int i = 0; i < params.num_out; ++i) {
    TYPE_ASSIGN_CHECK(*out_type, i, outtypes[i]);
  }

  return true;
}

 bool InferShape(const NodeAttrs& attrs,
                mxnet::ShapeVector *in_shape,
                mxnet::ShapeVector *out_shape) {
   CustomOpParam& params = (CustomOpParam&)nnvm::get<CustomOpParam>(attrs.parsed);

  std::vector<uint32_t*> inshapes(params.num_in);
  std::vector<int> indims(params.num_in);

  size_t buff_size = 0;
  for (const auto& i : *in_shape) buff_size += i.ndim();
  std::vector<uint32_t> inbuff(buff_size);
  uint32_t *ptr = inbuff.data();
  for (size_t i = 0; i < in_shape->size(); ++i) {
    inshapes[i] = ptr;
    indims[i] = (*in_shape)[i].ndim();
    for (int j = 0; j < (*in_shape)[i].ndim(); ++j, ++ptr) {
      *ptr = static_cast<uint32_t>((*in_shape)[i][j]);
    }
  }

  uint32_t** outshapes = nullptr;
  int* outdims = nullptr;
  CHECK(OpRegistry::get()->_callInferShape((mxAlloc_t)&(CustomOp::CallAllocator),
                                          params.op,
                                          params.op_type.c_str(),
                                          params.keys.data(),
                                          params.vals.data(),
                                          params.keys.size(),
                                          inshapes.data(),
                                          indims.data(),
                                          params.num_in,
                                          &outshapes,
                                          &outdims,
                                          params.num_out)
        ) << "Error inferring shapes for custom operator '" << params.op_type << "'";

  std::vector<uint32_t*> out_shapes(params.num_out);
  buff_size = 0;
  for (int i=0; i<params.num_out; i++) {
    buff_size += outdims[i];
  }

  std::vector<uint32_t> outbuff(buff_size);
  ptr = outbuff.data();
  for (int i = 0; i < params.num_out; ++i) {
    out_shapes[i] = ptr;
    for (int j = 0; j < outdims[i]; ++j, ++ptr) {
      *ptr = static_cast<uint32_t>(outshapes[i][j]);
    }
  }

  for (int i = 0; i < params.num_out; ++i) {
    SHAPE_ASSIGN_CHECK(*out_shape, i,
        mxnet::TShape(out_shapes[i], out_shapes[i]+outdims[i]));
  }

  //free memory used by custom op to allocate shapes/dims
  params.op->release(outdims);
  for(int i=0; i<params.num_out; i++) {
    params.op->release(outshapes[i]);
  }
  params.op->release(outshapes);

  return true;
}

void Forward_cpu(const nnvm::NodeAttrs& attrs,
                   const OpContext& ctx,
                   const std::vector<TBlob>& inputs,
                   const std::vector<OpReqType>& req,
                   const std::vector<TBlob>& outputs) {
  const CustomOpParam& params = nnvm::get<CustomOpParam>(attrs.parsed);
  CHECK(params.op->getFCompute_cpu()) << "Error! Custom operator '" << params.op_type << "' does not support CPU forward";

  //create a vector of tensors for inputs
  int num_in = inputs.size();
  std::vector<DLTensor> dl_inputs(num_in);
  for(int i=0; i<num_in; i++) {
    dl_inputs[i] = inputs[i].dltensor();
  }

  //create a vector of tensors for outputs
  int num_out = outputs.size();
  std::vector<DLTensor> dl_outputs(num_out);
  for(int i=0; i<num_out; i++) {
      dl_outputs[i] = outputs[i].dltensor();
  }

  CHECK(OpRegistry::get()->_callFCompute_cpu(params.op_type.c_str(),
                                         params.keys.data(),
                                         params.vals.data(),
                                         params.keys.size(),
                                         dl_inputs.data(),
                                         dl_inputs.size(),
                                         dl_outputs.data(),
                                         dl_outputs.size())
        ) << "Error calling CPU FCompute for custom operator '" << params.op_type << "'";
}

void Forward_gpu(const nnvm::NodeAttrs& attrs,
                   const OpContext& ctx,
                   const std::vector<TBlob>& inputs,
                   const std::vector<OpReqType>& req,
                   const std::vector<TBlob>& outputs) {
  const CustomOpParam& params = nnvm::get<CustomOpParam>(attrs.parsed);
  CHECK(params.op->getFCompute_gpu()) << "Error! Custom operator '" << params.op_type << "' does not support GPU forward";

  //create a vector of tensors for inputs
  int num_in = inputs.size();
  std::vector<DLTensor> dl_inputs(num_in);
  for(int i=0; i<num_in; i++) {
    dl_inputs[i] = inputs[i].dltensor();
  }

  //create a vector of tensors for outputs
  int num_out = outputs.size();
  std::vector<DLTensor> dl_outputs(num_out);
  for(int i=0; i<num_out; i++) {
      dl_outputs[i] = outputs[i].dltensor();
  }
  
  CHECK(OpRegistry::get()->_callFCompute_gpu(params.op_type.c_str(),
                                         params.keys.data(),
                                         params.vals.data(),
                                         params.keys.size(),
                                         dl_inputs.data(),
                                         dl_inputs.size(),
                                         dl_outputs.data(),
                                         dl_outputs.size())
        ) << "Error calling GPU FCompute for custom operator '" << params.op_type << "'";
}
 
}  // namespace op
}  // namespace mxnet
#endif  // MXNET_OPERATOR_CROP_INL_H_
