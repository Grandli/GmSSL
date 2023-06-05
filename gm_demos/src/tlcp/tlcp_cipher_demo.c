#include "tlcp_cipher_demo.h"
#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <gmssl/tls.h>


//测试用例1
static int doTestSuit1()
{

    ComPrintf("tlcp test success\n");
    return GM_SUCCESS;
}

static const char *testName() {
    return "tlcp";
}

static TestResultInfo *doTest(int testTimes) {
    //开始测试
    CommonBeginTest(&tlcpCipherDemo, testName(), testTimes);

    while(1)
    {
        tlcpCipherDemo.haveTestTimes++;
        if(doTestSuit1()==GM_SUCCESS)
        {
            tlcpCipherDemo.resultInfo->successTimes++;
        }
        if(tlcpCipherDemo.haveTestTimes>=tlcpCipherDemo.resultInfo->totalTimes)
        {
            break;
        }
    }

    //结束测试
    CommonEndTest(&tlcpCipherDemo);

    return tlcpCipherDemo.resultInfo;
}

Cipher_Demo_Base tlcpCipherDemo = {
        NULL,
        0,
        0,
        0,
        doTest,
        testName
};
