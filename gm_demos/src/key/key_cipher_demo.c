#include "key_cipher_demo.h"

//测试用例1
static int doTestSuit1() {


    return GM_SUCCESS;
}


static const char *testName() {
    return "key";
}

static TestResultInfo *doTest(int testTimes) {
    //开始测试
    CommonBeginTest(&keyCipherDemo, testName(), testTimes);

    while(1)
    {
        keyCipherDemo.haveTestTimes++;
        if(doTestSuit1()==GM_SUCCESS)
        {
            keyCipherDemo.resultInfo->successTimes++;
        }
        if(keyCipherDemo.haveTestTimes>=keyCipherDemo.resultInfo->totalTimes)
        {
            break;
        }
    }

    //结束测试
    CommonEndTest(&keyCipherDemo);

    return keyCipherDemo.resultInfo;
}

Cipher_Demo_Base keyCipherDemo = {
        NULL,
        0,
        0,
        0,
        doTest,
        testName
};
