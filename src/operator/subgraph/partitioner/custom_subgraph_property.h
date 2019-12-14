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

#ifndef MXNET_OPERATOR_SUBGRAPH_PARTITIONER_CUSTOM_SUBGRAPH_PROPERTY_H_
#define MXNET_OPERATOR_SUBGRAPH_PARTITIONER_CUSTOM_SUBGRAPH_PROPERTY_H_

#include <string>
#include <utility>
#include <vector>
#include <nnvm/pass_functions.h>
#include "../common.h"
#include "../subgraph_property.h"
#include "../../include/mxnet/lib_api.h"
namespace mxnet {
namespace op {

/*
 * This selects nodes for a subgraph
 */
class CustomContainOpSelector: public SubgraphSelector {
 public:
  CustomContainOpSelector(std::vector<std::string> supportedNodes) :
    supportedNodes_(supportedNodes) {}
  virtual bool Select(const nnvm::Node &n) {
    return std::find(supportedNodes_.begin(), supportedNodes_.end(),
                     n.attrs.name) != supportedNodes_.end();
  }
  virtual bool SelectInput(const nnvm::Node &n, const nnvm::Node &new_node) {
    return false;
  }
  virtual bool SelectOutput(const nnvm::Node &n, const nnvm::Node &new_node) {
    return false;
  }
  std::vector<std::string> supportedNodes_;
};

/*
 * This subgraph property finds a subgraph
 */
class  CustomSubgraphProperty: public SubgraphProperty {
 public:
  CustomSubgraphProperty() {
    supportedOps_ = nullptr;
  }
  CustomSubgraphProperty(partCallSupportedOps_t callSupportedOps,
                         supportedOps_t supportedOps,
                         std::string op_name) :
  callSupportedOps_(callSupportedOps),
    supportedOps_(supportedOps),
    subgraph_op_name(op_name) {}
  // create custom subgraph property
  static SubgraphPropertyPtr Create() {
    return std::make_shared<CustomSubgraphProperty>();
  }
  void PrePartition(const nnvm::Graph& g,
    const std::vector<std::pair<std::string, std::string>>& options_map) {
    std::cout << "PrePartition" << std::endl;
    std::string subgraph_json = nnvm::pass::SaveJSON(g);
    int num_ids = 0;
    DFSVisit(g.outputs, [&](const nnvm::NodePtr& nptr) {
        nnvm::Node *node = nptr.get();
        // increment count for number of nodes in model
        num_ids++;
      });
    std::vector<int> supportedNodeIDs(num_ids, 0);
    if (supportedOps_ == nullptr) {
      std::cout << "supportedOps_ is null" << std::endl;
    } else {
      const char* json = subgraph_json.c_str();
      int *ids = supportedNodeIDs.data();
      int retval = callSupportedOps_(supportedOps_, json, num_ids, ids);
    }

    const auto& idx = g.indexed_graph();
    std::cout << "supportedNodes:" << std::endl;
    for (int i = 0; i < num_ids; i++) {
      if (supportedNodeIDs[i]) {
        supportedNodes.push_back(idx[i].source->attrs.name);
        std::cout << idx[i].source->attrs.name << std::endl;
      }
    }
  }
  // override CreateSubgraphNode
  virtual nnvm::NodePtr CreateSubgraphNode(const nnvm::Symbol &sym,
                                           const int subgraph_id = 0) const {
    nnvm::NodePtr n = nnvm::Node::Create();
    n->attrs.op = Op::Get(subgraph_op_name);
    n->attrs.name = "_op" + std::to_string(subgraph_id);
    n->attrs.subgraphs.push_back(std::make_shared<nnvm::Symbol>(sym));
    return n;
  }
  // override CreateSubgraphSelector
  virtual SubgraphSelectorPtr CreateSubgraphSelector() const {
    return std::make_shared<CustomContainOpSelector>(supportedNodes);
  }

  partCallSupportedOps_t callSupportedOps_;
  supportedOps_t supportedOps_;
  std::vector<std::string> supportedNodes;
  std::string subgraph_op_name;
};
}  // namespace op
}  // namespace mxnet

#endif  // MXNET_OPERATOR_SUBGRAPH_PARTITIONER_CUSTOM_SUBGRAPH_PROPERTY_H_
