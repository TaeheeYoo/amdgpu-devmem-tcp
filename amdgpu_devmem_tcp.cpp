/*
   Copyright (c) 2015-2016 Advanced Micro Devices, Inc. All rights reserved.

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
 */

#include "amdgpu_devmem_tcp.h"
#include <iostream>
#include "hsa/hsa.h"
#include "hip/hip_runtime.h"
#include "hsa/hsa_ext_amd.h"

#ifdef NDEBUG
#define HIP_ASSERT(x) x
#else
#define HIP_ASSERT(x) (assert((x)==hipSuccess))
#endif

__global__ void 
validation_kern(unsigned char* __restrict__ src, int len, unsigned char seed)

{
	int i;
	
	for (i = 0; i < len; i++)
		src[i] = 0xff;
}

using namespace std;
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
	hipGetDeviceProperties(&devProp, 0);
	cout << " System minor " << devProp.minor << endl;
	cout << " System major " << devProp.major << endl;
	cout << " agent prop name " << devProp.name << endl;
	cout << "hip Device prop succeeded " << endl ;

	HIP_ASSERT(hipMalloc((void**)&devmem, ctx->size));
	hostmem = (unsigned char*)malloc(65536);
	if (!hostmem) {
		printf("[TEST]%s %u hostmem alloc failed\n", __func__, __LINE__);
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

	hipMemcpy(hostmem, devmem, len, hipMemcpyDeviceToHost);
	printf("[TEST] [%u] \n", hostmem[0]);
	hipLaunchKernelGGL(validation_kern, 
			   dim3(1),
			   dim3(1),
			   0, 0,
			   devmem, len, 0);

	err = hipDeviceSynchronize();
	if (err != hipSuccess)
		    fprintf(stderr, "Device sync failed: %s\n", hipGetErrorString(err));
	hipMemcpy(hostmem, devmem, len, hipMemcpyDeviceToHost);

	printf("[TEST] [%u] \n", hostmem[0]);

	return 0;
}
