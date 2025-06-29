CC := hipcc
CXX := hipcc

OUTPUT := hip_devmem_tcp

CFLAGS +=  -fPIC
LDLIBS += -lhsa-runtime64 -lynl -lamdhip64
LDFLAGS += -L/usr/libexec/kselftests/net/lib/ynl -no-pie
