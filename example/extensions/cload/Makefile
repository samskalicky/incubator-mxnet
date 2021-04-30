.PHONY: test1, test2

all:
	g++ -std=c++11 -O3 -o test main.cc -I../../../include -L../../../lib -lmxnet

test1:
	@LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:../../../lib; ./test ../lib_api/libinit_lib.so
test2:
	@LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:../../../lib; ./test ../lib_custom_op/libgemm_lib.so
