
#include <ioavr.h>


//
// ˵��
//
// ���С�����ǲ��� DTMF �����������ġ�
// ����ƽ̨: ATMega8L, 3.3V����, 8M�������, IAR
// ADC0 �ܽ�ֱ��ƫѹ�� Vcc/2 (ͨ��������ȵĵ���� Vcc ��ѹ)
// DTMF �ź�ͨ�� 1 ��������Ͻ�ADC0 �ܽ�
// ���Է��������Ȱ���������˵�Ӻ�Ӳ��
//           �ⲿ���� DTMF �ź��������
//           ���б�����
//           �Ӵ��ڽ��� DTMF ������
//
// author: hendry
// date: 2008/12
//



//
// �Զ�������
//
typedef		unsigned char		bool;
#define		true				1
#define		false				0


//
// A/D ��������
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
// ��ʼ������
// ʹ�� 8MHz ����115200bps
//
void init_uart()
{
	UBRRL = 8;					
	UCSRA = (1<<U2X);
	UCSRB = (1<<RXCIE) | (1<<TXEN);
	UCSRC = (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0);
}

//
// ���ڷ���
//
void write_uart(char ch)
{
	while (!(UCSRA & (1<<UDRE)))
	{}
	
	UDR = ch;
}


//
// ��ʼ�� A/D �� timer
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
// timer �ж�
//
#pragma vector = TIMER1_COMPA_vect
__interrupt void on_timer1_compA()
{
	unsigned char	low, high;
	unsigned short	adc_value;
	
	//
	// ��ȡ A/D ����ֵ
	//
	low = ADCL;
	high = ADCH;
	adc_value = (low + (high << 8));
	
	//
	// �޷�
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
	// д�� fifo
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
	// DTMF �����������
	//
	extern void init_dtmf_decoder();
	extern unsigned char decode_dtmf(signed char sample);

	//
	// ��ʼ��Ӳ��
	//
	init_uart();
	init_ad();
	asm("SEI");
	
	
	//
	// ��ʼ��������
	//
	init_dtmf_decoder();
		
	
	while (1)
	{
		if (!is_fifo_empty())
		{
			// ��ȡ fifo
			ch = read_fifo();
			
			// ת���з�����
			ch += -128;
			
			// ���������
			ch = decode_dtmf(ch);
			
			// ���������
			if (ch != 0)
			{
				write_uart(ch);
			}
		}
	}
}


