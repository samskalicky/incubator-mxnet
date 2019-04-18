import mxnet as mx

mx.operator.load_op_lib('libtest.so')

mx.nd.CustomOp('sam')
