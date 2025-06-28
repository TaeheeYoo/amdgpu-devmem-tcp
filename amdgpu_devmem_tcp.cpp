#include "amdgpu_devmem_tcp.h"

#include <hip/hip_runtime.h>

#include <hsa/amd_hsa_common.h>
#include <hsa/hsa_ext_amd.h>

#include <stdio.h>

#define HIP_ASSERT(x) (assert((x) == hipSuccess))
#define HIP_CHECK(expression)                  				\
{                                              				\
    const hipError_t status = expression;      				\
    if (status != hipSuccess) {                  			\
	fprintf(stderr,"HIP error %d: %s: at %s:%d",			\
		       status, hipGetErrorString(status),		\
		       __FILE__, __LINE__);				\
    }									\
}

__global__ void
validation_kern(unsigned char* __restrict__ src, int len, unsigned char seed)
{
	int i;
	
	for (i = 0; i < len; i++)
		src[i] = 0xff;
}

int init_hip(struct ctx *ctx)
{
	unsigned char* devmem, *hostmem;
	int dmabuf_fd = -1;
	hsa_status_t err;
	uint64_t offset;
	int errors = 0;
	int i;

	if (!ctx || !ctx->size) {
		printf("[TEST]%s %u ctx or size is not defined\n", __func__, __LINE__);
		return -1;
	}
	hipDeviceProp_t devProp;
	HIP_CHECK(hipGetDeviceProperties(&devProp, 0));
	printf("System minor %d\n", devProp.minor);
	printf("System major %d\n", devProp.major);
	printf("Hip Device Prop Succeeded!\n");

	HIP_ASSERT(hipMalloc((void**)&devmem, ctx->size));
	hostmem = (unsigned char*) malloc(65536);
	if (!hostmem) {
		fprintf(stderr, "[TEST] %s %u hostmem alloc failed\n", __func__, __LINE__);
		return -1;
	}
	ctx->device_memory = devmem;
	ctx->host_memory = hostmem;
	err = hsa_amd_portable_export_dmabuf((void *)devmem,
					     ctx->size,
					     &dmabuf_fd,
					     &offset);
	if (err != HSA_STATUS_SUCCESS) {
		fprintf(stderr, "dmabuf export failed!\n");
		errors = -1;
	} else {
		ctx->memfd = dmabuf_fd;
		ctx->offset = offset;
		printf("dmabuf export succeeded! fd = %d devmem %p offset = %lu \n", dmabuf_fd, devmem, offset);
	}

	return errors;
}

int dispatch_validation(struct ctx *ctx, unsigned long long offset, unsigned int len)
{
	unsigned char *devmem = ctx->device_memory + offset;
	unsigned char *hostmem = ctx->host_memory;
	hipError_t err;

	HIP_CHECK(hipMemcpy(hostmem, devmem, len, hipMemcpyDeviceToHost));
	printf("[TEST] [%u] \n", hostmem[0]);
	hipLaunchKernelGGL(validation_kern, 
			   dim3(1),
			   dim3(1),
			   0, 0,
			   devmem, len, 0);

	err = hipDeviceSynchronize();
	if (err != hipSuccess)
		    fprintf(stderr, "Device sync failed: %s\n", hipGetErrorString(err));
	HIP_CHECK(hipMemcpy(hostmem, devmem, len, hipMemcpyDeviceToHost));

	printf("[TEST] [%u] \n", hostmem[0]);

	return 0;
}
