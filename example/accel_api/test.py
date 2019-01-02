import mxnet as mx

ctx = mx.context.load_acc('libmyacc.so')
print(ctx)

mx.nd.empty(4,ctx=ctx)
