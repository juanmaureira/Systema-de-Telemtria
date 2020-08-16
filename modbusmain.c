#include <18f6722.h>
#use delay(clock=8000000)           	//Used to set the reserved FUSE bits

#define	MODEM_SERIAL_BAUD		9600

#use rs232(baud=MODEM_SERIAL_BAUD, xmit=PIN_C6,rcv=PIN_C7, parity=N, stream=MODEM)
#use rs232(baud=9600,xmit=PIN_B1,rcv=PIN_B0,parity=n,stream=COMV)

#FUSES INTRC_IO              	//Internal RC Osc, no CLKOUT                 	//Internal RC Osc
#FUSES NOWDT                 	//No Watch Dog Timer
//#FUSES HS                   	//High speed Osc (> 4mhz for PCM/PCH) (>10mhz for PCD)
#FUSES NOPROTECT             	//Code not protected from reading
#FUSES NOBROWNOUT            	//No brownout reset
	
#define MODBUS_SLAVE_ADDRESS 				0x01
#define	SEGREADPANEL						10
#define NUMREG								17
#define LCD_BCKL							PIN_D3
#define LedRojo								PIN_B1
#define LedVerde							PIN_B2
#define Boton1								PIN_B5
#define Boton2								PIN_B4
#define	DCD0								PIN_C0
#define	RTS0								PIN_C1
#define	DSR0								PIN_C2
#define CTS0								PIN_C3
#define	DTR0								PIN_C4
#define RI0									PIN_C5	


// Tablas con Valores para Paneles
//  Parametro                      EasyGen        Powerwizard 2.0    Panel6000
//0 Carga Kw                       50259          105                10
//1 Potencia Real Generada         50121-50122    106-107             0
//2 Potencia Aparente Generada     0              138-139             0
//3 Carga VA                       0              140                 0
//4 Potencia Reactiva Generada     50124-50125    141-142            16
//5 Percent Carga VA               50580-50059    143                 0
//6 Potencia KWh                   0              144-145             5
//7 Presion Aceite                 50254          217                 2
//8 Temp. Motor                    50248          219                 3
//9 Tension Bateria                50021          202                 1
//10 RPM                           50246-50247    0                   6
//11 Horas Operacion               50127          204                 4
//12 Dias para Mant                50126          210                 0
//13 Semanas para mant             0              299                 0
//14 Modo Operacion                50120          1053               72
//15 Nivel Comb.                   0              0                   0
//16 Estdo Equipo Yelcho           0              0                   0
//17 Alarmas                       0              0                   0
//18 Direccion Equipoc             00001          1                   0

            //       0     1     2     3     4     5     6     7     8     9    10    11    12    13    14    15    16    17    18    19    21
//Mapas de registros y su longitud en respectivos paneles (Orden es correspondencia de lectura)
int16 mapa_pw2[NUMREG] = {105, 106, 138, 140, 141, 143, 144, 200, 201, 202, 203, 204, 212, 299, 301, 805, 1053};
int16 long_pw2[NUMREG] = { 1,   2,   2,   1,   2,   1,   2,   1,   1,   1,   1,   2,   1,   1,   1,   1,    1};

int16 mapa_eg[NUMREG] = {50259, 50121, 0, 0, 50124, 50580, 0, 50254, 50248, 50021, 50246, 50127, 50126, 0, 50120, 0, 0};
int16 long_eg[NUMREG] = {1, 2, 1, 1, 2, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1};

int16 mapa_p6[NUMREG] = {10, 0, 0, 0, 16, 0, 5, 2, 3, 1, 6, 4, 0, 0, 72, 0, 0};
int16 long_p6[NUMREG] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

int16 mapa_pw1[1] = {1}; //Panel no tiene comunicaciones, solo entradas discretas
int16 long_pw1[1] = {1};

int16 BufferData[20];

int mapa_rg[1] = {1}; //Panel no se ha revisado
int long_rg[1] = {1};

//Funciones utilizadas
void start_init(void);
void write_holding_reg(void);
void read_holding_reg(void);
void processIO(void);

int16 contador=0;
int cont;
int mode=25;

#include "modbus.c"
#include "MenuInt_ext.c"
#include "ADH8066.c"


/**************************************************************************
			ECUACION Y CALCULO PARA TIEMPO DE TIMER1
ECUACION
	T=Tcm*Preescaler*(65536-CargaTMR1)
Donde:
	Tcm es el ciclo de máquina= 4/Fosc;
	T=Tiempo de desborde del TMR1
	
Para un tiempo de desborde de T=100ms y Preescaler de 1:8

	CargaTMR1 = 65536 - T/(Tcm*Preescaler)

	Con CargaTMR1 = 0
	-> T=104,8ms

	CargaTMR1 = 65536 - 0,1/((4/20M)*8) = 3036	T=100ms
**************************************************************************/
#int_TIMER1
void timer1_isr(void){
//		timeoutmodem++;
	//	habilita=true;	
		output_toggle(PIN_B2);
		//set_timer1(3036);
}

