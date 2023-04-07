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
#include <gmssl/sm2.h>
#include <gmssl/error.h>
#include "demo_util.h"

SM2_KEY sm2_key;
SM2_KEY pub_key;
unsigned char dgst[32];

//提前准备sm2测试
void PrePareSM2SignTest()
{
    sm3_digest((unsigned char *)"hello world", strlen("hello world"), dgst);
    sm2_key_generate(&sm2_key);
}

#define OneTimeTestAmount  5*1024
unsigned int DoSm2SignTest()
{
    unsigned char sig[SM2_MAX_SIGNATURE_SIZE];
    size_t sigLen;
    int ret;
    sm2_sign(&sm2_key, dgst, sig, &sigLen);

    memcpy(&pub_key, &sm2_key, sizeof(SM2_POINT));

    ret = sm2_verify(&pub_key, dgst, sig, sigLen);
    if (ret!= 1) {
        //fprintf(stderr, "verify failed\n");
        return 0;
    } else {
        //printf("verify success\n");
    }
    return 32;
}


int main(void)
{
    PrePareSM2SignTest();
    demoDoUtilTest(DoSm2SignTest, 1, "sm2Sign");


	return 0;
}
