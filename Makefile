HIP_PATH?= $(wildcard /opt/rocm)
HIPCC=$(HIP_PATH)/bin/hipcc

EXECUTABLE=./hip_devmem_tcp

.PHONY: test

all: $(EXECUTABLE)

CXXFLAGS =-g

CXX=$(HIPCC)

$(EXECUTABLE):
	$(HIPCC) -L /opt/rocm/lib \
		-I /opt/rocm/include \
		-I /usr/include amdgpu_devmem_tcp.cpp \
		--emit-static-lib -fPIC \
		-o libamdgpu_devmem_tcp.a \
		-lhsa-runtime64 \
		-D__HIP_PLATFORM_AMD__
	$(HIPCC) -x c ncdevmem.c \
		-o hip_ncdevmem \
		-I. \
		-I /usr/include \
		-I /usr/src/`uname -r`/include \
		-I /root/tools/net/ynl/generated \
		-I /root/tools/net/ynl/lib \
		-L. \
		-L /root/tools/net/ynl/lib \
		-L /root/tools/net/ynl \
		-L /opt/rocm/lib \
		-I /opt/rocm/include \
		-lamdhip64 \
		-lhsa-runtime64 \
		-lamdgpu_devmem_tcp \
		-lynl \
		-D__HIP_PLATFORM_AMD__

test: $(EXECUTABLE)
	$(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)
	rm -f $(OBJECTS)
	rm -f ncdevmem
	rm -rf libamdgpu_devmem_tcp.a
