#define bit_is_set(sfr, bit) sfr & (1 << bit)

#include "app.h"
#include <wiringPi.h>
#include <string.h>

int main(int argc, char *argv[]) {
    wiringPiSetup();
    if (argc == 2 && strcmp(argv[1], "--init") == 0) {
        lcd_init();
        return 0;
    }
    delay(5);
    lcd_send(CMD, LCD_CURSOROFF);
    if (argc == 2) {
        lcd_clear();
        if (strlen(argv[1]) > 2) {
            lcd_write("ERR: argv[1] > 2");
            return 1;
        }
        char curBalance[4];
        sprintf(curBalance, "%2sE\0", argv[1]);
        lcd_write("Cur Balance:");
        lcd_set_pos(0, 16 - 3);
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
        sprintf(articleName, "%13s\0", argv[3]);
        lcd_set_pos(1, 0);
        lcd_write(articleName);
        lcd_set_pos(1, 16 - 3);
        lcd_write(price);
        lcd_set_pos(2, 0);
        lcd_write("New Balance:");
        lcd_set_pos(3, 16 - 3);
        lcd_write(newBalance);
    }
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

void lcd_set_pos(int posy, int posx) {
    lcd_send(CMD, LCD_SETDDRAM + posx + 0x40 * posy);
    delay(1);
}

void lcd_send(unsigned char type, unsigned char c) {
    if (type == CMD)
        digitalWrite(LCD_PIN_RS, 0); /* RS=0: Befehl folgt ... ******/
    else
        digitalWrite(LCD_PIN_RS, 1); /* RS=1: Daten folgen ... ******/

    /* (1) HIGH NIBBLE wird gesendet ******/
    if (bit_is_set(c, 7))
        digitalWrite(LCD_PIN_D7, 1);
    else
        digitalWrite(LCD_PIN_D7, 0);

    if (bit_is_set(c, 6))
        digitalWrite(LCD_PIN_D6, 1);
    else
        digitalWrite(LCD_PIN_D6, 0);

    if (bit_is_set(c, 5))
        digitalWrite(LCD_PIN_D5, 1);
    else
        digitalWrite(LCD_PIN_D5, 0);

    if (bit_is_set(c, 4))
        digitalWrite(LCD_PIN_D4, 1);
    else
        digitalWrite(LCD_PIN_D4, 0);

    /* Flanke zur Übernahme der Daten senden ... ******/
    digitalWrite(LCD_PIN_E, 1);
    delay(1);
    digitalWrite(LCD_PIN_E, 0);

    /* (2) LOW NIBBLE wird gesendet ******/
    if (bit_is_set(c, 3))
        digitalWrite(LCD_PIN_D7, 1);
    else
        digitalWrite(LCD_PIN_D7, 0);

    if (bit_is_set(c, 2))
        digitalWrite(LCD_PIN_D6, 1);
    else
        digitalWrite(LCD_PIN_D6, 0);

    if (bit_is_set(c, 1))
        digitalWrite(LCD_PIN_D5, 1);
    else
        digitalWrite(LCD_PIN_D5, 0);

    if (bit_is_set(c, 0))
        digitalWrite(LCD_PIN_D4, 1);
    else
        digitalWrite(LCD_PIN_D4, 0);

    /* Flanke zur Übernahme der Daten senden ... ******/
    digitalWrite(LCD_PIN_E, 1);
    delay(1);
    digitalWrite(LCD_PIN_E, 0);

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
//    lcd_send(CMD, 0x28);
    lcd_send(CMD, 0x14);

    lcd_send(CMD, LCD_OFF);
    lcd_send(CMD, LCD_CLEAR);

    lcd_send(CMD, 0x06);
    lcd_send(CMD, LCD_ON);
    lcd_clear();
}
