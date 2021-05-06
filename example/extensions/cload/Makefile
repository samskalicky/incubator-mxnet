.PHONY: load, link, test1, test2, clean

all:
	@echo "options:"
	@echo "   link: build application for dynamic linking"
	@echo "   load: build application for dynamic loading"
	@echo "   test1: build test for dynamic linking example using init lib"
	@echo "   test2: build test for dynamic linking example using custom op lib"
	@echo "   test3: build test for dynamic loading example"

link:
	g++ -std=c++11 -O3 -o link dynamic_link.cc -I../../../include -Lmxnet -lmxnet

load:
	g++ -std=c++11 -O3 -o load dynamic_load.cc -I../../../include -ldl

test1:
	@LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:mxnet; ./link ../lib_api/libinit_lib.so
test2:
	@LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:mxnet; ./link ../lib_custom_op/libgemm_lib.so
test3:
	@./load ../lib_api/libinit_lib.so aws-mx/mxnet/libmxnet.so 

clean:
	rm -rf *.o *~ load link
