#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include <hip/hip_runtime.h>

#include "amdgpu_memory_provider.h"
#include "amdgpu_dmabuf_provider.h"

#include "ncdevmem.h"

#define NUM_PAGES 1024

static struct amdgpu_memory_provider *provider = &amdgpu_dmabuf_provider;

int main(int argc, char *argv[])
{
	struct amdgpu_memory_buffer *dmabuf;
	struct ncdevmem *ncdevmem;
	void *hostmem;
	hipError_t error;

	dmabuf = provider->alloc(getpagesize() * NUM_PAGES);
	if (dmabuf == NULL) {
		fprintf(stderr, "failed to provider->alloc()\n");
		exit(EXIT_FAILURE);
	}

	error = hipMalloc(&hostmem, getpagesize() * NUM_PAGES);
	if (error != hipSuccess) {
		fprintf(stderr, "failed to hipMalloc: %s\n",
				hipGetErrorString(error));
		exit(EXIT_FAILURE);
	}

	ncdevmem = ncdevmem_setup(0, 1, dmabuf->fd);
	if (ncdevmem == NULL) {
		fprintf(stderr, "failed to ncdevmem_setup()\n");
		exit(EXIT_FAILURE);
	}

	ncdevmem_cleanup(ncdevmem);
	provider->free(dmabuf);
	hipFree(hostmem);

	return 0;
}
