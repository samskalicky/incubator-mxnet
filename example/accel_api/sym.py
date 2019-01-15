import os
os.environ['MXNET_ENGINE_TYPE'] = 'NaiveEngine'


import mxnet as mx


ctx = mx.context.load_acc('../example/accel_api/libmyacc.so')
print(ctx)

data = mx.nd.empty(4,ctx=ctx)
a = mx.sym.var('data')
b = mx.sym.exp(a)

exe = b.bind(ctx, {'data':data}, grad_req='null')
out = exe.forward()
