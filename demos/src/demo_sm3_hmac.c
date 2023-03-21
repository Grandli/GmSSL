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
#include <gmssl/sm3.h>
#include "demo_util.h"

#define OneTimeTestAmount  100*1024

unsigned int doHmacTest() {
    SM3_HMAC_CTX hmac_ctx;
    unsigned char key[16] = {
            0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
            0x01,0xf2,0x03,0x04,0x05,0x06,0x07,0x08,
    };
    uint8_t buf[OneTimeTestAmount] = "chinese standard message";
    size_t len = OneTimeTestAmount;
    unsigned char hmac[32] = {0};
    int i;


    sm3_hmac_init(&hmac_ctx, key, sizeof(key));
    sm3_hmac_update(&hmac_ctx, buf, len);
    sm3_hmac_finish(&hmac_ctx, hmac);

    //sm3_hmac(key, sizeof(key), buf, len, hmac);

    return OneTimeTestAmount;

}

int main(void)
{
    demoDoUtilTest(doHmacTest, 10, "hmac");

	return 0;
}
