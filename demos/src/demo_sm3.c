/*
 *  Copyright 2014-2022 The GmSSL Project. All Rights Reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the License); you may
 *  not use this file except in compliance with the License.
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <gmssl/sm3.h>
#include "demo_util.h"

#define OneTimeTestAmount  8*1024


unsigned int doSm3Test()
{
    SM3_CTX sm3_ctx;
    uint8_t buf[OneTimeTestAmount] = "chinese standard message";
    size_t len = OneTimeTestAmount;
    uint8_t dgst[32];

    sm3_init(&sm3_ctx);
    sm3_update(&sm3_ctx, buf, strlen(buf));
    sm3_finish(&sm3_ctx, dgst);
    //sm3_digest(buf, len, dgst);
    //printf("dgst = %s\n", ComStrToHex(dgst, 32, true));
    dumpMem(dgst, 32);

    return OneTimeTestAmount;
}

int main(int argc, char **argv)
{

    demoDoUtilTest(doSm3Test, 1, "sm3");

	return 0;
}
