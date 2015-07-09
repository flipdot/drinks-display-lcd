// #define LCDPORT       PORTD
// #define LCDDDR        DDRD
// #define LCDDDR_PORT   0xFB
#define LCD_PIN_RS    7 // 0
#define LCD_PIN_E     9 // 1
#define LCD_PIN_D4    0 // 3
#define LCD_PIN_D5    1 // 4
#define LCD_PIN_D6    16 // 5
#define LCD_PIN_D7    15 // 6
#define LCD_SETDDRAM  0x80 // Set Display RAM Address

/***** LCD STEUERBEFEHLE *****/

#define LCD_CLEAR     0x01 // Display löschen
#define LCD_HOME      0x02 // Cursor an den Anfang zurück
#define LCD_ON        0x0C // 0x0C = LCD ON, Cursor OFF
#define LCD_OFF       0x08 // Display OFF

/***** HILFSDEFINITIONEN *****/

#define CMD           0
#define DATA          1

// flags for display on/off control
#define LCD_DISPLAYON 0x04
#define LCD_DISPLAYOFF 0x00
#define LCD_CURSORON 0x02
#define LCD_CURSOROFF 0x00
#define LCD_BLINKON 0x01
#define LCD_BLINKOFF 0x00

void lcd_clear(void);
void lcd_init(void);
void lcd_send(unsigned char type, unsigned char c);
void lcd_write(char *t);
void lcd_set_pos(int posy, int posx);
