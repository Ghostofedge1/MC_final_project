#define F_CPU 16000000UL
#define SET_BIT(REG,BIT) (REG |= (1<<BIT))
#define RST_BIT(REG,BIT) (REG &= ~(1<<BIT))
#define GET_BIT(REG,BIT) ((REG & (1<<BIT))>>BIT)
#define send_upper(REG,BIT) ( REG=(REG & 0xE8)|(((BIT>>4) & 0x07) | (((BIT>>4) & 0x08)<<1) ) )
#define send_lower(REG,BIT) ( REG=(REG & 0xE8)|((BIT & 0x07) | ((BIT & 0x08) << 1)) )


#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#define LCD PORTB
#define control_port PORTA
#define RS PA3
#define E PA2
#define RW 7
#define Heater PD0
#define AirConditioner PD1


unsigned int data;
void pulse();
void LCD_init();
void LCDcmd(unsigned char cmd);
void LCDdata(unsigned char data);
void LCDprint(const char *str);
void adc_init();
unsigned int adc_read(unsigned int channel);

int main(void)
{
	LCD_init();
	LCDprint("temp: ");
	RST_BIT(DDRA,0);
	RST_BIT(DDRA,1);
	SET_BIT(DDRB,PB0); // set lcd pins
	SET_BIT(DDRB,PB1);
	SET_BIT(DDRB,PB2);
	SET_BIT(DDRB,PB4);
	SET_BIT(DDRD,Heater);
	SET_BIT(DDRD,AirConditioner);

//	SET_BIT(DDRA,4);
//	SET_BIT(DDRA,5);
//	SET_BIT(DDRA,6);
	SET_BIT(DDRB,7);

	adc_init();

	unsigned int x ;
	 //int percentage;

	while (1)
	{
//
		data = adc_read(1);
		int temperature = (data * 5 / 1024.0) * 100.0;
		char ASCi[16];
		sprintf(ASCi, "%d", temperature);
		LCDprint(ASCi);

		_delay_ms(500);
		LCDcmd(0x06);	// move cursor to right
		LCDcmd(0x01);

		  x =adc_read(0);
			int percentage = (((5 *x)/ 1023.0)/5) * 100.0;
//			sprintf(ASCi, "%d", percentage);
//			LCDprint(ASCi);
//			_delay_ms(500);
//					LCDcmd(0x06);	// move cursor to right
//					LCDcmd(0x01);

 if( percentage>=99){
					RST_BIT(PORTA,4);
					RST_BIT(PORTA,5);
					RST_BIT(PORTA,6);
					RST_BIT(PORTB,7);
					   }
 else if(percentage<99 && percentage>=80){
						   SET_BIT(PORTB,7);
						   RST_BIT(PORTA,4);
						   RST_BIT(PORTA,5);
						   RST_BIT(PORTA,6);
					   }
 else if (percentage < 80 && percentage >=60){
						   SET_BIT(PORTB,7);
						   SET_BIT(PORTA,6);
						   RST_BIT(PORTA,4);
						   RST_BIT(PORTA,5);
					   }
   else if(percentage < 60 && percentage>=40){
						   SET_BIT(PORTB,7);
						   SET_BIT(PORTA,6);
						   SET_BIT(PORTA,5);
						   RST_BIT(PORTA,4);
					   }
  else if(percentage<40){
						   SET_BIT(PORTB,7);
						   SET_BIT(PORTA,6);
						   SET_BIT(PORTA,5);
						   SET_BIT(PORTA,4);
					   }

	 if (temperature <=22 && temperature >=-15)
		{
			SET_BIT(PORTD, Heater);
			RST_BIT(PORTD,AirConditioner);
		}
		else if (temperature >= 22 && temperature <28)
		{
			RST_BIT(PORTD, Heater);
			RST_BIT(PORTD,AirConditioner);

		}
		else if (temperature >= 29 && temperature <50)
		{
			RST_BIT(PORTD, Heater);
			SET_BIT(PORTD,AirConditioner);
		}
		else if (temperature >= 51)
			{
				RST_BIT(PORTD, Heater);
				RST_BIT(PORTD,AirConditioner);
			}


	}
}

void pulse()
{
	SET_BIT(control_port,E);
	_delay_ms(1);
	RST_BIT(control_port,E);
}

void LCD_init()
{
	DDRA = 0xFF;
	RST_BIT(control_port,E);
	_delay_ms(15);
	LCDcmd(0x33);
	LCDcmd(0x32);
	LCDcmd(0x28);	// 4 BIT Mode 2 lines
	LCDcmd(0x0E);   // start
	LCDcmd(0x06);	// move cursor to right
	LCDcmd(0x01);	// clear dis
	_delay_ms(200);
}

void LCDcmd(unsigned char cmd)
{
	RST_BIT(control_port,RS);
	RST_BIT(control_port,RW);
	send_upper(LCD,cmd);
	pulse();
	send_lower(LCD,cmd);
	pulse();
	_delay_ms(5);
}


void LCDdata(unsigned char data)
{
	SET_BIT(control_port,RS);
	RST_BIT(control_port,RW);
	send_upper(LCD,data);
	pulse();
	send_lower(LCD,data);
	pulse();
	_delay_ms(20);
}


void LCDprint(const char *str)
{
	while (*str)
	{
		LCDdata(*str++);
		_delay_ms(10);
	}
}

void adc_init(){
	//choosing the second mode (VCC refrence)
	ADMUX|=(1<<REFS0);
	// ADMUX|=(1<<ADLAR);
	//ADC enable and prescale it to 128
	ADCSRA|=(1<<ADEN)|(1<<ADPS1)|(1<<ADPS0)| (1<<ADPS2);
}

unsigned int adc_read(unsigned int channel){
	ADMUX|=channel;
	SET_BIT(ADCSRA,ADSC);
	while((ADCSRA&(1<<ADIF))==0);
	SET_BIT(ADCSRA,ADIF);
	ADMUX&= ~channel;
	return ADC;

}
