import mxnet as mx

ctx = mx.context.load_acc('../example/accel_api/libmyacc.so')
print(ctx)

a = mx.nd.empty(4,ctx=ctx)
print(a)

b = mx.nd.exp(a)
