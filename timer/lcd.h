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
void delay(uint32_t); 
#endif

