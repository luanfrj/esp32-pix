
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "ili9488.h"
#include "font_table.h"

#define dir_mask (\
    (1 << LCD_DATA_BIT0) | \
    (1 << LCD_DATA_BIT1) | \
    (1 << LCD_DATA_BIT2) | \
    (1 << LCD_DATA_BIT3) | \
    (1 << LCD_DATA_BIT4) | \
    (1 << LCD_DATA_BIT5) | \
    (1 << LCD_DATA_BIT6) | \
    (1 << LCD_DATA_BIT7))

uint32_t xset_mask[256];

#define PARALLEL_INIT_LCD_DATA_DATA_BUS                     \
for (uint32_t c = 0; c<256; c++)                             \
{                                                           \
    xset_mask[c] = 0;                                       \
    if ( c & 0x01 ) xset_mask[c] |= (1 << LCD_DATA_BIT0);   \
    if ( c & 0x02 ) xset_mask[c] |= (1 << LCD_DATA_BIT1);   \
    if ( c & 0x04 ) xset_mask[c] |= (1 << LCD_DATA_BIT2);   \
    if ( c & 0x08 ) xset_mask[c] |= (1 << LCD_DATA_BIT3);   \
    if ( c & 0x10 ) xset_mask[c] |= (1 << LCD_DATA_BIT4);   \
    if ( c & 0x20 ) xset_mask[c] |= (1 << LCD_DATA_BIT5);   \
    if ( c & 0x40 ) xset_mask[c] |= (1 << LCD_DATA_BIT6);   \
    if ( c & 0x80 ) xset_mask[c] |= (1 << LCD_DATA_BIT7);   \
}


#define set_mask(C) xset_mask[C]

/*
// #define set_mask(C)\ 
//     (((C)&0x80)>>7)<<LCD_DATA_BIT7 | \ 
//     (((C)&0x40)>>6)<<LCD_DATA_BIT6 | \ 
//     (((C)&0x20)>>5)<<LCD_DATA_BIT5 | \ 
//     (((C)&0x10)>>4)<<LCD_DATA_BIT4 | \ 
//     (((C)&0x08)>>3)<<LCD_DATA_BIT3 | \ 
//     (((C)&0x04)>>2)<<LCD_DATA_BIT2 | \ 
//     (((C)&0x02)>>1)<<LCD_DATA_BIT1 | \ 
//     (((C)&0x01)>>0)<<LCD_DATA_BIT0
*/

/* 
 * Realiza o setup dos pinos de comunicação com o display LCD
 */
void setup_lcd_pins()
{
    gpio_pad_select_gpio(LCD_DATA_BIT0);
    gpio_pad_select_gpio(LCD_DATA_BIT1);
    gpio_pad_select_gpio(LCD_DATA_BIT2);
    gpio_pad_select_gpio(LCD_DATA_BIT3);
    gpio_pad_select_gpio(LCD_DATA_BIT4);
    gpio_pad_select_gpio(LCD_DATA_BIT5);
    gpio_pad_select_gpio(LCD_DATA_BIT6);
    gpio_pad_select_gpio(LCD_DATA_BIT7);
    gpio_pad_select_gpio(LCD_RST);
    gpio_pad_select_gpio(LCD_CS);
    gpio_pad_select_gpio(LCD_RS);
    gpio_pad_select_gpio(LCD_WR);
    gpio_pad_select_gpio(LCD_RD);

    gpio_intr_disable(LCD_DATA_BIT0);
    gpio_intr_disable(LCD_DATA_BIT1);
    gpio_intr_disable(LCD_DATA_BIT2);
    gpio_intr_disable(LCD_DATA_BIT3);
    gpio_intr_disable(LCD_DATA_BIT4);
    gpio_intr_disable(LCD_DATA_BIT5);
    gpio_intr_disable(LCD_DATA_BIT6);
    gpio_intr_disable(LCD_DATA_BIT7);
    gpio_intr_disable(LCD_RST);
    gpio_intr_disable(LCD_CS);
    gpio_intr_disable(LCD_RS);
    gpio_intr_disable(LCD_WR);
    gpio_intr_disable(LCD_RD);

    gpio_set_direction(LCD_DATA_BIT0, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_DATA_BIT1, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_DATA_BIT2, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_DATA_BIT3, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_DATA_BIT4, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_DATA_BIT5, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_DATA_BIT6, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_DATA_BIT7, GPIO_MODE_OUTPUT);

    gpio_set_direction(LCD_RST, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_CS, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_RS, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_WR, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_RD, GPIO_MODE_OUTPUT);

    PARALLEL_INIT_LCD_DATA_DATA_BUS;
}

/* 
 * Copia um byte para os GPIOs especificados
 */
void copy_data_to_gpio(unsigned char byte_data)
{
    GPIO.out_w1tc = dir_mask;
    GPIO.out_w1ts = set_mask(byte_data);
}

/* 
 * Envia um comando para o LCD
 */
