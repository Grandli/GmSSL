//
// Created by Grand
//

#include "cipher_test_report.h"

//打印测试信息
int ReportPrintTestResult(TestResultInfo *resultInfo)
{
    if(resultInfo==NULL)
    {
        ComPrintf("resultInfo is NULL\n");
        return 1;
    }
    ComPrintf("%s  testResult: costTime = %ld ms totalTimes = %d, succeedTimes = %d\n",
              resultInfo->testName, resultInfo->costTime, resultInfo->totalTimes, resultInfo->successTimes);
    if(resultInfo->totalTimes != resultInfo->successTimes)
    {
        return 0;
    }
    return 1;
}