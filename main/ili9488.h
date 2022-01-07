#include <stdio.h>

#define LCD_DATA_BIT0   2
#define LCD_DATA_BIT1   4
#define LCD_DATA_BIT2   16
#define LCD_DATA_BIT3   17
#define LCD_DATA_BIT4   5
#define LCD_DATA_BIT5   18
#define LCD_DATA_BIT6   19
#define LCD_DATA_BIT7   21

#define LCD_RST	        12       // Reset  
#define LCD_CS	        13       // Chip select 0 - Selected 1 - Not selected
#define LCD_RS	        14      // 0 - Command 1 - Data
#define LCD_WR	        27      // Rise edge write
#define LCD_RD	        26      // Rise edge read

void setup_lcd_pins();

void init_lcd();

void delay_ms(unsigned int ms);

void write_pixel(unsigned short x, unsigned short y,
    unsigned char r, unsigned char g, unsigned char b);

void write_string(char *str, 
    unsigned short x, unsigned short y,
    unsigned char r, unsigned char g, unsigned char b);