void send_command(unsigned char cmd)
{
    GPIO.out_w1tc = (1 << LCD_CS) | (1 << LCD_RS) | (1 << LCD_WR) | (1 << LCD_RD);
    GPIO.out_w1ts = (1 << LCD_RST) | (1 << LCD_RD);
    copy_data_to_gpio(cmd);
    GPIO.out_w1ts = (1 << LCD_WR);
}

/* 
 * Envia um dado para o LCD
 */
void send_data(unsigned char data)
{
    GPIO.out_w1tc = (1 << LCD_CS) | (1 << LCD_RS) | (1 << LCD_WR) | (1 << LCD_RD);
    GPIO.out_w1ts = (1 << LCD_RST) | (1 << LCD_RS) | (1 << LCD_RD);
    copy_data_to_gpio(data);
    GPIO.out_w1ts = (1 << LCD_WR);
}

// set bg collor
void set_bgcolor(unsigned char r, unsigned char g, unsigned char b)
{
    unsigned short i, j;
     // write data command
    send_command(0x2C);
    
    // send color
    for (i = 0; i < 320; i++)
    {
        for (j = 0; j < 480; j++)
        {
            send_data(r);
            send_data(g);
            send_data(b);
        }
    }
}

// set address
void set_address(unsigned short x, unsigned short y)
{
    unsigned char xt, xb, yt, yb;
    
    xt = (x >> 8) & 0xFF;
    xb = x & 0xFF;
    
    yt = (y >> 8) & 0xFF;
    yb = y & 0xFF;

    // set cursor
    send_command(0x2A);
    // set start x
    send_data(xt);
    send_data(xb);
    // set end x
    send_data(xt);
    send_data(xb);
    send_command(0x00);
    
    send_command(0x2B);
    // set start y
    send_data(yt);
    send_data(yb);
    // set end y
    send_data(yt);
    send_data(yb);
    send_command(0x00);
}

// write pixel at position
void write_pixel(unsigned short x, unsigned short y,
    unsigned char r, unsigned char g, unsigned char b)
{
    set_address(x, y);
    send_command(0x2C);
    send_data(r);
    send_data(g);
    send_data(b); 
}

// write char row
void write_row(unsigned char rt, unsigned char rb, 
        unsigned short x, unsigned short y, 
        unsigned char r, unsigned char g, unsigned char b)
{
    unsigned short row;
    unsigned char i;
    row = (rt << 8) | (rb);
    
    for (i = 0; i < 10; i++)
    {
        if (row > 32767)
        {
            write_pixel(x, y, r, g, b);
        }
        x++;
        row = row << 1;
    }
}

// write character at position
void write_char(char c, unsigned short x, unsigned short y, 
        unsigned char r, unsigned char g, unsigned char b)
{
    unsigned short base = (c - ' ') * 30;
    unsigned char i, rt, rb;
    
    for (i = 0; i < 15; i++)
    {
        rt = courierNew_12ptBitmaps[base + 2*i];
        rb = courierNew_12ptBitmaps[base + 2*i + 1];
        write_row(rt, rb, x, y, r, g, b);
        y++;
    }
    
}

// write string
void write_string(char *str, 
        unsigned short x, unsigned short y,
        unsigned char r, unsigned char g, unsigned char b)
{
    while(*str)
    {
        write_char(*str, x, y, r, g, b);
        x += 10;
        str++;
    }
}

void delay_ms(unsigned int ms) 
{
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

// init lcd
void init_lcd()
{
    // reset LCD
    gpio_set_level(LCD_RST, 0);
    delay_ms(100);
    gpio_set_level(LCD_RST, 1);
    
    // software reset
    send_command(0x01);
    delay_ms(100);
    
    // sleep out
    send_command(0x11);
    delay_ms(100);
    
    // memory acces control
    send_command(0x36);
    send_data(0x48);
    delay_ms(100);
    
    // set dbi
    send_command(0x3A);
    send_data(0x77);
    delay_ms(100);
    
    // partial mode on
    send_command(0x13);
    delay_ms(100);
    
    // display on
    send_command(0x29);
    delay_ms(100);
    
    // set cursor
    send_command(0x2A);
    // set start x
    send_data(0x00);
    send_data(0x00);
    // set end x
    send_data(0x01);
    send_data(0x3F);
    send_command(0x00);
    
    send_command(0x2B);
    // set start y
    send_data(0x00);
    send_data(0x00);
    // set end y
    send_data(0x01);
    send_data(0xDF);
    send_command(0x00);
    
    delay_ms(100);
   
    // set brightness
    send_command(0x51);
    send_data(0x0F);
    delay_ms(100);
    
    // set brightness control
    send_command(0x53);
    send_data(0x2C);
    delay_ms(100);
    
    // set framerate
    send_command(0xB1);
    send_data(0xA0);
    send_data(0x11);
    delay_ms(50);
    
    set_bgcolor(0, 0, 0);
    
    gpio_set_level(LCD_CS, 0);
    delay_ms(100);
    
}