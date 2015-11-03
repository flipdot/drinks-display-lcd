#include <wiringPi.h>

#include <string.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    wiringPiSetup();
    lcd_send(CMD, LCD_CURSOROFF);
    if (argc == 2) {
        if (strcmp(argv[1], "--init") == 0) {
            lcd_init();
            lcd_generate_chars();
            return 0;
        } else if (strcmp(argv[1], "--help") == 0) {
            printf("Usage: app CURRENT_BALANCE\n");
            printf("or\n");
            printf("Usage: app NEW_BALANCE PRICE ARTICLE\n");
            printf("Example:\n");
            printf("First call: app 5\n");
            printf("Second call: app 4 1 Beer\n");
            printf("or\n");
            printf("Usage: app --raw justSomeTextToBeDisplayed\n");
            return 0;
        }
        lcd_clear();
        if (strlen(argv[1]) > 2) {
            lcd_write("ERR: argv[1] > 2");
            return 1;
        }
        char curBalance[4];
        snprintf(curBalance, sizeof(curBalance), "%2sE\0", argv[1]);
        lcd_set_pos(0, 0);
        lcd_write("Cur Balance:");
        lcd_set_pos(0, 13);
        lcd_write(curBalance);
    } else if (argc == 3 && strcmp(argv[1], "--raw") == 0) {
        lcd_clear();
        for (int i=0; i<4; i++) {
            if (strlen(argv[2]) <= 16 * i) {
                break;
            }
            char line[17];
            strncpy(line, &argv[2][i * 16], 16);
            lcd_set_pos(i, 0);
            line[16] = '\0';
            lcd_write(line);
        }
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
        snprintf(newBalance, sizeof(newBalance), "%2sE\0", argv[1]);
        snprintf(price, sizeof(price), "%2sE\0", argv[2]);
        snprintf(articleName, sizeof(articleName), "%-12s\0", argv[3]);
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
        //lcd_write("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890,.-#+<|!");
    }
    //lcd_set_pos(3, 15);
    //lcd_send(DATA, 0);
    return 0;
}
