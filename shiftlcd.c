#include <xc.h>
#include "shiftlcd.h"

#define E_PIN 0b00001000
#define RS_PIN 0b00000100
#define PWR_PIN 0b00000010
#define BK_PIN 0b00000001


#define EIGHTSTATE_BYTE 0b00110000
#define FOURSTATE_BYTE 0b00100000


#define CLS_DISP_BIT 0b00000001

#define DISP_CTRL_BIT 0b00001000
#define CURSBLINK_BIT 0b00000001
#define CURSON_BIT 0b00000010
#define DISP_ON_BIT 0b00000100

#define SCGRAM_BIT 0b01000000
#define SDDRAM_BIT 0b10000000

#define WRAM_BIT 0b00000000

#define _XTAL_FREQ 8000000

const unsigned char row_offsets[] = {0x00, 0x40, 0x14, 0x54};

//Clocks shift register clock
void shift_clock(Lcd_cfg cfg) {
    *cfg.clock_port &= ~cfg.clock_pin;
    *cfg.clock_port |= cfg.clock_pin;
    *cfg.clock_port &= ~cfg.clock_pin;
}

//Clocks shift reg strobe
void shift_strobe(Lcd_cfg cfg) {
    
    *cfg.strobe_port &= ~cfg.strobe_pin; //Setting it to 0
    *cfg.strobe_port |= cfg.strobe_pin; //Setting it to 1
    *cfg.strobe_port &= ~cfg.strobe_pin; //Setting it to 0
}

//Sets the next bit to be shifted
void shift_data(Lcd_cfg cfg, unsigned char val) {
    if(val == 0){
        *cfg.data_port &= ~cfg.data_pin;
        return;
    }
    
    *cfg.data_port |= cfg.data_pin;
}

//Shifts out byte to shift register
void shift_byte(Lcd_cfg cfg, unsigned char b) { 
    
    b |= cfg.metactrl;
    
    for(char i=0; i <= 7; i++){
        shift_data(cfg, b & 128); //b anded with 128 leaves either 128 or 0
        shift_clock(cfg);
        b <<= 1;
    }
    
    shift_strobe(cfg);
}



void Lcd_write_nibble(Lcd_cfg cfg, unsigned char nibble) {
    shift_byte(cfg, nibble | E_PIN); //E high, 
    shift_byte(cfg, nibble & ~E_PIN); //E LOW;
}

void Lcd_write_data(Lcd_cfg cfg, unsigned char cmd){
    Lcd_write_nibble(cfg, (cmd & 0b11110000) | RS_PIN);
    cmd <<= 4;
    Lcd_write_nibble(cfg, (cmd & 0b11110000) | RS_PIN);
}

void Lcd_write_command(Lcd_cfg cfg, unsigned char cmd){
    Lcd_write_nibble(cfg, (cmd & 0b11110000) & ~RS_PIN);
    cmd <<= 4;
    Lcd_write_nibble(cfg, cmd & 0b11110000);
}

void Lcd_print(Lcd_cfg *cfg, char *str){
    
    for(char i=0; ; i++){
        if(str[i] == '\0'){
            break;
        }
        Lcd_putc(cfg, str[i]);
    }
    
}

//Put character on display
void Lcd_putc(Lcd_cfg *cfg, unsigned char c){
    if(c == '\n'){
        Lcd_set_cursor(cfg, cfg->curr_row+1, 0);
        return;
    }
    
    Lcd_write_data(*cfg, WRAM_BIT | c);
    __delay_us(37);
    Lcd_set_cursor(cfg, cfg->curr_row, cfg->curr_col+1);
}

//Set lcd cursor position
void Lcd_set_cursor(Lcd_cfg *cfg, unsigned char row, unsigned char col){
    
    if(col > cfg->cols){
        col = 0;
        row += 1;
    }
    
    if(row > cfg->rows)
        row = cfg->rows;
   
    cfg->curr_row = row;
    cfg->curr_col = col;
    
    unsigned char position = row_offsets[row];
    position += col;
    
    Lcd_write_command(*cfg, SDDRAM_BIT | position);
    __delay_us(37);
}

//Power on the LCD, represents actual power delivery (If wired up)
void Lcd_poweron(Lcd_cfg *cfg){
    cfg->metactrl = cfg->metactrl | PWR_PIN;
    shift_byte(*cfg, cfg->metactrl);
}