void LeePanel(void){
	read_holding_reg();
//	printf(lcd_putc, "\f");									//Borra el display
//	printf(lcd_putc, "MS-%u-%lu",MODBUS_SLAVE_ADDRESS,++contador);
}

//void MenuRegistros(void){
//	menu=false;
//	disable_interrupts(int_timer1);
//		fprintf(COMV,"***********************************************************\r");
//		fprintf(COMV,"Carga Kw=%f\r",(BufferData[0]*0.0078125));
//		fprintf(COMV,"Potencia Real Generada=%x%x\r",BufferData[1],BufferData[2]);
//		fprintf(COMV,"Potencia Aparente Generada=%x%x\r",BufferData[3],BufferData[4]);
//		fprintf(COMV,"Carga VA=%lu\r",BufferData[5]);
//		fprintf(COMV,"Potencia Reactiva Generada=%x%x\r",BufferData[6],BufferData[7]);	
//		fprintf(COMV,"Total Percent kVAr =%f\r",(BufferData[8]*0.0078125));
//		fprintf(COMV,"Potencia KWh=%x%x\r",BufferData[9],BufferData[10]);
//		fprintf(COMV,"Presion Aceite=%f[kPa]\r",(BufferData[11]*0.125));
//		fprintf(COMV,"Temp. Motor=%f[ºC]\r",(BufferData[12]*0.03125 ));
//		fprintf(COMV,"Tension Bateria=%f[V]\r",(BufferData[13]*0.05));
//		fprintf(COMV,"RPM =%f[rpm]\r",(BufferData[14]*0.125));
//		fprintf(COMV,"Horas Operacion=%x%x\r",BufferData[15],BufferData[16]);
//		fprintf(COMV,"Dias para Mant=%lu[Hrs]\r",BufferData[17]);
//		fprintf(COMV,"Semanas para mant=%lu[Semanas]\r",BufferData[18]);
//		fprintf(COMV,"Modo Operacion=%lu\r",BufferData[19]);
//	enable_interrupts(int_timer1);
//}

void InitEeprom(void){
int status;
int i;
char c;

delay_ms(250);
disable_interrupts(int_ext);

fprintf(COMV,"\r\nLeyendo datos de eeprom");

	for(i=0;i<31;i++){
		fputc('.',COMV);
		apnGPRS[i]=0x00;
		userGPRS[i]=0x00;
		passGPRS[i]=0x00;
		ipGPRS[i]=0x00;
		portGPRS[i]=0x00;
		delay_ms(125);
	}	
contrpt=read_eeprom(30);
delay_ms(5);
	if(contrpt>0&&contrpt<=30){
		status=0b00000000;
		ReadArray(31,apnGPRS,contrpt);
		delay_ms(5);
	}
	else
		status=0b00000001;

contrpt=read_eeprom(80);
delay_ms(5);

	if(contrpt>0&&contrpt<=30){
		status|=0b00000000;
		ReadArray(81,userGPRS,contrpt);
		delay_ms(5);
	}
	else
		status|=0b00000010;

contrpt=read_eeprom(120);
delay_ms(5);

	if(contrpt>0&&contrpt<=30){
		status|=0b00000000;
		ReadArray(121,passGPRS,contrpt);
		delay_ms(5);
	}
	else
		status|=0b00000100;

contrpt=read_eeprom(160);
delay_ms(5);

	if(contrpt>0&&contrpt<=30){
		status|=0b00000000;
		ReadArray(161,ipGPRS,contrpt);
		delay_ms(5);
	}
	else
		status|=0b00001000;

contrpt=read_eeprom(210);
delay_ms(5);

	if(contrpt>0&&contrpt<=30){
		status|=0b00000000;
		ReadArray(211,portGPRS,contrpt);
		delay_ms(5);
	}
	else
		status|=0b00010000;

panel=read_eeprom(0);
delay_ms(5);

	if(panel=='1'||panel=='2')
		status|=0b00000000;
	else
		status|=0b00100000;

modetransmicion=read_eeprom(20);		 
delay_ms(5);

	if(modetransmicion=='1'||modetransmicion=='2')
		status|=0b00000000;
	else
		status|=0b01000000;

timerptval=ReadWord(10);
delay_ms(5);

	if(timerptval>0&&timerptval<65536)
		status|=0b00000000;
	else
		status|=0b10000000;

	if(status==0b00000000){

		fprintf(COMV,"\r\nPanel=%c\r\n",panel);
		fprintf(COMV,"\r\nTiempo de reporte= %lu\r\n",timerptval); 
		fprintf(COMV,"\r\nModo de Transmision = %c\r\n",modetransmicion);			
		fprintf(COMV,"\r\nAPN: %s\r\n",apnGPRS);
		fprintf(COMV,"\r\nUSER: %s\r\n",userGPRS);
		fprintf(COMV,"\r\nPASWORD: %s\r\n",passGPRS);
		fprintf(COMV,"\r\nIP: %s\r\n",ipGPRS);
		fprintf(COMV,"\r\nPort: %s\r\n",portGPRS);
		
		fprintf(COMV,"\r\nDesea iniciar reporte con los parametros guardados [S/N] ");
			do{

				c=getc(COMV);

				switch(c){
					
					case 'S': case 's':
						 fprintf(COMV,"\r\nIniciando Reporte\r\n");
						 disable_interrupts(int_ext);
						 mode=2;
				    break;
					
					case 'N': case 'n':
						fprintf(COMV,"\r\nPresione cualquier tecla para configure nuevos parametros\r\n");
						enable_interrupts(int_ext);
						mode=0;
					break;
					
					default: 
						fprintf(COMV,"\r\nOpcion no valida\r\n");
					break;
				}

			}while(c!='N'&&c!='n'&&c!='S'&&c!='s');
	}
	else{
		fprintf(COMV,"\r\nFaltan parametros para iniciar equipo\r\n");
		enable_interrupts(int_ext);
	}
}

