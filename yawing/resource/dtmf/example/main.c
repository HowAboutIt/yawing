
#include <ioavr.h>


//
// 说明
//
// 这个小程序是测试 DTMF 解码器工作的。
// 测试平台: ATMega8L, 3.3V供电, 8M晶振外接, IAR
// ADC0 管脚直流偏压至 Vcc/2 (通过两个相等的电阻对 Vcc 分压)
// DTMF 信号通过 1 个电容耦合进ADC0 管脚
// 测试方法：首先按照上述所说接好硬件
//           外部产生 DTMF 信号输入进来
//           运行本程序
//           从串口接收 DTMF 解码结果
//
// author: hendry
// date: 2008/12
//



//
// 自定义类型
//
typedef		unsigned char		bool;
#define		true				1
#define		false				0


//
// A/D 采样缓存
//
#define			FIFO_LEN				8

unsigned char	g_fifo_p0 = 0, g_fifo_p1 = 0;
unsigned char	g_fifo[FIFO_LEN];

unsigned char get_fifo_len()
{
	if (g_fifo_p1 >= g_fifo_p0)
	{
		return (g_fifo_p1 - g_fifo_p0);
	}
	else
	{
		return (g_fifo_p1 + FIFO_LEN - g_fifo_p0);
	}
}

bool is_fifo_empty()
{
	return (get_fifo_len() == 0);
}

bool is_fifo_full()
{
	return (get_fifo_len() == (FIFO_LEN - 1));
}

unsigned char read_fifo()
{
	unsigned char	ch;
	
	ch = g_fifo[g_fifo_p0 ++];
	
	if (g_fifo_p0 == FIFO_LEN)
	{
		g_fifo_p0 = 0;
	}
	
	return ch;
}

void write_fifo(unsigned char ch)
{
	g_fifo[g_fifo_p1 ++] = ch;
	
	if (g_fifo_p1 == FIFO_LEN)
	{
		g_fifo_p1 = 0;
	}
}





//
// 初始化串口
// 使用 8MHz 晶振，115200bps
//
void init_uart()
{
	UBRRL = 8;					
	UCSRA = (1<<U2X);
	UCSRB = (1<<RXCIE) | (1<<TXEN);
	UCSRC = (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0);
}

//
// 串口发送
//
void write_uart(char ch)
{
	while (!(UCSRA & (1<<UDRE)))
	{}
	
	UDR = ch;
}


//
// 初始化 A/D 和 timer
//
void init_ad()
{
	ADMUX = (1<<REFS0);
	ADCSR = (1<<ADEN) | (1<<ADSC) | (1<<ADFR) | ( 1<<ADPS1) | (1<<ADPS0);
	TCCR1B = (1<<WGM12)| (1<<CS11);
	OCR1A = 249;
	TIMSK |= (1 << OCIE1A);	
}


//
// timer 中断
//
#pragma vector = TIMER1_COMPA_vect
__interrupt void on_timer1_compA()
{
	unsigned char	low, high;
	unsigned short	adc_value;
	
	//
	// 读取 A/D 采样值
	//
	low = ADCL;
	high = ADCH;
	adc_value = (low + (high << 8));
	
	//
	// 限幅
	//
	if (adc_value < 0x180)
	{
		adc_value = 0;
	}
	else if (adc_value > 0x27f)
	{
		adc_value = 0xff;
	}
	else
	{
		adc_value = adc_value - 0x180;
	}

	//
	// 写入 fifo
	//
	if (!is_fifo_full())
	{
		write_fifo((unsigned char)adc_value);
	}	
}




void main( void )
{
	signed char	ch;
	
	//
	// DTMF 解码程序声明
	//
	extern void init_dtmf_decoder();
	extern unsigned char decode_dtmf(signed char sample);

	//
	// 初始化硬件
	//
	init_uart();
	init_ad();
	asm("SEI");
	
	
	//
	// 初始化解码器
	//
	init_dtmf_decoder();
		
	
	while (1)
	{
		if (!is_fifo_empty())
		{
			// 读取 fifo
			ch = read_fifo();
			
			// 转成有符号数
			ch += -128;
			
			// 送入解码器
			ch = decode_dtmf(ch);
			
			// 输出解码结果
			if (ch != 0)
			{
				write_uart(ch);
			}
		}
	}
}


