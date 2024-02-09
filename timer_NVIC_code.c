#include <LPC17xx.h>
#include <stdint.h>
#include <stdio.h>
#include "uart.h"
#include "lcd.h"

#define VREF       3.3 //Reference Voltage at VREFP pin, given VREFN = 0V(GND)
#define ADC_CLK_EN (1<<12)
#define SEL_AD0_3  (1<<3) //Select Channel AD0.3
#define SEL_AD0_2  (1<<2) //Select Channel AD0.2
#define SEL_AD0_1  (1<<1) //Select Channel AD0.1
#define CLKDIV     (3 << 8)      //ADC clock-divider (ADC_CLOCK=PCLK/CLKDIV+1) = 1Mhz @ 4Mhz PCLK
#define PWRUP      (1<<21) //setting it to 0 will power it down
#define START_CNV  (1<<24) //001 for starting the conversion immediately
#define ADC_DONE   (1U<<31) //define it as unsigned value or compiler will throw #61-D warning int m

#define  T_COEFF 100.0f
						 
void int_config(void);
void timer_config(void);		 

float moist(void);
float temp(void);
float water_level(void);
void sensors(void);


int main() 
{
    LPC_GPIO1->FIODIR |= (0xFF << 19);    // Configure P1.19 as output
	LPC_GPIO1->FIOCLR |= (0xFF << 26);    // Configure P1.19 as output

    LPC_PINCON->PINSEL1 |= (0x01<<16) ; //select AD0.1 for P0.24
	LPC_PINCON->PINSEL1 |= (0x01<<18) ; //select AD0.2 for P0.25
	LPC_PINCON->PINSEL1 |= (0x01<<20) ; //select AD0.3 for P0.26
	LPC_SC->PCONP |= ADC_CLK_EN; //Enable ADC clock
	LPC_ADC->ADCR =  PWRUP | CLKDIV;

    uart_config();                     // Initialize UART configuration
    timer_config();                    // Initialize Timer configuration
    int_config();                      // Initialize interrupt configuration
    lcd_init();

    lcd_cmd_write(0x01);

    while (1) 
	{
        lcd_cmd_write(0x80);
		lcd_str_write("waiting");
        LPC_GPIO1->FIOSET |= (0x0f << 19);    // Set P1.19 high
        delay(100);
        LPC_GPIO1->FIOCLR |= (0x0f << 19);    // Clear P1.19 low
        delay(100);
    }
}

void timer_config(void) 
{
    LPC_SC->PCONP |= (1 << 1);    // Power up Timer0

    LPC_TIM0->PR = 14;            // Set the prescaler value
    LPC_TIM0->MR0 = 2000000;      // Set match register for 10 seconds delay
    LPC_TIM0->MCR = (1 << 0) | (1 << 1);  // Interrupt and Reset on Match0
    LPC_TIM0->TCR = 1 << 1;        // Reset Timer0
    LPC_TIM0->TCR = 1;              // Enable Timer0
	uart_str("Timer Config Done\r\n");
}

void int_config(void) 
{
    NVIC_ClearPendingIRQ(TIMER0_IRQn);     // Clear pending interrupts for Timer0
    NVIC_SetPriority(TIMER0_IRQn, 1);      // Set priority for Timer0 interrupt
    NVIC_EnableIRQ(TIMER0_IRQn);           // Enable Timer0 interrupt 
	uart_str("Interrupt Config Done\r\n");  
}

float moist(void)
{
	int result;
	float m;

	while((LPC_ADC->ADDR3 & ADC_DONE) == 0){} //Wait untill conversion is finished
		
	result = (LPC_ADC->ADDR3>>4) & 0xFFF; //12 bit Mask to extract result
		
	//volts = (result*VREF)/4096.0; //Convert result to Voltage
	m = (100.0 - ((result/4095.0)*100.0));

	return m;
}

float temp(void)
{
	int result;
	float volts, t;

	while((LPC_ADC->ADDR2 & ADC_DONE) == 0){} //Wait untill conversion is finished
		
	result = (LPC_ADC->ADDR2>>4) & 0xFFF; //12 bit Mask to extract result
		
	volts = (result*VREF)/4095.0; //Convert result to Voltage
	t = volts * T_COEFF;

	return t;
}

