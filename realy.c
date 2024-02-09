#include <LPC17xx.h>
#include <stdio.h>
#include <stdint.h>

#define relay_pin (0x01<<28)

/*uint8_t soil_moisture_reading()
{
	
}*/

/*uint8_t temerature_reading()
{

}*/
unit8_t range();
int main()
{
	LPC_GPIO1->FIODIR |= relay_pin;

	while(1)
	{
		unit8_t temp = temperature_reading();
		uint8_t soil_moisture =  soil_moisture_reading();
		if (temp < temperature_reading  && soil_moisture < range )
		{
			LPC_GPIO1->FIOSET |= relay_pin;
			lcd_str_write("RELAY ON");
			uart_str("RELAY ON");
  			uart_str("\r\n");
		}

		else if (temp > temperature_reading  && soil_moisture < range )
		{
			LPC_GPIO1->FIOSET |= relay_pin;
			lcd_str_write("RELAY ON");
			uart_str("RELAY ON");
  			uart_str("\r\n");
			for ( int i =0; i < 10000000; i++)
		}

		else if(temp < temperature_reading  && soil_moisture > range )
		{
			LPC_GPIO1->FIOCLR |= relay_pin;
			lcd_str_write("RELAY ON");
			uart_str("RELAY ON");
  			uart_str("\r\n");
		}

		else 
		{
			LPC_GPIO1->FIOCLR |= relay_pin;
			lcd_str_write("RELAY OFF");
			uart_str("RELAY OFF");
  			uart_str("\r\n");
		}
	}
}

