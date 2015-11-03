#include "app.h"

#include <wiringPi.h>

void lcd_write(const char *t) {
    if (t == NULL) {
        return;
    }

    for (int i = 0; t[i] != '\0'; ++i) {
        lcd_send(DATA, t[i]);
    }
}

void lcd_set_pos(int posy, int posx) {
    posx %= LCD_COLUMNS; // Maybe just return if the position is out of bounds?
    posy %= LCD_ROWS;
    lcd_send(CMD, LCD_SETDDRAM | (posx | display_offsets[posy]));
    delay(1);
}

void lcd_send(const lcd_message_type type, const unsigned char c) {
    digitalWrite(LCD_PIN_RS, (int)type);

    write_nibble(c, 4); // High nibble
    write_nibble(c, 0); // Low nibble

    delay(1);
}

void write_nibble(const unsigned char c, const unsigned char offset) {
    digitalWrite(LCD_PIN_D7, bit_is_set(c, offset + 3));
    digitalWrite(LCD_PIN_D6, bit_is_set(c, offset + 2));
    digitalWrite(LCD_PIN_D5, bit_is_set(c, offset + 1));
    digitalWrite(LCD_PIN_D4, bit_is_set(c, offset + 0));

    lcd_send_apply();
}

void lcd_send_apply() {
    // Writing signal flag to apply data which is set to pins
    digitalWrite(LCD_PIN_E, 1);
    delay(1);
    digitalWrite(LCD_PIN_E, 0);
}

void lcd_clear() {
    lcd_send(CMD, LCD_CLEAR);
}

void lcd_init() {
    // Set ports as output
    pinMode(LCD_PIN_RS, OUTPUT);
    pinMode(LCD_PIN_E, OUTPUT);
    pinMode(LCD_PIN_D4, OUTPUT);
    pinMode(LCD_PIN_D5, OUTPUT);
    pinMode(LCD_PIN_D6, OUTPUT);
    pinMode(LCD_PIN_D7, OUTPUT);

    // wait for lcd to be ready
    delay(50);

    // configure 4 bit mode ------Function-Set 1
    digitalWrite(LCD_PIN_D5, 1);
    digitalWrite(LCD_PIN_D4, 0);

    delay(50);
    // configure 4 bit mode ------Function-Set 2
    digitalWrite(LCD_PIN_D5, 1);
    digitalWrite(LCD_PIN_D4, 0);

    delay(50);
    // configure 4 bit mode ------Function-Set 3
    digitalWrite(LCD_PIN_D5, 1);
    digitalWrite(LCD_PIN_D4, 0);

    // apply 4 bit mode
    lcd_send_apply();
    delay(50);

/* 2 Zeilen, 4-Bit Modus */
    lcd_send(CMD, 0x28);
//    lcd_send(CMD, 0x14);

    lcd_send(CMD, LCD_OFF);
    lcd_send(CMD, LCD_CLEAR);

    lcd_send(CMD, 0x06);
    lcd_send(CMD, LCD_ON);
    lcd_clear();
}

void lcd_generate_char(int code, const int *data) {
    // Startposition des Zeichens einstellen
    lcd_send(CMD, 0x40|(code<<3));

    // Bitmuster übertragen
    for (int i=0; i<8; i++) {
        lcd_send(DATA, data[i]);
    }
}

void lcd_generate_chars() {
    lcd_send(CMD, 0x40|(0<<3));
    // custom character data: °, Battery empty, Battery..., Battery full
    int data[8][8] = {
        {0b00110, 0b01001, 0b01001, 0b00110, 0b00000, 0b00000, 0b00000, 0b00000},
        {0b01110, 0b11011, 0b10101, 0b10101, 0b10001, 0b10101, 0b10001, 0b11111},
        {0b01110, 0b11011, 0b10001, 0b10001, 0b10001, 0b10001, 0b11111, 0b11111},
        {0b01110, 0b11011, 0b10001, 0b10001, 0b10001, 0b11111, 0b11111, 0b11111},
        {0b01110, 0b11011, 0b10001, 0b10001, 0b11111, 0b11111, 0b11111, 0b11111},
        {0b01110, 0b11011, 0b10001, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111},
        {0b01110, 0b11011, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111},
        {0b01110, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111},
    };
    for (int i=0; i<8; i++) {
        lcd_generate_char(i, data[i]);
    }
}

bool bit_is_set(int sfr, char bit) {
    return (sfr & (1 << bit)) ? 1 : 0;
}
