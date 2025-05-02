#include "amdgpu_devmem_tcp.h"

#define NUM_PAGES 16000

int main(void)
{
	struct ctx *ctx = (struct ctx *)malloc(sizeof(struct ctx));
	int res;

	if (!ctx) {
		printf("[TEST]%s %u failed to allocate context structure\n", __func__, __LINE__);
		return -1;
	}
	ctx->size = 4096 * NUM_PAGES;
	res = init_hip(ctx);
	if (res) {
		printf("[TEST]%s %u failed to initialize context\n", __func__, __LINE__);
		free(ctx);
	}
	return 0;
}
