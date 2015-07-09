#define bit_is_set(sfr, bit) ((sfr & (1 << bit)) ? 1 : 0)

#include "app.h"
#include <wiringPi.h>
#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    wiringPiSetup();
    if (argc == 2 && strcmp(argv[1], "--init") == 0) {
        lcd_init();
        lcd_generate_chars();
        return 0;
    }
    delay(5);
    lcd_send(CMD, LCD_CURSOROFF);
    delay(5);
    if (argc == 2) {
        lcd_clear();
        if (strlen(argv[1]) > 2) {
            lcd_write("ERR: argv[1] > 2");
            return 1;
        }
        char curBalance[4];
        sprintf(curBalance, "%2sE\0", argv[1]);
        lcd_write("Cur Balance:");
        lcd_set_pos(0, 13);
        lcd_write(curBalance);
    } else if (argc == 4) {
        if (strlen(argv[1]) > 2) {
            lcd_clear();
            lcd_write("ERR: argv[1] > 2");
            return 1;
        }
        if (strlen(argv[2]) > 2) {
            lcd_clear();
            lcd_write("ERR: argv[2] > 2");
            return 1;
        }
        char newBalance[4];
        char price[4];
        char articleName[16 - 2];
        sprintf(newBalance, "%2sE\0", argv[1]);
        sprintf(price, "%2sE\0", argv[2]);
        sprintf(articleName, "%-12s\0", argv[3]);
        lcd_set_pos(1, 0);
        lcd_write(articleName);
        lcd_set_pos(1, 13);
        lcd_write(price);
        lcd_set_pos(2, 0);
        lcd_write("New Balance:");
        lcd_set_pos(2, 13);
        lcd_write(newBalance);
    } else {
	    lcd_clear();

        while (1) {

         for (int line = 0; line < 9001; line++) {
                for (int col = 0; col < 20; col++) {
                    lcd_set_pos(line, col);
                    lcd_write("X");
                    delay(100);
                    lcd_set_pos(line, col);
                    lcd_write(" ");
                    delay(50);
                }
            }
        }

        // lcd_write("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890,.-#+<|!");
    }
    lcd_set_pos(3, 15);
    lcd_send(DATA, 0);
    return 0;
}

void lcd_write(char *t) {
    for (int i = 0; i < 255; i++) {
        if (t[i] == '\0') {
            return;
        }
        lcd_send(DATA, t[i]);
    }
}

const char display_offsets[] = { 0x00, 0x40, 0x10, 0x50 };

void lcd_set_pos(int posy, int posx) {
    posx %= LCD_COLUMNS;
    posy %= (sizeof(display_offsets) / sizeof(char));
    lcd_send(CMD, LCD_SETDDRAM | (posx | display_offsets[posy]));
    delay(1);
}

const char data_pins[] = { LCD_PIN_D7, LCD_PIN_D6, LCD_PIN_D5, LCD_PIN_D4 };

void lcd_send(unsigned char type, unsigned char c) {
    if (type == CMD)
        digitalWrite(LCD_PIN_RS, 0); /* RS=0: Befehl folgt ... ******/
    else
        digitalWrite(LCD_PIN_RS, 1); /* RS=1: Daten folgen ... ******/

    /* (1) HIGH NIBBLE wird gesendet ******/
    for(int i = 7; i >= 0; --i) {
        char current_pin = data_pins[(7 - i) % (sizeof(data_pins) / sizeof(char))];
        digitalWrite(current_pin, bit_is_set(c, i));

        if((i % LCD_ROWS) == 0) {
            /* Flanke zur Übernahme der Daten senden ... ******/
            digitalWrite(LCD_PIN_E, 1);
            delay(1);
            digitalWrite(LCD_PIN_E, 0);
        }
    }

    /* (3) Auf den LCD Controller warten ...******/
    delay(5);
}

void lcd_clear() {
    lcd_send(CMD, LCD_CLEAR);
}

void lcd_init() {
/* Set ports as output */
    pinMode(LCD_PIN_RS, OUTPUT);
    pinMode(LCD_PIN_E, OUTPUT);
    pinMode(LCD_PIN_D4, OUTPUT);
    pinMode(LCD_PIN_D5, OUTPUT);
    pinMode(LCD_PIN_D6, OUTPUT);
    pinMode(LCD_PIN_D7, OUTPUT);

    // wait for lcd to be ready
    delay(50);

/* configure 4-bit mode ------Function-Set 1*/
    digitalWrite(LCD_PIN_D5, 1);
    digitalWrite(LCD_PIN_D4, 0);

    delay(50);
/* configure 4-bit mode ------Function-Set 2*/
    digitalWrite(LCD_PIN_D5, 1);
    digitalWrite(LCD_PIN_D4, 0);

    delay(50);
/* configure 4-bit mode ------Function-Set 3*/
    digitalWrite(LCD_PIN_D5, 1);
    digitalWrite(LCD_PIN_D4, 0);

/* 4-Bit Modus starten ... */
    digitalWrite(LCD_PIN_E, 1);
    digitalWrite(LCD_PIN_E, 0);
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
    for (int i=0; i<8; i++)
    {
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