//Power off the LCD, represents actual power delivery (If wired up)
void Lcd_poweroff(Lcd_cfg *cfg){
    cfg->metactrl = cfg->metactrl & ~PWR_PIN;
    shift_byte(*cfg, cfg->metactrl);
}

void Lcd_backlight_off(Lcd_cfg *cfg){
    cfg->metactrl = cfg->metactrl & ~BK_PIN;
    shift_byte(*cfg, cfg->metactrl);
}

void Lcd_backlight_on(Lcd_cfg *cfg){
    cfg->metactrl = cfg->metactrl | BK_PIN;
    shift_byte(*cfg, cfg->metactrl);
}

void Lcd_on(Lcd_cfg *cfg){
    cfg->cursctrl = cfg->cursctrl | DISP_ON_BIT;
    Lcd_write_command(*cfg, DISP_CTRL_BIT | cfg->cursctrl);
}

void Lcd_off(Lcd_cfg *cfg){
    cfg->cursctrl = cfg->cursctrl & ~DISP_ON_BIT;
    Lcd_write_command(*cfg, DISP_CTRL_BIT | cfg->cursctrl);
}

void Lcd_curs_blink_on(Lcd_cfg *cfg){
    cfg->cursctrl = cfg->cursctrl | CURSBLINK_BIT;
    Lcd_write_command(*cfg, DISP_CTRL_BIT | cfg->cursctrl);
    __delay_us(37);
}

void Lcd_curs_blink_off(Lcd_cfg *cfg){
    cfg->cursctrl = cfg->cursctrl & ~CURSBLINK_BIT;
    Lcd_write_command(*cfg, DISP_CTRL_BIT & cfg->cursctrl);
    __delay_us(37);
}

void Lcd_curs_on(Lcd_cfg *cfg){
    cfg->cursctrl = cfg->cursctrl | CURSON_BIT;
    Lcd_write_command(*cfg, DISP_CTRL_BIT | cfg->cursctrl);
    __delay_us(37);
}

void Lcd_curs_off(Lcd_cfg *cfg){
    cfg->cursctrl = cfg->cursctrl & ~CURSON_BIT;
    Lcd_write_command(*cfg, DISP_CTRL_BIT & cfg->cursctrl);
    __delay_us(37);
}

void Lcd_cls(Lcd_cfg *cfg){
    cfg->curr_row = 0;
    cfg->curr_col = 0;
    
    Lcd_write_command(*cfg, CLS_DISP_BIT);
    __delay_ms(2);
    
}


//Runs the magic sequence to initialize the display, no matter its current state.
void Lcd_init(Lcd_cfg *cfg, unsigned char rows, unsigned char cols){
    
    __delay_ms(100);
    
    cfg->rows = rows;
    cfg->cols = cols;
    
    for(char i=0; i < 3; i++){
        shift_byte(*cfg, EIGHTSTATE_BYTE | E_PIN);
        shift_byte(*cfg, EIGHTSTATE_BYTE & ~E_PIN);
        __delay_us(37);
    }
    
    shift_byte(*cfg, FOURSTATE_BYTE | E_PIN);
    shift_byte(*cfg, FOURSTATE_BYTE & ~E_PIN);
    __delay_us(37);

    cfg->initialized = 1;
}

/*
data_port, clock_port and strobe_port represent the port registers.
e.g. LATB (Not PORTB or TRISB)

data_pin, clock_pin and strobe_pin represent bitmasks for the specific PIN on that register.
To use say pin 0 of PORT-A as the data line you would use:

data_port=LATA
data_pin=0x01

Remember to call Lcd_init afterwards
*/
Lcd_cfg New_Shift_Lcd(volatile unsigned char* data_port, 
                       volatile unsigned char* clock_port,
                       volatile unsigned char* strobe_port,
        
                       unsigned char data_pin, 
                       unsigned char clock_pin,
                       unsigned char strobe_pin){
    
    Lcd_cfg cfg;
    
    cfg.data_port = data_port;
    cfg.clock_port = clock_port;
    cfg.strobe_port = strobe_port;
    
    cfg.data_pin = data_pin;
    cfg.clock_pin = clock_pin;
    cfg.strobe_pin = strobe_pin;
    
    cfg.initialized = 0;
    cfg.rows = 0;
    cfg.cols = 0;
    cfg.curr_row = 0;
    cfg.curr_col = 0;
    cfg.metactrl = 0;
    cfg.cursctrl = 0;
    
    return cfg;
}
