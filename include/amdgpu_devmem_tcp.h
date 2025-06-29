#ifndef AMD_GPU_DEVMEM_TCP_H__
#define AMD_GPU_DEVMEM_TCP_H__

struct ctx {
	unsigned char* device_memory;
	unsigned char* host_memory;
	int memfd;
	unsigned int size;
	int offset;
};

#ifdef __cplusplus
extern "C" {
#endif
int init_hip(struct ctx *ctx);
int dispatch_validation(struct ctx *ctx, unsigned long long offset, unsigned int len);
#ifdef __cplusplus
}
#endif

#endif
