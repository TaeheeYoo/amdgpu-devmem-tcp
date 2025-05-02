#include <assert.h>
#include <algorithm>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include "hsa/hsa.h"
#include "hip/hip_runtime.h"
#include "hsa/hsa_ext_finalize.h"
#include "hsa/hsa_ext_image.h"
#include "hsa/hsa_ext_amd.h"

int do_backend(void);
