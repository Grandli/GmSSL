#include "cipher_demo_factory.h"
#include "key/key_cipher_demo.h"
#include "sm3/sm3_cipher_demo.h"
#include "sm9/sm9_cipher_demo.h"
#include "sm4/sm4_cipher_demo.h"
#include "hmac/hmac_cipher_demo.h"
#include "common/Utils.h"

//创建相应的密码运算实例
Cipher_Demo_Base *FactoryCreateCipherDemo(int type) {
    Cipher_Demo_Base *cipherDemo = NULL;
    switch(type)
    {
        case KEY_DEMO:
            cipherDemo = &keyCipherDemo;
            break;
            break;
        case SM4_DEMO:
            cipherDemo = &sm4CipherDemo;
            break;
        case SM9_DEMO:
            cipherDemo = &sm9CipherDemo;
            break;
        case SM3_DEMO:
            cipherDemo = &sm3CipherDemo;
            break;
        case HMAC_DEMO:
            cipherDemo = &hmacCipherDemo;
            break;
    }

    //如果创建成功
    if(cipherDemo==NULL)
    {
        ComPrintf("type = %d is not supported\n", type);
    }
    else
    {
        //对相应的密码测试实例进行初始化
        CommonDemoInit(cipherDemo);
        ComPrintf("%s demo test create ok\n", cipherDemo->testName());
    }
    return cipherDemo;
}