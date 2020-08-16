///////////////////////////////////////////////////////////////////////////
////                             LCDD.C                                ////
////                 Driver for common LCD modules                     ////
////                                                                   ////
////  lcd_init()   Must be called before any other function.           ////
////                                                                   ////
////  lcd_putc(c)  Will display c on the next position of the LCD.     ////
////                     The following have special meaning:           ////
////                      \f  Clear display                            ////
////                      \n  Go to start of second line               ////
////                      \b  Move back one position                   ////
////                                                                   ////
////  lcd_gotoxy(x,y) Set write position on LCD (upper left is 1,1)    ////
////                                                                   ////
////  lcd_getc(x,y)   Returns character at position x,y on LCD         ////
////                                                                   ////
///////////////////////////////////////////////////////////////////////////
////        (C) Copyright 2010 2M Electrónica Ltda.			           ////
///////////////////////////////////////////////////////////////////////////

#define enable PIN_D2
#define rw		PIN_D1
#define rs 		PIN_D0

#define lcd_line_two 0x40		// LCD RAM address for the second line
#define lcd_type 2				//Type of LCD

struct lcd_data_pin {
	int	nada : 4;				//Spare por configuración de hardware
	int	data : 4;				//Datos LCD (configuración 4 bit)
        } lcd;

#locate lcd = 0xF83	//Posición de memoria del puerto asignado a datos

//Aqui se define el puerto que usa las lineas de datos
//las lineas de comando no cambian de dirección
#define set_tris_lcd(x) set_tris_d(x)

BYTE const LCD_INIT_STRING[4] = {0x20 | (lcd_type << 2), 0xc, 1, 6};	// These bytes need to be sent to the LCD to start it up.

#DEFINE LCD_WRITE 0x00			// For write mode data pins are out 
#DEFINE LCD_READ 0xF0			// For read mode data pins are in

BYTE lcd_read_byte() {
      BYTE low,high;
      set_tris_lcd(LCD_READ);
      output_bit(rw,1);
      delay_cycles(1);
      output_bit(enable,1);
      delay_cycles(1);
      high = lcd.data;
      output_bit(enable,0);
      delay_cycles(1);
      output_bit(enable,1);
      delay_us(1);
      low = lcd.data;
      output_bit(enable,0);
      set_tris_lcd(LCD_WRITE);
      return( (high<<4) | low);
}


void lcd_send_nibble( BYTE n ) {
		lcd.data = n;
      delay_cycles(1);
      output_bit(enable,1);
      delay_us(2);
      output_bit(enable,0);
}


void lcd_send_byte( BYTE address, BYTE n ) {

      output_bit(rs,0);
      while ( bit_test(lcd_read_byte(),7) ) ;
      output_bit(rs,address);
      delay_cycles(1);
      output_bit(rw,0);
      delay_cycles(1);
      output_bit(enable,0);
      lcd_send_nibble(n >> 4);
      lcd_send_nibble(n & 0xf);
}


void lcd_init() {
    BYTE i;
    set_tris_lcd(LCD_WRITE);
    output_bit(rs,0);
    output_bit(rw,0);
    output_low(enable);
    delay_ms(15);
    for(i=1;i<=3;++i) {
       lcd_send_nibble(3);
       delay_ms(5);
    }
    lcd_send_nibble(2);
    for(i=0;i<=3;++i)
       lcd_send_byte(0,LCD_INIT_STRING[i]);
}


void lcd_gotoxy( BYTE x, BYTE y) {
   BYTE address;

   if(y!=1)
     address=lcd_line_two;
   else
     address=0;
   address+=x-1;
   lcd_send_byte(0,0x80|address);
}

void lcd_putc( char c) {
   switch (c) {
     case '\f'   : lcd_send_byte(0,1);
                   delay_ms(2);
                                           break;
     case '\n'   : lcd_gotoxy(1,2);        break;
     case '\b'   : lcd_send_byte(0,0x10);  break;
     default     : lcd_send_byte(1,c);     break;
   }
}

char lcd_getc( BYTE x, BYTE y) {
   char value;

    lcd_gotoxy(x,y);
    while ( bit_test(lcd_read_byte(),7) ); // wait until busy flag is low
    output_bit(rs,1);
    value = lcd_read_byte();
    output_bit(rs,0);
    return(value);
}
