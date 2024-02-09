									  //driver code for 16 X 2 LCD for 8-bit 2-line display mode operation
//LCD pin connections
// LCD          - LPC1768
//------------------------
// RS           -  P0.10
// EN           -  P0.11
// DATA [D7-D0] -  [P0.22 - P0.15] 

#ifndef LCD_H
#define LCD_H
#include <LPC17xx.h>

#include <stdint.h>

#define LCD_DATA_PINS (0xFF << 15)
#define LCD_RS_PIN(X) (1 << X)
#define LCD_EN_PIN(X) (1 << X)

void lcd_init(void);
void lcd_cmd_write(char);
void lcd_data_write(char);
void lcd_str_write(char *);
void delay(uint32_t ); 
#endif

