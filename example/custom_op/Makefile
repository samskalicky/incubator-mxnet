.PHONY: lib test

all: lib test

lib:
	g++ -std=c++11 -o libtest.so -Wl,--export-dynamic -DMXNET_CUSTOM_OP -shared -fpic test.cc -I../../include/mxnet

test:
	g++ -std=c++11 -o test test_customOp.cc -ldl -I../../include/mxnet

clean:
	rm -rf test
ultraclean: clean
	rm -rf *.so
