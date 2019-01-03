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

/*!
 * Copyright (c) 2015 by Contributors
 * \file acc_storage_manager.h
 * \brief Accelerator Storage manager
 */
#ifndef MXNET_ACC_STORAGE_MANAGER_H_
#define MXNET_ACC_STORAGE_MANAGER_H_

#include <mxnet/base.h>
#include <mxnet/storage.h>
#include <unordered_map>
#include <algorithm>
#include <vector>
#include <mutex>
#include <new>
#include "./storage_manager.h"
#include "../common/utils.h"

namespace mxnet {
namespace storage {

/*!
 * \brief Accelerator Storage manager 
 */
class AccStorageManager final : public StorageManager {
 public:
  /*!
   * \brief Default constructor.
   */
 AccStorageManager(Context& context) : ctx(context) {}

  /*!
   * \brief Default destructor.
   */
  ~AccStorageManager() {
    ReleaseAll();
  }

  void Alloc(Storage::Handle* handle) override;
  void Free(Storage::Handle handle) override;
  void DirectFree(Storage::Handle handle) override;
  
 private:
  Context ctx;
  void ReleaseAll();
};  // class AccStorageManager

void AccStorageManager::Alloc(Storage::Handle* handle) {
  LOG(FATAL) << "Unimplemented alloc for device " << handle->ctx;
}

void AccStorageManager::Free(Storage::Handle handle) {
  LOG(FATAL) << "Unimplemented free for device " << handle.ctx;
}

void AccStorageManager::DirectFree(Storage::Handle handle) {
  LOG(FATAL) << "Unimplemented directfree for device " << handle.ctx;
}
 
void AccStorageManager::ReleaseAll() {
  LOG(FATAL) << "Unimplemented releaseAll for device " << ctx;
}

}  // namespace storage
}  // namespace mxnet

#endif  // MXNET_ACC_STORAGE_MANAGER_H_
