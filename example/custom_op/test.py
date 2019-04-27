import mxnet as mx

mx.operator.load_op_lib('libtest.so')

'''
try:
    mx.nd.CustomOp(op_type='sams')
except Exception as e:
    print(e)
'''

a = mx.nd.ones((1,))
print(a)
b = mx.nd.CustomOp(a,op_type='sam',myParam='2')
print(b)

