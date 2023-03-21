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





void PrePareSM9Test()
{
    uint8_t buf[512];
    const char *id = "Alice";
    uint8_t *p = buf;
    const uint8_t *cp = buf;
    size_t len;

    sm9_enc_master_key_generate(&master);
    sm9_enc_master_key_extract_key(&master, id, strlen(id), &key);

    sm9_enc_master_public_key_to_der(&master, &p, &len);
    sm9_enc_master_public_key_from_der(&master_public, &cp, &len);
}

#define OneTimeTestAmount  10*1024
unsigned int DoSm9Test() {
    const char *id = "Alice";
    //明文数据
    unsigned char message[OneTimeTestAmount]= "chinese standard message";
    unsigned int messageLen = OneTimeTestAmount;
    //密文数据
    unsigned char cipherData[OneTimeTestAmount+1024];
    unsigned int cipherLen = OneTimeTestAmount+1024;
    //解密后的数据
    unsigned char outData[OneTimeTestAmount+1024];
    unsigned int outLen = OneTimeTestAmount+1024;
    int ret;
    sm9_encrypt(&master_public, id, strlen(id), message, messageLen, cipherData, &cipherLen);
    ret = sm9_decrypt(&key, id, strlen(id), cipherData, cipherLen, (uint8_t *)outData, &outLen);
    if (ret != 1) {
        return 0;
    }
    return OneTimeTestAmount;
}

int main(void)
{
    PrePareSM9Test();
    demoDoUtilTest(DoSm9Test, 1, "sm9");

	return 0;
}
