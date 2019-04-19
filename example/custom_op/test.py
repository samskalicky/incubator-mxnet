import mxnet as mx

mx.operator.load_op_lib('libtest.so')

try:
    mx.nd.CustomOp('sams')
except Exception as e:
    print(e)

mx.nd.CustomOp('sam')


