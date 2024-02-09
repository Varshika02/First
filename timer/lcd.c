#include "lcd.h"

void lcd_cmd_write(char cmd)
{
    LPC_GPIO0 -> FIOCLR = LCD_DATA_PINS;
    LPC_GPIO0 -> FIOSET = cmd << 15;
    LPC_GPIO0 -> FIOCLR = LCD_RS_PIN(10);
    LPC_GPIO0 -> FIOSET = LCD_EN_PIN(11);
    delay(10);
    LPC_GPIO0 -> FIOCLR = LCD_EN_PIN(11);
    return;
}

void lcd_data_write(char dat)
{
    LPC_GPIO0 -> FIOCLR = LCD_DATA_PINS;
    LPC_GPIO0 -> FIOSET = dat << 15;
    LPC_GPIO0 -> FIOSET = LCD_RS_PIN(10);
    LPC_GPIO0 -> FIOSET = LCD_EN_PIN(11);
    delay(10);
    LPC_GPIO0 -> FIOCLR = LCD_EN_PIN(11);
    return;
}

void lcd_str_write(char *str)
{
 while(*str != '\0')
 {
  lcd_data_write(*str);
  str++;
 }
return;
}

void lcd_init(void)
{
    LPC_GPIO0 -> FIODIR |= LCD_DATA_PINS;
    LPC_GPIO0 -> FIODIR |= LCD_RS_PIN(10);
    LPC_GPIO0 -> FIODIR |= LCD_EN_PIN(11);
    lcd_cmd_write(0x38);
    lcd_cmd_write(0x0E);
    lcd_cmd_write(0x01);
    return;
}

void delay(uint32_t ms)
{
uint32_t i,j,k;
for(i = 0 ; i < ms ; i++)
{
k = 0 ;
for(j = 0 ; j < 3000 ; j++)
{
 k++ ;
}

}
 return ; 
}
