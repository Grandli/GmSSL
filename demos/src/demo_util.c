#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include "demo_util.h"

//获取当前时间 单位：毫秒
long getCurrentTime() {
#ifdef WIN32
    return clock();
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    long total_time = tv.tv_sec*1000+tv.tv_usec/1000;
    return total_time;
#endif
}

//获取打印的长度
void getPrintSize(char *retStr, unsigned long long size){
    unsigned long long rest = 0;
    if(size < 1024){
        sprintf(retStr, "%dB", size);
        return;
    }else{
        size /= 1024;
    }

    if(size < 1024){
        sprintf(retStr, "%dKB", size);
        return;
    }else{
        rest = size % 1024;
        size /= 1024;
    }

    if(size < 1024){
        size = size * 100;
        sprintf(retStr, "%d.%dMB", (size / 100), (rest * 100 / 1024 % 100));
        return;
    }else{
        size = size * 100 / 1024;
        sprintf(retStr, "%d.%dGB", (size / 100), (size % 100));
        return;
    }
}

void demoDoUtilTest(TestFunc testFunc, unsigned int testTime, const char *testName)
{
    unsigned long long haveTestAmount = 0;
    unsigned int tmpAmount = 0;
    unsigned int testTimes = 0;
    char amountStr[100]="";

    unsigned long currentTime = getCurrentTime();

    while(1){
        if(getCurrentTime()>=currentTime+testTime*1000)
            break;
        tmpAmount = testFunc();
        haveTestAmount += tmpAmount;
        testTimes++;
    }
    getPrintSize(amountStr, haveTestAmount);
    long costTime = getCurrentTime()-currentTime;
    printf("testName = %s, haveCostTime = %ld ms, one CostTime = %d ms, testTimes = %d,  testAmount = %s\n", testName, costTime, costTime/testTimes, testTimes, amountStr);
}