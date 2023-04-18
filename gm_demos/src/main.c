#include "cipher_demo.h"
#include "cipher_demo_factory.h"
#include "cipher_test_report.h"


int main() {

    int i;

    //创建测试demo类型
    Cipher_Demo_Base *cipherDemoList[MAX_DEMO_NUM] = {NULL};
    //需要测试的次数
    int cipher_test_times[MAX_DEMO_NUM] = {0};

    //初始化测试demo
    CipherDemoInit();


    //创建测试实例
    for(i=0;i<MAX_DEMO_NUM;i++)
    {
        cipher_test_times[i] = 1;//需要测试的次数
        cipherDemoList[i] = FactoryCreateCipherDemo(i);
    }

    ComPrintf("begin test \n");

    //测试并打印结果
    for(i=0;i<MAX_DEMO_NUM;i++)
    {
        if(cipherDemoList[i])
        {
            ComPrintf("do test  %s\n", cipherDemoList[i]->testName());
            cipherDemoList[i]->doTest(cipher_test_times[i]);
        }
    }

    int testOkCount = 0;
    //统计正确返回的个数及打印结果
    for(i=0;i<MAX_DEMO_NUM;i++)
    {
        if(cipherDemoList[i])
        {
            if(ReportPrintTestResult(cipherDemoList[i]->resultInfo)==1)
            {
                testOkCount++;
            }
        }
        else
        {
            testOkCount++;
        }
    }

    //销毁信息
    for(i=0;i<MAX_DEMO_NUM;i++)
    {
        if(cipherDemoList[i])
        {
            CommonDemoExit(cipherDemoList[i]);
        }
    }

    //检查测试是否都通过
    if(testOkCount==MAX_DEMO_NUM)
    {
        ComPrintf("all testSuits  passed\n");
    }
    else
    {
        ComPrintf("%d testSuits not passed\n", MAX_DEMO_NUM-testOkCount);
    }
    ComPrintf("test over\n");

    //销毁密码组合服务相关对象
    DestroyCipherDemo();

    return 0;

}
