#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
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
        sprintf(retStr, "%dB", (int)size);
        return;
    }else{
        size /= 1024;
    }

    if(size < 1024){
        sprintf(retStr, "%dKB", (int)size);
        return;
    }else{
        rest = size % 1024;
        size /= 1024;
    }

    if(size < 1024){
        size = size * 100;
        sprintf(retStr, "%d.%dMB", (unsigned int)(size / 100), (int)(rest * 100 / 1024 % 100));
        return;
    }else{
        size = size * 100 / 1024;
        sprintf(retStr, "%d.%dGB", (unsigned int)(size / 100), (int)(size % 100));
        return;
    }
}

//进行用例测试
void demoDoUtilTest(TestFunc testFunc, unsigned int testTime, const char *testName)
{
    unsigned long long haveTestAmount = 0;
    unsigned int tmpAmount = 0;
    unsigned int testTimes = 0;
    char amountStr[100]="";
    unsigned long needTestTime = testTime*1000;
    unsigned long currentTime = getCurrentTime();


    while(1){
        if(getCurrentTime()>=currentTime+needTestTime)
            break;
        tmpAmount = testFunc();
        haveTestAmount += tmpAmount;
        testTimes++;
        //printf("current time = %d, cost_time= %ld \n",testTimes, getCurrentTime()-currentTime);
    }
    getPrintSize(amountStr, haveTestAmount);
    long costTime = getCurrentTime()-currentTime;
    printf("testName = %s, haveCostTime = %ld ms, one CostTime = %f ms, testTimes = %d,  testAmount = %s\n", testName, costTime, (float)costTime/testTimes, testTimes, amountStr);
}

static struct timespec begin_ts[MAX_TIME_COUNT_NUM];
//开始时间计时
void beginCommonTime(int index)
{
    clock_gettime(CLOCK_MONOTONIC,&begin_ts[index]);//记录函数开始时间
}

//结束时间计时
float endCommonTime(int index)
{
    struct timespec end_ts;
    float time_s,time_ns,time_ms;
    clock_gettime(CLOCK_MONOTONIC,&end_ts);//记录函数结束时间
    time_s = end_ts.tv_sec - begin_ts[index].tv_sec;  //得到秒的时间
    time_ns = (end_ts.tv_nsec - begin_ts[index].tv_nsec); //得到纳秒的时间
    //由于总的时间=time_s+time_ns
    //为了显示方便，将总的时间统一转化为毫秒
    time_ms = time_s*1000+time_ns/1000000;
    return time_ms;
}