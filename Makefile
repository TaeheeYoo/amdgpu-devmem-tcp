HIP_PATH?= $(wildcard /opt/rocm)
HIPCC=$(HIP_PATH)/bin/hipcc

SOURCES = amdgpu_devmem_tcp.cpp 
OBJECTS = $(SOURCES:.cpp=.o)

EXECUTABLE=./hip_devmem_tcp

.PHONY: test


all: $(EXECUTABLE) test

CXXFLAGS =-g

CXX=$(HIPCC)


$(EXECUTABLE): $(OBJECTS) 
	$(HIPCC) -L /opt/rocm/lib -I /opt/rocm/include $(SOURCES) --emit-static-lib -fPIC -o amdgpu_devmem_tcp.a -lhsa-runtime64
	$(HIPCC) -o hip_devmem_tcp server.cpp -L. -L /opt/rocm/lib -I /opt/rocm/include -lamdhip64 -lhsa-runtime64 -lamdgpu_devmem_tcp -D__HIP_PLATFORM_AMD__


test: $(EXECUTABLE)
	$(EXECUTABLE)


clean:
	rm -f $(EXECUTABLE)
	rm -f $(OBJECTS)
	rm -f $(HIP_PATH)/src/*.o
