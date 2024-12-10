//
// Created by Ruohao L. on 03/12/2024.
//
#include "config.h"
#define BEEPCON 0x1000000 //蜂鸣器的控制位F0.24（第 25 位（从右往左，第 0 位开始计数）），低电平蜂鸣

void DelayNS(uint32 dly)
/*实现简单的延时，通过嵌套循环消耗 CPU 时间实现延时 - 模拟一个毫秒级延时函数
 * dly 参数：控制延时的时间，循环的次数越多，延时越长
 */
//这种方式的延时不是精确的，它的实际时间与系统频率 (Fcclk) 密切相关 - 可使用定时器或芯片的内部延时函数
{
    uint32 i;
    for (;dly>0;dly--)
        for (i=0;i<5000;++i);
}

void Beep(uint32 duration)
{
    //IO0SET 和 IO0CLR 是 GPIO 控制寄存器，用于设置某些 GPIO 引脚输出高电平或低电平
    IO0SET |= BEEPCON;//将其第 25 位设为 1，输出高电平
    DelayNS(duration);
    IO0CLR |= BEEPCON;//将其第 25 位设为 1，输出低电平
    DelayNS(duration);
}

int main()
{
    uint32 i;
    //PINSEL1 是寄存器，用于配置引脚的功能
    PINSEL1 &= ~(1 << 16); //16位为0
    PINSEL1 &= ~(1 << 17); //17位为0

    //IO0DIR 是寄存器，用于配置 GPIO 的方向（输入或输出）
    IO0DIR |= BEEPCON; //25位设为1，表示输出模式；其他位不变

    while (1)
    {
        Beep(10);
    }
    return 0;
}
