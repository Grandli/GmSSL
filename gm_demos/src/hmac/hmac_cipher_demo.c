#include "hmac_cipher_demo.h"

//测试用例1
static int doTestSuit1() {
    //hmac运算的密钥
    char key[] = "goodluck";
    //需要计算hmac的消息
    char message[]= "chinese standard message";
    //期望的哈希值（16进制编码）（根据上面两个的信息）
    char myHmac[] = "1F5E75076D73E62513C7B2E18455FE68650F94C73EEB83AB7A074C019E863A30";
    //返回的hmac的比特数据
    unsigned char mac[32] = "";
    unsigned char mac2[32] = "";
    unsigned int macLen = 32;
    int iRet = 0;
    char *outMac;

    unsigned char hmac[32] = {0};
    int i;
    SM3_HMAC_CTX hmac_ctx;


    sm3_hmac_init(&hmac_ctx, key, strlen(key));
    sm3_hmac_update(&hmac_ctx, message, strlen(message));
    sm3_hmac_finish(&hmac_ctx, hmac);

    outMac = ComStrToHex(hmac, 32, true);
    if(memcmp(outMac, myHmac, strlen(myHmac))!=0)
    {
        ComPrintf("hmac data is error\n");
        return -1;
    }

    ComPrintf("hmac test success, mac = %s\n", outMac);

    return GM_SUCCESS;
}

static const char *testName() {
    return "hmac";
}


static TestResultInfo *doTest(int testTimes) {
    //开始测试
    CommonBeginTest(&hmacCipherDemo, testName(), testTimes);

    while(1)
    {
        hmacCipherDemo.haveTestTimes++;
        if(doTestSuit1()==GM_SUCCESS)
        {
            hmacCipherDemo.resultInfo->successTimes++;
        }
        if(hmacCipherDemo.haveTestTimes>=hmacCipherDemo.resultInfo->totalTimes)
        {
            break;
        }
    }

    //结束测试
    CommonEndTest(&hmacCipherDemo);

    return hmacCipherDemo.resultInfo;
}



Cipher_Demo_Base hmacCipherDemo = {
        NULL,
        0,
        0,
        0,
        doTest,
        testName
};
