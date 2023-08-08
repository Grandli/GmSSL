#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <malloc.h>
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
    long costTime = getCurrentTime()-currentTime;
    getPrintSize(amountStr, haveTestAmount);
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

//把字符串数据转为十六进制的字符串
char *ComStrToHex(unsigned char *inData, int inLen, bool bUpper)
{
    int outLen = inLen * 2 +2;
    int i = 0;
    char *outData = (char*) malloc(outLen);
    if (!outData)
        return "";
    memset(outData, 0, outLen);

    for(i=0; i<inLen; i++){
        sprintf(outData+i*2, bUpper?"%02X":"%02x", inData[i]);
    }
    return outData;
}
//把十六进制的字符串转为字符串数据
int ComHexToStr(const char *inData, unsigned char *outBuf, int inLen)
{
    unsigned char high, low;
    int idx, retLen=0;
    for (idx=0; idx<inLen; idx+=2)
    {
        high = inData[idx];
        low = inData[idx+1];

        if(high>='0' && high<='9')
            high = high-'0';
        else if(high>='A' && high<='F')
            high = high - 'A' + 10;
        else if(high>='a' && high<='f')
            high = high - 'a' + 10;
        else
            return -1;

        if(low>='0' && low<='9')
            low = low-'0';
        else if(low>='A' && low<='F')
            low = low - 'A' + 10;
        else if(low>='a' && low<='f')
            low = low - 'a' + 10;
        else
            return -1;

        outBuf[retLen++] = high<<4 | low;
    }
    return retLen;
}

//按16进制打印内存数据
void dumpMem(unsigned char *data, int len)
{
    int i;
    //数据为空返回
    if(data==NULL)
    {
        return;
    }
    printf("----begin dump len: %d ---\n", len);
    for(i=0; i<len; i++)
        printf("0x%02x,",(unsigned char)(data[i]));
    printf("\n----dump end---\n");
}

//把数据写入到文件（新建文件，或者覆盖文件）
int ComWriteFile(char *filename, unsigned char *data, unsigned int dataLen)
{
    int ret = 1;
    FILE *fp = fopen(filename, "wb");
    if(!fp) {
        return -1;
    }
    ret = fwrite(data, 1, dataLen, fp);
    if(ret != dataLen) {
        return -2;
    }
    fclose(fp);
    return 1;
}

//从文件读取所有数据（dataLen需要传data的长度，以保证能正常的读取）
int ComReadFile(char *filename, unsigned char *data, unsigned int *dataLen)
{
    int ret = 1;
    int fLen = 0;
    FILE *fp = fopen(filename, "rb");
    if(!fp) {
        return -1;
    }
    //先将指针偏移到文件尾
    fseek(fp, 0, SEEK_END);
    fLen = ftell(fp);
    if(fLen>*dataLen) {
        *dataLen = fLen;
        fclose(fp);
        return -2;
    }
    //再将指针偏移到文件头
    fseek(fp, 0, SEEK_SET);
    //读取文件
    ret = fread(data, 1, fLen, fp);
    if(ret != fLen) {
        fclose(fp);
        return -3;
    }
    *dataLen = fLen;

    fclose(fp);
    return 1;
}