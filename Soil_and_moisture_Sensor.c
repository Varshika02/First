#include<LPC17xx.h>
#include <stdio.h>
#include "lcd.h"
#include "uart.h"
#define VREF       3.3 //Reference Voltage at VREFP pin, given VREFN = 0V(GND)
#define ADC_CLK_EN (1<<12)
#define SEL_AD0_3  (1<<3) //Select Channel AD0.3
#define SEL_AD0_2  (1<<2) //Select Channel AD0.2
#define CLKDIV     (3 << 8)      //ADC clock-divider (ADC_CLOCK=PCLK/CLKDIV+1) = 1Mhz @ 4Mhz PCLK
#define PWRUP      (1<<21) //setting it to 0 will power it down
#define START_CNV  (1<<24) //001 for starting the conversion immediately
#define ADC_DONE   (1U<<31) //define it as unsigned value or compiler will throw #61-D warning int m

#define  T_COEFF 100.0f

float moist()
{
	int result;
	float m;

	while((LPC_ADC->ADDR3 & ADC_DONE) == 0){} //Wait untill conversion is finished
		
	result = (LPC_ADC->ADDR3>>4) & 0xFFF; //12 bit Mask to extract result
		
	//volts = (result*VREF)/4096.0; //Convert result to Voltage
	m = (100.0 - ((result/4095.0)*100.0));

	return m;
}

float temp()
{
	int result;
	float volts, t;

	while((LPC_ADC->ADDR2 & ADC_DONE) == 0){} //Wait untill conversion is finished
		
	result = (LPC_ADC->ADDR2>>4) & 0xFFF; //12 bit Mask to extract result
		
	volts = (result*VREF)/4096.0; //Convert result to Voltage
	t = volts * T_COEFF;

	return t;
}

int main(void)
{
	int result = 0;

	float moisture = 0;
	char smoisture[20];

	float temperature_reading = 0;
  	char stemp[20];
	
	LPC_PINCON->PINSEL1 |= (0x01<<18) ; //select AD0.2 for P0.25
	LPC_PINCON->PINSEL1 |= (0x01<<20) ; //select AD0.3 for P0.26
	LPC_SC->PCONP |= ADC_CLK_EN; //Enable ADC clock
	LPC_ADC->ADCR =  PWRUP | CLKDIV;
	// LPC_ADC->ADCR |= SEL_AD0_2;	
	
	lcd_init();
	uart_config();
		
	while(1)
	{
		LPC_ADC->ADCR |= START_CNV; //Start new Conversion

		//lcd_cmd_write(0x80); // Cursor to beginning of 2nd row
		LPC_ADC->ADCR |= SEL_AD0_3;
		LPC_ADC->ADCR &= ~SEL_AD0_2;
		moisture = moist();

		sprintf(smoisture ,"Moisture = %0.2f", moisture);
		uart_str(smoisture);
		lcd_str_write(smoisture);
		lcd_cmd_write(0xC0); // Cursor to beginning of 2nd row
		uart_str("\r\n");
        
		LPC_ADC->ADCR |= SEL_AD0_2;
		LPC_ADC->ADCR &= ~SEL_AD0_3;
		temperature_reading = temp();
		sprintf(stemp,"Temp=%.2f 'C",temperature_reading);
		lcd_str_write(stemp);
		delay(500); //Slowing down Updates to 2 Updates per second*/
	    lcd_cmd_write(0x01); // Clear display screen
	}
	
	return 0; //This won't execute
}