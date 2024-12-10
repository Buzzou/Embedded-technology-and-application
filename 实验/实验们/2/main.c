#include "config.h"
#define SCK (0x01<<4) //P0.4
#define SI (0x01<<6) //P0.6
#define RCK (0x01<<7) //P0.7

void DelayNS(uint32 dly)
{
	uint32 i;
	for(;dly>0;dly--)
		for(i=0;i<5000;i++);

}
// GPIO output
void GPIO_Init(void)
/*括号内写void表示明确不接受任何参数*/
{
	//配置 P0.4, P0.6, P0.7 为 GPIO 功能（通过清零 PINSEL0 的相应位）
	//初始化 GPIO 引脚，将 P0.4, P0.6, P0.7 分别作为时钟信号、串行输入信号和锁存信号
	PINSEL0 &= ~(0x03<<8);	// 右数8,9位为0 1100000000 [00]11111111
	PINSEL0 &= ~(0x03<<12);
	PINSEL0 &= ~(0x03<<14);
	//3个引脚输出 - 设置 P0.4, P0.6, P0.7 为输出模式（通过设置 IO0DIR 的相应位）
	IO0DIR |= SCK;
	IO0DIR |= SI;
	IO0DIR |= RCK;
	
}

void SendByte(uint16 data)
/*通过控制信号的时序，将 16 位数据逐位发送到目标设备*/
{
	uint8 i = 0;
	//清零 RCK，准备发送数据
	IO0CLR |= RCK;
	//循环发送 data 的每一位（从高位到低位），控制 SCK（时钟信号）和 SI（数据输入信号）
	//每发送一位，时钟信号 SCK 会产生一个上升沿
	//数据发送完成后，设置 RCK，完成锁存
	for(i = 0; i < 16; ++i)
	{
		IO0CLR |= SCK;
		if(0 == (data & 0x8000))	//发最高位
		{
			//如果最高位是 0，则清零 SI（输出低电平）
			IO0CLR |= SI;
		}
		else
		{
			//如果最高位是 1，则置位 SI（输出高电平）
			IO0SET |= SI;
		}
			
		IO0SET |= SCK;	//发完1位，移位
		
		data = (data << 1);	//发送下一位
	}
	
	IO0SET |= RCK;
}

int main()
/*以交替的方式发送不同的数据，实现某种设备的交替控制（如灯光的闪烁）*/
{
	GPIO_Init();
	//程序初始化 GPIO 后进入无限循环
	while(1)
	{
		//依次发送两个不同的数据，使用延时函数控制状态切换的速度
		SendByte(0xAAAA); //1010101010101010 - 1不亮 - 控制设备亮灭的交替状态
		DelayNS(1000);
		SendByte(0x5555);//0101010101010101 - 另一种交替状态
		DelayNS(1000);
	}
	
	return 0;
}
