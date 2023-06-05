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

//把字符串数据转为十六进制的字符串
static char *ComStrToHex(unsigned char *inData, int inLen, bool bUpper)
{
    int outLen = inLen * 2 +2;
    int i = 0;
    char *outData = (char*) malloc(outLen);
    if (!outData)
        return "";
    memset(outData, 0, outLen);

    for(i=0; i<inLen; i++){
        sprintf(outData+i*2, bUpper?"%02X":"%02x", inData[i]);
    }
    return outData;
}

unsigned int doSm3Test()
{
    SM3_CTX sm3_ctx;
    uint8_t buf[OneTimeTestAmount] = "chinese standard message";
    size_t len = OneTimeTestAmount;
    uint8_t dgst[32];

    sm3_init(&sm3_ctx);
    sm3_update(&sm3_ctx, buf, len);
    sm3_finish(&sm3_ctx, dgst);
    //sm3_digest(buf, len, dgst);
    //printf("dgst = %s\n", ComStrToHex(dgst, 32, true));

    return OneTimeTestAmount;
}

int main(int argc, char **argv)
{

    demoDoUtilTest(doSm3Test, 3, "sm3");

	return 0;
}