//Ciclo principal
void main(void){
boolean status;
	setup_adc_ports(NO_ANALOGS);
	setup_adc(ADC_CLOCK_DIV_2);
	setup_spi(SPI_SS_DISABLED);
	setup_timer_1(T1_INTERNAL|T1_DIV_BY_8);	
	set_timer1(3036);
	setup_comparator(NC_NC_NC_NC);
	start_init();
	clear_interrupt(INT_TIMER1);
		while(true){
			switch(mode){
				case 1: MenuUser(); break;
				case 2: InitModem(); break;
				case 3: 
						status=InitComunication();
						if(status)
							write_holding_reg();
						else
							fprintf(COMV,"ERROR DE CONEXION");
				break;
				default: break;
			}
		}
}

void start_init(void){
	setup_oscillator(OSC_8MHZ);
	enable_interrupts(global);
	modbus_init();
	InitEeprom();
}

void write_holding_reg(void){
	int16 Hreg_add=201;
	int16 Hreg_data=80;
	int i;
		for(i=0;i<9;i++)
			fprintf(MODEM,"\r\nMENSAJE DE PRUEBA: CHUPALO 8==============D");
	CloseComunication();		
		//if(!(modbus_write_single_register(MODBUS_SLAVE_ADDRESS,Hreg_add,Hreg_data))){
		//	printf(lcd_putc,"\nWSHR-%lu-%lu       ",Hreg_add,Hreg_data);
		//	output_high(LedVerde);
		//	output_low(LedRojo);
		//	delay_ms(1000);
		//}
//   else{
//   //	printf(lcd_putc,"\nException 0x%X      ",modbus_rx.error);
//    //  output_low(LedVerde);
//    // 	output_high(LedRojo);
//	//	delay_ms(1000);
//   }
}

void read_holding_reg(void){
	int8 x,y;
	int16 z;
	byte i,j,k;
	int16 temp=0;
	boolean StablishConection=true;
	disable_interrupts(int_timer1);
	k=0;
	for(i=0;i<NUMREG;i++){
			do{
				StablishConection=modbus_read_holding_registers(MODBUS_SLAVE_ADDRESS,(mapa_pw2[i]-1),long_pw2[i]);
			}while(StablishConection);
				if(!StablishConection){
					x=modbus_rx.data[1];
					y=modbus_rx.data[2];
					z=make16(x,y);
						for(j=0;j<long_pw2[i];j++){
							BufferData[k]=z;
							//fprintf(COM,"K=%d   Buffer=%lx  z=%lx  x=%x y=%x  REGISTRO=%lu  Registros=%lu\n\r",k,BufferData[k],z,x,y,mapa_pw2[i],long_pw2[i]);
							k++;	
						}
					//printf(lcd_putc, "\f");
					//printf(lcd_putc, "MS-%u-%lx",MODBUS_SLAVE_ADDRESS,z);
					//printf(lcd_putc,"\nRSHR-%lu-%lu-%lx",mapa_pw2[i]-1,long_pw2[i],BufferData[k]);
					//output_high(LedVerde);
					//output_low(LedRojo);
					//delay_ms(500);
				} 
			   else{
			   //	printf(lcd_putc,"\nException 0x%X      ",modbus_rx.error);
			   //   output_low(LedVerde);
			   //  	output_high(LedRojo);
			///		delay_ms(500);
			   }
		delay_ms(500);
   }
	enable_interrupts(int_timer1);
}