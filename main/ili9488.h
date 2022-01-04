#include <stdio.h>

#define LCD_DATA_BIT0   12
#define LCD_DATA_BIT1   13
#define LCD_DATA_BIT2   27
#define LCD_DATA_BIT3   14
#define LCD_DATA_BIT4   25
#define LCD_DATA_BIT5   26
#define LCD_DATA_BIT6   32
#define LCD_DATA_BIT7   33

#define LCD_RST	        2       // Reset  
#define LCD_CS	        4       // Chip select 0 - Selected 1 - Not selected
#define LCD_RS	        17      // 0 - Command 1 - Data
#define LCD_WR	        18      // Rise edge write
#define LCD_RD	        21      // Rise edge read

void setup_lcd_pins();

void init_lcd();

void delay_ms(unsigned int ms);

void write_pixel(unsigned short x, unsigned short y,
    unsigned char r, unsigned char g, unsigned char b);

void write_string(char *str, 
    unsigned short x, unsigned short y,
    unsigned char r, unsigned char g, unsigned char b);