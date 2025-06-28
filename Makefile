HIPCC=hipcc

EXECUTABLE=hip_devmem_tcp

CXX=$(HIPCC)

.PHONY: all
all: $(EXECUTABLE)

$(EXECUTABLE): amdgpu_devmem_tcp.o ncdevmem.o
	$(HIPCC) -o $@ $^ -no-pie -lhsa-runtime64 -lynl -lamdhip64

amdgpu_devmem_tcp.o: amdgpu_devmem_tcp.cpp
	$(HIPCC) -fPIC -o $@ -c $^ -D__HIP_PLATFORM_AMD__

ncdevmem.o: ncdevmem.c
	$(HIPCC) -o $@ -c $^

clean:
	rm -f amdgpu_devmem_tcp.o ncdevmem.o $(EXECUTABLE)
