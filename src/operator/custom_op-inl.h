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

namespace mxnet {
namespace op {

struct CustomOpParam {
  std::string op_type;
};

void AttrParser(NodeAttrs* attrs) {
  attrs->parsed = CustomOpParam();
  CustomOpParam& params = nnvm::get<CustomOpParam>(attrs->parsed);

  char const** keys;
  char const** vals;
  int idx=0;
  for (auto& p : attrs->dict) {
    if (p.first == "op_type") {
      params.op_type = p.second;
    }
    else {
      keys[idx] = p.first.c_str();
      vals[idx++] = p.second.c_str();
    }
  }
  
  CHECK(!params.op_type.empty()) << "Required argument `op_type` is missing.";
  CHECK(OpRegistry::get()->hasOp(params.op_type)) << "Cannot find custom operator " << params.op_type;
  CustomOp *op = OpRegistry::get()->op(params.op_type);
  op->getParseAttrs()(keys,vals,idx);
  /*

  CHECK(creator(params.op_type.c_str(), keys.size(), keys.data(),
                vals.data(), params.info.get()));

  params.num_args = List<kCustomOpPropListArguments>(*attrs).size();
  params.num_outs = List<kCustomOpPropListOutputs>(*attrs).size();
  params.num_auxs = List<kCustomOpPropListAuxiliaryStates>(*attrs).size();

  int num_dep, *rdeps, counter = 0;
  std::vector<int> out_grad, in_data, out_data;
  for (size_t i = 0; i < params.num_outs; ++i) out_grad.push_back(counter++);
  for (size_t i = 0; i < params.num_args; ++i) in_data.push_back(counter++);
  for (size_t i = 0; i < params.num_outs; ++i) out_data.push_back(counter++);
  CHECK(reinterpret_cast<CustomOpBwdDepFunc>(
    params.info->callbacks[kCustomOpPropDeclareBackwardDependency])(
      out_grad.data(), in_data.data(), out_data.data(), &num_dep,
      &rdeps, params.info->contexts[kCustomOpPropDeclareBackwardDependency]));
  params.bwd_idx.insert(params.bwd_idx.end(), rdeps, rdeps+num_dep);
  */
}

bool InferType(const NodeAttrs& attrs,
               std::vector<int> *in_type,
               std::vector<int> *out_type) {
  out_type[0] = in_type[0];
  /*
  const CustomParam& params = nnvm::get<CustomParam>(attrs.parsed);

  if (params.info->num_callbacks <= kCustomOpPropInferType) {
    return ElemwiseAttr<int, type_is_none, type_assign, true, type_string>(
        attrs, in_type, out_type, -1);
  }

  std::vector<int> types;
  types.reserve(params.num_args + params.num_outs + params.num_auxs);
  for (size_t i = 0; i < params.num_args; ++i) {
    types.push_back((*in_type)[i]);
  }
  for (const auto& i : *out_type) {
    types.push_back(i);
  }
  for (size_t i = 0; i < params.num_auxs; ++i) {
    types.push_back((*in_type)[params.num_args+i]);
  }

  CHECK(reinterpret_cast<CustomOpInferTypeFunc>(
      params.info->callbacks[kCustomOpPropInferType])(
          types.size(), types.data(), params.info->contexts[kCustomOpPropInferType]));

  for (size_t i = 0; i < params.num_args; ++i) {
    TYPE_ASSIGN_CHECK(*in_type, i, types[i]);
  }
  for (size_t i = 0; i < params.num_outs; ++i) {
    TYPE_ASSIGN_CHECK(*out_type, i, types[params.num_args+i]);
  }
  for (size_t i = 0; i < params.num_auxs; ++i) {
    TYPE_ASSIGN_CHECK(*in_type, params.num_args+i,
                      types[params.num_args+params.num_outs+i]);
  }
  */
  return true;
}

 bool InferShape(const NodeAttrs& attrs,
                mxnet::ShapeVector *in_shape,
                mxnet::ShapeVector *out_shape) {
   /*
  const CustomParam& params = nnvm::get<CustomParam>(attrs.parsed);

  size_t total = params.num_args + params.num_outs + params.num_auxs;
  std::vector<uint32_t*> shapes(total);
  std::vector<int> ndims(total);
  size_t buff_size = 0;
  for (const auto& i : *in_shape) buff_size += i.ndim();
  std::vector<uint32_t> buff(buff_size);
  uint32_t *ptr = buff.data();
  for (size_t i = 0; i < in_shape->size(); ++i) {
    shapes[i] = ptr;
    ndims[i] = (*in_shape)[i].ndim();
    for (size_t j = 0; j < (*in_shape)[i].ndim(); ++j, ++ptr) {
      *ptr = static_cast<uint32_t>((*in_shape)[i][j]);
    }
  }

  CHECK(reinterpret_cast<CustomOpInferShapeFunc>(
      params.info->callbacks[kCustomOpPropInferShape])(
          shapes.size(), ndims.data(), shapes.data(),
          params.info->contexts[kCustomOpPropInferShape]));

  for (size_t i = 0; i < params.num_args; ++i) {
    SHAPE_ASSIGN_CHECK(*in_shape, i, mxnet::TShape(shapes[i], shapes[i]+ndims[i]));
  }

  size_t base = params.num_args;
  for (size_t i = 0; i < params.num_outs; ++i) {
    SHAPE_ASSIGN_CHECK(*out_shape, i,
        mxnet::TShape(shapes[base+i], shapes[base+i]+ndims[base+i]));
  }

  base = params.num_args + params.num_outs;
  for (size_t i = 0; i < params.num_auxs; ++i) {
    SHAPE_ASSIGN_CHECK(*in_shape, params.num_args+i,
        mxnet::TShape(shapes[base+i], shapes[base+i]+ndims[base+i]));
  }
  */
  return true;
}

void Forward(const nnvm::NodeAttrs& attrs,
                   const OpContext& ctx,
                   const std::vector<TBlob>& inputs,
                   const std::vector<OpReqType>& req,
                   const std::vector<TBlob>& outputs) {
  const CustomOpParam& params = nnvm::get<CustomOpParam>(attrs.parsed);

  fcomp_t fcomp = OpRegistry::get()->op(params.op_type)->getFCompute();
  
  std::cout << "Forward: " << params.op_type << std::endl;
  fcomp();
}
 
}  // namespace op
}  // namespace mxnet
#endif  // MXNET_OPERATOR_CROP_INL_H_
