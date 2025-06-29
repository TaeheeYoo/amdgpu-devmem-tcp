CC := hipcc
CXX := hipcc

OUTPUT := hip_devmem_tcp

CFLAGS +=  -fPIC -D__HIP_PLATFORM_AMD__
LDLIBS += -lhsa-runtime64 -lynl -lamdhip64
LDFLAGS += 