float water_level(void)
{
	int result;
	float percent;

	while((LPC_ADC->ADDR1 & ADC_DONE) == 0){} //Wait untill conversion is finished
		
	result = (LPC_ADC->ADDR1>>4) & 0xFFF; //12 bit Mask to extract result
		
	percent = (result*100)/4095.0; //Convert result to Voltage

	return percent;
}

void sensors(void)
{
	float moisture = 0;
	char smoisture[20];

	float temperature_reading = 0;
  	char stemp[20];

	float percentage = 0;
	char spercent[20];

	float temp_limit = 25.0;
    float moist_limit = 15.0;
		
	LPC_ADC->ADCR |= START_CNV; //Start new Conversion

    lcd_cmd_write(0x80); // Cursor to beginning of 2nd row
    LPC_ADC->ADCR |= SEL_AD0_3;
    LPC_ADC->ADCR &= ~SEL_AD0_2;
    LPC_ADC->ADCR &= ~SEL_AD0_1;
    moisture = moist();

    sprintf(smoisture ,"Moisture = %0.2f", moisture);
    uart_str(smoisture);
    lcd_str_write(smoisture);
    lcd_cmd_write(0xC0); // Cursor to beginning of 2nd row
    uart_str("\r\n");
    
    LPC_ADC->ADCR |= SEL_AD0_2;
    LPC_ADC->ADCR &= ~SEL_AD0_1;
    LPC_ADC->ADCR &= ~SEL_AD0_3;
    temperature_reading = temp();
    sprintf(stemp,"Temp = %.2f 'C",temperature_reading);
    lcd_str_write(stemp);
    uart_str(stemp);
    delay(500); //Slowing down Updates to 2 Updates per second*/

    LPC_ADC->ADCR |= SEL_AD0_1;
    LPC_ADC->ADCR &= ~SEL_AD0_2;
    LPC_ADC->ADCR &= ~SEL_AD0_3;
    lcd_cmd_write(0x01);
    lcd_cmd_write(0x80);
    percentage = water_level();
    sprintf(spercent,"Water lvl = %0.2f", percentage);
	uart_str("\n");
    uart_str(spercent);
    lcd_str_write(spercent);
    //delay(10);
    //lcd_cmd_write(0x01); // Clear display screen

    if(temp_limit < temperature_reading && moist_limit > moisture)
    {
        uart_str("RELAY ON in if");
        uart_str("\r\n");
        delay(5000);		   //5 sec relay ON
    }
    else
    {
        uart_str("RELAY OFF in else");
        uart_str("\r\n");
        delay(5000);		   //5 sec relay ON
    }
    
    // if (temp_limit < temperature_reading && moisture < moist_limit)
    // {
    //     LPC_GPIO1->FIOSET |= relay_pin;
    //     lcd_str_write("RELAY ON");
    //     uart_str("RELAY ON");
    //     uart_str("\r\n");
    //     delay(5000);		   //5 sec relay ON
    // }

    // else if (temp_limit > temperature_reading && soil_moisture < range )
    // {
    //     LPC_GPIO1->FIOSET |= relay_pin;
    //     lcd_str_write("RELAY ON");
    //     uart_str("RELAY ON");
    //     uart_str("\r\n");
    //     delay(10000);		   //10 sec relay ON
    // }

    // else if(temp < temperature_reading  && soil_moisture > range )
    // {
    //     LPC_GPIO1->FIOCLR |= relay_pin;
    //     lcd_str_write("RELAY OFF");
    //     uart_str("RELAY OFF");
    //     uart_str("\r\n");
    //     delay(100);
    // }

    // else 
    // {
    //     LPC_GPIO1->FIOCLR |= relay_pin;
    //     lcd_str_write("RELAY OFF");
    //     uart_str("RELAY OFF");
    //     uart_str("\r\n");
    //     delay(100);
    // }
    timer_config();        // Initialize Timer configuration
}

void TIMER0_IRQHandler (void) 
{
    uart_str("\n\nInt from Timer\r\n");       // Send a message through UART
    lcd_cmd_write(0x01);
    lcd_cmd_write(0x80);
    lcd_str_write("\nINT from Timer");
	
	//delay(100);
	//lcd_cmd_write(0x01);
    
	//Reading and displaying all the sensors
    	
	sensors();
	LPC_GPIO1->FIOPIN ^= (1 << 26);    // Set P1.26 high
    delay(100);
    LPC_TIM0->IR = 1 << 0;                 // Clear interrupt flag
    NVIC_ClearPendingIRQ(TIMER0_IRQn);     // Clear pending interrupt for Timer0
}
