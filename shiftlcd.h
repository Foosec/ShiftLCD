/* 
 * File:   shiftlcd.h
 * Author: foobar
 *
 * Created on April 28, 2020, 1:23 AM
 */

#ifndef SHIFTLCD_H
#define	SHIFTLCD_H

#ifdef	__cplusplus
extern "C" {
#endif

    typedef struct Lcd_cfg{
        volatile unsigned char *data_port; unsigned char data_pin;
        volatile unsigned char *clock_port; unsigned char clock_pin;
        volatile unsigned char *strobe_port; unsigned char strobe_pin;

        unsigned char rows; unsigned char cols;
        unsigned char curr_row; unsigned char curr_col;

        unsigned char cursctrl; unsigned char metactrl;

        unsigned char initialized;

        /*
         * SET THE PORTS TO OUTPUT YOURSELF
         * ports are LATX, pins are the binary representation of the pin you want
         *  for example, PORTB, PIN 0x01 would be PORTB pin 0
         * 
         */

    }Lcd_cfg;
    
    Lcd_cfg New_Shift_Lcd(volatile unsigned char* data_port, 
                       volatile unsigned char* clock_port,
                       volatile unsigned char* strobe_port,
        
                       unsigned char data_pin, 
                       unsigned char clock_pin,
                       unsigned char strobe_pin);
    
    void shift_byte(Lcd_cfg cfg, unsigned char b);
    void shift_data(Lcd_cfg cfg, unsigned char val);
    void shift_strobe(Lcd_cfg cfg);
    void shift_clock(Lcd_cfg cfg);
    
    
    void Lcd_write_command(Lcd_cfg cfg, unsigned char cmd);
    void Lcd_write_data(Lcd_cfg cfg, unsigned char cmd);
    void Lcd_write_nibble(Lcd_cfg cfg, unsigned char nibble);
    
    void Lcd_init(Lcd_cfg *cfg, unsigned char rows, unsigned char cols);
    void Lcd_print(Lcd_cfg *cfg, char *str);
    void Lcd_putc(Lcd_cfg *cfg, unsigned char c);
    void Lcd_set_cursor(Lcd_cfg *cfg, unsigned char row, unsigned char col);
    void Lcd_curs_blink_on(Lcd_cfg *cfg);
    void Lcd_curs_blink_off(Lcd_cfg *cfg);
    void Lcd_curs_on(Lcd_cfg *cfg);
    void Lcd_curs_off(Lcd_cfg *cfg);
    void Lcd_cls(Lcd_cfg *cfg);
    void Lcd_on(Lcd_cfg *cfg);
    void Lcd_off(Lcd_cfg *cfg);
    void Lcd_poweron(Lcd_cfg *cfg);
    void Lcd_poweroff(Lcd_cfg *cfg);
    void Lcd_backlight_off(Lcd_cfg *cfg);
    void Lcd_backlight_on(Lcd_cfg *cfg);
    
    



#ifdef	__cplusplus
}
#endif

#endif	/* SHIFTLCD_H */

