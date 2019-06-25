import mxnet as mx
import time

mx.operator.load_op_lib('libtest.so')

size = 2048
A = mx.nd.random.randn(size,size,dtype='double')
B = mx.nd.random.randn(size,size,dtype='double')
C = mx.nd.empty((size,size),dtype='double')
print(A)
print(B)

#benchmark the custom operator
times = []
for i in range(10):    
    start = time.time()
    C = mx.nd.CustomOp(A,B, op_type='sam', myParam=0)
    mx.nd.waitall()
    stop = time.time()
    elapsed = stop-start
    print('elapsed: %4.3f seconds' % elapsed)
    times.append(elapsed)

print(C)
print(times)

