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
#include <gmssl/sm9.h>
#include <gmssl/error.h>
#include "demo_util.h"

static SM9_ENC_MASTER_KEY master;
static SM9_ENC_MASTER_KEY master_public;
static SM9_ENC_KEY key;




//提前准备sm9测试
void PrePareSM9Test()
{
    uint8_t buf[512];
    const char *id = "Alice";
    uint8_t *p = buf;
    const uint8_t *cp = buf;
    size_t len = 0;

    sm9_enc_master_key_generate(&master);
    sm9_enc_master_key_extract_key(&master, id, strlen(id), &key);

    sm9_enc_master_public_key_to_der(&master, &p, &len);
    sm9_enc_master_public_key_from_der(&master_public, &cp, &len);

    sm9_enc_reCompute(master_public.Ppube, 1);
    sm9_enc_preCompute_for_user(master_public.Ppube, id, strlen(id), 1);
}

#define OneTimeTestAmount  1024
unsigned int DoSm9Test() {
    const char *id = "Alice";
    //明文数据
    unsigned char message[OneTimeTestAmount]= "chinese standard message";
    unsigned long messageLen = OneTimeTestAmount;//strlen(message);
    //密文数据
    unsigned char cipherData[OneTimeTestAmount+1024];
    unsigned long cipherLen = OneTimeTestAmount+1024;
    //解密后的数据
    unsigned char outData[OneTimeTestAmount+1024]="";
    unsigned long outLen = OneTimeTestAmount+1024;
    int ret;
    unsigned int idLen = strlen(id);
    ret = sm9_encrypt(&(master_public.Ppube), id, idLen, message, messageLen, cipherData, &cipherLen, 0x11);
    if (ret != 1) {
        return 0;
    }
    //dumpMem(cipherData, cipherLen);
    //printf("sm9_encrypt cipherLen = %ld", cipherLen);
    ret = sm9_decrypt(&key, id, idLen, cipherData, cipherLen, (uint8_t *)outData, &outLen);
    if (ret != 1) {
        return 0;
    }
    //如果数据不一致
    if(memcmp(outData, message, outLen)!=0)
    {
        printf("outData[%ld] = %s\n", outLen, outData);
        return 0;
    }
    return messageLen;
}

//static void doTestMul()
//{
//    sm9_fp4_t a0 = {{{2, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}}, {{0, 0, 0, 0, 0, 0, 0, 0}, {7, 0, 0, 0, 0, 0, 0, 0}}};
//    sm9_fp4_t a1 = {{{3, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}}, {{0, 0, 0, 0, 0, 0, 0, 0}, {13, 0, 0, 0, 0, 0, 0, 0}}};
//    sm9_fp4_t a2 = {{{4, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}}, {{0, 0, 0, 0, 0, 0, 0, 0}, {1, 0, 0, 0, 0, 0, 0, 0}}};
//    sm9_fp4_t b1 = {{{5, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}}, {{0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}}};
//    sm9_fp4_t r0, r1, r2, t;
//
//    sm9_fp4_mul(r0, a0, a1);
//    sm9_fp4_mul_v(t, a0, a1);
//    sm9_fp4_sqr_v(r1, a2);
//
//    sm9_fp4_add(r0, r0, t);
//    sm9_fp4_mul_v(t, a2, b1);
//    sm9_fp4_add(r0, r0, t);
//
//}

static void doTestMul()
{
    sm9_fp2_t a0 = {{2, 0, 0, 0, 0, 0, 0, 0}, {1, 0, 0, 0, 0, 0, 0, 0}};
    sm9_fp2_t a1 = {{3, 0, 0, 0, 0, 0, 0, 0}, {1, 0, 0, 0, 0, 0, 0, 0}};
    sm9_fp2_t a2 = {{2, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}};
    sm9_fp2_t b1 = {{1, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 0, 0, 0, 0, 0}};
    sm9_fp2_t r0, r1, r2, t;

    sm9_fp2_mul(r0, a0, a1);
    sm9_fp2_mul_u(t, a0, a1);
    sm9_fp2_sqr_u(r1, a2);

    sm9_fp2_add(r0, r0, t);


}

int main(void)
{
    doTestMul();
    PrePareSM9Test();
    int i = 0;
    for(i=0;i<3;i++)
    {
        demoDoUtilTest(DoSm9Test, 1, "sm9");
    }


	return 0;
}
