//Modbus sobre PIC16F877-887
//Marcos Silva Rivas
//Versión original		
//10-julio-2011	Implementación esclavo en MT_USB maestro en PC
//22-julio-2011	Implementación maestro en MT_USB esclavo en PC
//

//#include <16F887.h>
//#device *=16
//#device adc=16
//
//#FUSES NOWDT                 	//No Watch Dog Timer
//#FUSES INTRC_IO              	//Internal RC Osc, no CLKOUT
//#FUSES PUT                   	//Power Up Timer
//#FUSES MCLR                  	//Master Clear pin enabled
//#FUSES NOPROTECT             	//Code not protected from reading
//#FUSES NOCPD                 	//No EE protection
//#FUSES NOBROWNOUT            	//No brownout reset
//#FUSES NOIESO                	//Internal External Switch Over mode disabled
//#FUSES NOFCMEN               	//Fail-safe clock monitor disabled
//#FUSES NOLVP                 	//No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O
//#FUSES NODEBUG               	//No Debug mode for ICD
//#FUSES NOWRT                 	//Program memory not write protected
//#FUSES BORV21                	//Brownout reset at 2.1V
//#FUSES RESERVED              	//Used to set the reserved FUSE bits
//
//#use delay(clock=8000000,RESTART_WDT)
//#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8)

#include <16F877A.h>
#device *=16
#device adc=16

#FUSES NOWDT                 	//No Watch Dog Timer
#FUSES HS                    	//High speed Osc (> 4mhz for PCM/PCH) (>10mhz for PCD)
#FUSES PUT                   	//Power Up Timer
#FUSES NOPROTECT             	//Code not protected from reading
#FUSES NODEBUG               	//No Debug mode for ICD
#FUSES NOBROWNOUT            	//No brownout reset
#FUSES NOLVP                 	//No low voltage prgming, B3(PIC16) or B5(PIC18) used for I/O
#FUSES NOCPD                 	//No EE protection
#FUSES NOWRT                 	//Program memory not write protected
#FUSES RESERVED              	//Used to set the reserved FUSE bits

#use delay(clock=20000000,RESTART_WDT)
//#use rs232(baud=9600,parity=N,xmit=PIN_C6,rcv=PIN_C7,bits=8)

#define MODBUS_TYPE MODBUS_TYPE_MASTER
#define MODBUS_SERIAL_TYPE MODBUS_RTU
#define MODBUS_SERIAL_INT_SOURCE MODBUS_INT_RDA
#define MODBUS_SERIAL_RX_BUFFER_SIZE 64
#define MODBUS_SERIAL_RX_PIN PIN_C6
#define MODBUS_SERIAL_TX_PIN PIN_C7
#define MODBUS_SERIAL_ENABLE_PIN PIN_E0
#define MODBUS_SERIAL_RX_ENABLE PIN_E1
#define MODBUS_SERIAL_BAUD 9600
#define MODBUS_SLAVE_ADDRESS 0xF7
//#define MODBUS_ADDRESS 0xF7

#define LCD_BCKL	PIN_D3
#define LedRojo	PIN_B1
#define LedVerde	PIN_B2
#define Boton1		PIN_B5
#define Boton2		PIN_B4

#include "modbus.c"
#include "LCD.C"

//Funciones utilizadas
void start_init(void);
void write_holding_reg(void);
void read_holding_reg(void);
void processIO(void);

int16 contador=0;
int1 habilita;

#int_TIMER1
void timer1_isr()
{
	habilita=true;
}

//Ciclo principal
void main()
{
   setup_adc_ports(NO_ANALOGS);
   setup_adc(ADC_CLOCK_DIV_2);
   setup_spi(SPI_SS_DISABLED);
   setup_timer_0(RTCC_INTERNAL|RTCC_DIV_1);
   setup_timer_1(T1_INTERNAL|T1_DIV_BY_8);	//104mS
   setup_comparator(NC_NC_NC_NC);
	start_init();
	printf(lcd_putc, "\f");									//Borra el display
	printf(lcd_putc, "MS-%u-%lu",MODBUS_SLAVE_ADDRESS,contador);
	clear_interrupt(INT_TIMER1);
	enable_interrupts(INT_TIMER1);
	enable_interrupts(GLOBAL);
	while(true)
	{
		processIO();
	}
}

void start_init()
{
	modbus_init();
	lcd_init();
	output_high(LCD_BCKL);	//Enciende Backlight del LCD
}

void write_holding_reg(int16 Hreg_add, int16 Hreg_data)
{
   if(!(modbus_write_single_register(MODBUS_SLAVE_ADDRESS,Hreg_add,Hreg_data)))
   {
   	printf(lcd_putc,"\nWSHR-%lu-%lu       ",Hreg_add,Hreg_data);
      output_high(LedVerde);
     	output_low(LedRojo);
		delay_ms(1000);
   }
   else
   {
   	printf(lcd_putc,"\nException 0x%X      ",modbus_rx.error);
      output_low(LedVerde);
     	output_high(LedRojo);
		delay_ms(1000);
   }
}

void read_holding_reg(int Hreg_add, int Hreg_qty)
{
	int8 x,y;
	int16 z;

   if(!(modbus_read_holding_registers(MODBUS_SLAVE_ADDRESS,Hreg_add,Hreg_qty)))
   {
   	x=modbus_rx.data[1];
		y=modbus_rx.data[2];
		z=make16(x,y);
		
   	printf(lcd_putc,"\nRSHR-%u-%u-%lu       ",Hreg_add,Hreg_qty,z);
      output_high(LedVerde);
     	output_low(LedRojo);
   } 
   else
   {
   	printf(lcd_putc,"\nException 0x%X      ",modbus_rx.error);
      output_low(LedVerde);
     	output_high(LedRojo);
		delay_ms(1000);
   }
}


void processIO()
{
	read_holding_reg(3,2);                    
	delay_ms(50);
	if(habilita)
	{
		if(!input(Boton1))
		{
			write_holding_reg(3,contador);
		}
		if(!input(Boton2))
		{
			contador++;
			printf(lcd_putc, "\fMS-%u-%lu",MODBUS_SLAVE_ADDRESS,contador);
		}
		habilita=false;
	}
}
