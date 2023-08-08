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
#include <stdlib.h>
#include <sys/time.h>
#include <gmssl/sm4.h>
#include <gmssl/rand.h>
#include "demo_util.h"


#define OneTimeTestAmount  32

unsigned int DoSm4Test()
{
    SM4_KEY sm4_key;
    unsigned char key[16];
    char *originKey="0123456789abcdeffedcba9876543210";
    unsigned char iv[16];
    char *originIV="0123456789abcdeffedcba9876543210";
    char *plain =  "0123456789abcdeffedcba98765432100123456789abcdeffedcba98765432100123456789abcdeffedcba98765432100123456789abcdef";
    size_t plainLen = strlen(plain);
    //明文数据
    unsigned char *message = (unsigned char *)malloc(plainLen);
    size_t messageLen = plainLen;
    //密文数据
    unsigned char *cipherData = (unsigned char *)malloc(plainLen+16);
    size_t cipherLen = plainLen+16;
    //解密后的数据
    unsigned char *outData = (unsigned char *)malloc(plainLen+16);
    size_t outLen = plainLen+16;
    int i;
    char *realPlainData = NULL;

    messageLen = ComHexToStr(plain, message, plainLen);
    //rand_bytes(key, sizeof(key));
    //rand_bytes(originIV, sizeof(originIV));
    ComHexToStr(originIV, iv, 32);
    ComHexToStr(originKey, key, 32);
 //   memcpy(iv, originIV, 16);
//    memcpy(key, originKey, 16);


    sm4_set_encrypt_key(&sm4_key, key);
    sm4_ofb_encrypt(&sm4_key, iv, message, messageLen, cipherData);

    dumpMem(cipherData, messageLen);

    ComHexToStr(originIV, iv, 32);
    ComHexToStr(originKey, key, 32);
//    memcpy(iv, originIV, 16);
//    memcpy(key, originKey, 16);

    sm4_set_encrypt_key(&sm4_key, key);
    sm4_ofb_decrypt(&sm4_key, iv, cipherData, messageLen, outData);

    dumpMem(outData, messageLen);
    realPlainData = ComStrToHex(outData, messageLen, false);
    realPlainData[messageLen*2] = '\0';
    printf("out PlainData = %s\n", realPlainData);

    return messageLen;
}

unsigned int DoSm4Test2()
{
    SM4_KEY sm4_key;
    unsigned char key[16];
    char *originKey="0123456789abcdeffedcba9876543210";
    unsigned char iv[16];
    char *originIV="0123456789abcdeffedcba9876543210";
    char *plain =  "0123456789abcdeffedcba98765432100123456789abcdeffedcba98765432100123456789abcdeffedcba98765432100123456789abcdef";
    size_t plainLen = strlen(plain);
    //明文数据
    unsigned char *message = (unsigned char *)malloc(plainLen);
    size_t messageLen = plainLen;
    //密文数据
    unsigned char *cipherData = (unsigned char *)malloc(plainLen+16);
    size_t cipherLen = 0;
    size_t cipherLen2 = 0;
    //解密后的数据
    unsigned char *outData = (unsigned char *)malloc(plainLen+16);
    size_t outLen = 0;
    size_t outLen2 = 0;

    memcpy(message, plain, plainLen);
    //rand_bytes(key, sizeof(key));
    //rand_bytes(originIV, sizeof(originIV));
    ComHexToStr(originIV, iv, 32);
    ComHexToStr(originKey, key, 32);
//    memcpy(iv, originIV, 16);
//    memcpy(key, originKey, 16);


    SM4_OFB_CTX ofbCtx;
    sm4_ofb_encrypt_init(&ofbCtx, key, iv);
    sm4_ofb_encrypt_update(&ofbCtx, message, messageLen/2, cipherData, &cipherLen2);
    cipherLen += cipherLen2;
    sm4_ofb_encrypt_update(&ofbCtx, message+messageLen/2, messageLen-messageLen/2, cipherData+cipherLen, &cipherLen2);
    cipherLen += cipherLen2;
    sm4_ofb_encrypt_finish(&ofbCtx, cipherData+cipherLen, &cipherLen2);
    cipherLen += cipherLen2;

    dumpMem(cipherData, cipherLen);

    ComHexToStr(originIV, iv, 32);
    ComHexToStr(originKey, key, 32);
//    memcpy(iv, originIV, 16);
//    memcpy(key, originKey, 16);

    sm4_ofb_decrypt_init(&ofbCtx, key, iv);
    sm4_ofb_decrypt_update(&ofbCtx, cipherData, cipherLen/2, outData, &outLen2);
    outLen += outLen2;
    sm4_ofb_decrypt_update(&ofbCtx, cipherData+cipherLen/2, cipherLen - cipherLen/2, outData+outLen, &outLen2);
    outLen += outLen2;
    sm4_ofb_decrypt_finish(&ofbCtx, outData+outLen, &outLen2);
    outLen += outLen2;

    dumpMem(outData, outLen);

    return messageLen;
}

int main(void)
{
    DoSm4Test();
    DoSm4Test2();
    //demoDoUtilTest(DoSm4Test, 3, "sm4_ofb");

	return 0;
}
