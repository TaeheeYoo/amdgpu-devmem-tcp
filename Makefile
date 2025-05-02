HIP_PATH?= $(wildcard /opt/rocm)
HIPCC=$(HIP_PATH)/bin/hipcc

SOURCES = amdgpu_devmem_tcp.cpp 
OBJECTS = $(SOURCES:.cpp=.o)

EXECUTABLE=./amdgpu_devmem_tcp

.PHONY: test


all: $(EXECUTABLE) test

CXXFLAGS =-g

CXX=$(HIPCC)


$(EXECUTABLE): $(OBJECTS) 
	$(HIPCC) -L /opt/rocm/lib $(OBJECTS) -o $@ -lhsa-runtime64


test: $(EXECUTABLE)
	$(EXECUTABLE)


clean:
	rm -f $(EXECUTABLE)
	rm -f $(OBJECTS)
	rm -f $(HIP_PATH)/src/*.o
