#include <assert.h>
#include <stdlib.h>
#include <unistd.h>

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

