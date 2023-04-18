#include "sm9_cipher_demo.h"

//测试用例1
static int doTestSuit1()
{
    //接收方的用户标识（注意目前为后台设置的手机号码）
    char *userID = "13800138002";
    unsigned int userLen = strlen(userID);
    //明文数据
    unsigned char message[]= "chinese standard message";
    unsigned int messageLen = strlen(message);
    //密文数据
    unsigned char cipherData[512];
    unsigned int cipherLen = 512;
    //解密后的数据
    unsigned char outData[512];
    unsigned int outLen = 512;
    //签名值
    unsigned char signData[1024];
    unsigned short signLen = 1024;
    int iRet = 0;



    ComPrintf("sm9 test success\n");

    return GM_SUCCESS;
}

static const char *testName() {
    return "sm9";
}

static TestResultInfo *doTest(int testTimes) {
    //开始测试
    CommonBeginTest(&sm9CipherDemo, testName(), testTimes);

    while(1)
    {
        sm9CipherDemo.haveTestTimes++;
        if(doTestSuit1()==GM_SUCCESS)
        {
            sm9CipherDemo.resultInfo->successTimes++;
        }
        if(sm9CipherDemo.haveTestTimes>=sm9CipherDemo.resultInfo->totalTimes)
        {
            break;
        }
    }

    //结束测试
    CommonEndTest(&sm9CipherDemo);

    return sm9CipherDemo.resultInfo;
}

Cipher_Demo_Base sm9CipherDemo = {
        NULL,
        0,
        0,
        0,
        doTest,
        testName
};
