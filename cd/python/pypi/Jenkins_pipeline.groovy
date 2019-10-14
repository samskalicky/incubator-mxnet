// -*- mode: groovy -*-

// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.
//
// Jenkins pipeline
// See documents at https://jenkins.io/doc/book/pipeline/jenkinsfile/

// NOTE: 
// ci_utils and cd_utils are loaded by the originating Jenkins job, e.g. jenkins/Jenkinsfile_release_job

// Only post the following variants to PyPI.
// This is a temporary solution until we are confident with the packages generated by CI
// This should be removed in the not too distant future.
// We only skip the publish step so we can still QA the other variants.
pypi_releases = ["cu92", "cu92mkl"]

def get_pipeline(mxnet_variant) {
  def node_type = mxnet_variant.startsWith('cu') ? NODE_LINUX_GPU : NODE_LINUX_CPU
  return cd_utils.generic_pipeline(mxnet_variant, this, node_type)
}

def get_environment(mxnet_variant) {
  def environment = "ubuntu_cpu"
  if (mxnet_variant.startsWith('cu')) {
    environment = "ubuntu_gpu_${mxnet_variant}".replace("mkl", "")
  }
  return environment
}

def build(mxnet_variant) {
  ws("workspace/python_pypi/${mxnet_variant}/${env.BUILD_NUMBER}") {
    ci_utils.init_git()
    cd_utils.restore_artifact(mxnet_variant, 'static')
    
    // create wheel file
    def environment = get_environment(mxnet_variant)
    def nvidia_docker = mxnet_variant.startsWith('cu')
    ci_utils.docker_run(environment, "cd_package_pypi ${mxnet_variant}", nvidia_docker, '500m', "RELEASE_BUILD='${env.RELEASE_BUILD}'")
  }
}

def test(mxnet_variant) {
  ws("workspace/python_pypi/${mxnet_variant}/${env.BUILD_NUMBER}") {
    // test wheel file
    def environment = get_environment(mxnet_variant)
    def nvidia_docker = mxnet_variant.startsWith('cu')
    ci_utils.docker_run(environment, "cd_integration_test_pypi python ${nvidia_docker}", nvidia_docker)
    ci_utils.docker_run(environment, "cd_integration_test_pypi python3 ${nvidia_docker}", nvidia_docker)
  }
}

def push(mxnet_variant) {
  ws("workspace/python_pypi/${mxnet_variant}/${env.BUILD_NUMBER}") {
    // publish package to pypi
    if (mxnet_variant in pypi_releases) {
      sh "./ci/docker/runtime_functions.sh cd_pypi_publish"
    } else {
      echo "Temporarily skipping publishing PyPI package for '${mxnet_variant}'."
    }
  }
}

return this
