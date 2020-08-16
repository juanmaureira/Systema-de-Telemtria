/*
			Driver para manejar ADH8066 via GPRS/TCP
*/
#include<string.h>
#define	ON_RST					PIN_F0					// PIN PARA ENCENDER MODEM

#define	MODEM_SERIAL_TIMEOUT	10						//en ms
#define MODEM_RCV_OFF() {disable_interrupts(INT_RDA);}

//#use rs232(baud=MODEM_SERIAL_BAUD, UART1, parity=N, stream=MODEM)

char rec_1[3] = {"OK"};
char rec_2[6] = {"ERROR"};
char rec_3[8] = {"CONNECT"};
char rec_4[11] = {"NO CARRIER"};
char rec_5[10] = {"IP STATUS"};
char rec_6[11] = {"IP INITIAL"};
char rec_7[8] = {"SHUT OK"};
char rec_8[7] = {"CLOSED"};
char rec_9[6] = {"DEACT"};
char rec_10[6] = {"READY"};
char rec_11[2] = {"."};
char rec_12[11] = {"POWER DOWN"};

boolean UART2=false;
char ModemBuffer[60];
int modem_in, modem_out;
long int timeoutmodem;
//boolean flagmodem=false;
//char userIP[30];
char c;

#int_timer0
void isr_timer0(void){
		timeoutmodem++;
}

#int_rda
void isr_rda(void){
	UART2=true;
	clear_interrupt(INT_RDA2);
}

void TimeoutOn(void){
	setup_timer_0(RTCC_INTERNAL|RTCC_DIV_32|RTCC_8_bit);
	enable_interrupts(int_timer0);
	timeoutmodem=0;
}

void TimeoutOff(void){
	disable_interrupts(int_timer0);
	timeoutmodem=0;
}

void ClearBuffer(void){
int i;
	for(i=0;i<60;i++)
		ModemBuffer[i]=0x00;
	modem_in=0;
	modem_out=0;		
}

void MODEM_RCV_ON(void){
	ClearBuffer();
	clear_interrupt(INT_RDA);
	enable_interrupts(int_rda);
}

int ProccessModem(long int timeout){
int respuesta=0;
	timeoutmodem=0;
	TimeoutOn();
		while(timeoutmodem<=timeout){
			if(kbhit(MODEM)){	
			c=getc(MODEM);
			putc(c,COMV);
				if((c==0x0A||c==0x0D)&&modem_in<60){
					ModemBuffer[modem_in++]=0x20;
				}
				else{
					if(modem_in<60){
						ModemBuffer[modem_in++]=c;
					}
				}
			}	
		}
	timeoutmodem=0;	
	respuesta = 0;
    if (strstr(ModemBuffer, rec_1)) respuesta = 1; //Ok
    if (strstr(ModemBuffer, rec_2)) respuesta = 2; //Error
    if (strstr(ModemBuffer, rec_3)) respuesta = 3; //Connect
    if (strstr(ModemBuffer, rec_4)) respuesta = 4; //No carrier
    if (strstr(ModemBuffer, rec_5)) respuesta = 5; //IP status
    if (strstr(ModemBuffer, rec_6)) respuesta = 6; //IP initial
    if (strstr(ModemBuffer, rec_7)) respuesta = 7; //Shut ok
    if (strstr(ModemBuffer, rec_8)) respuesta = 8; //Closed
    if (strstr(ModemBuffer, rec_9)) respuesta = 9; //Deact
    if (strstr(ModemBuffer, rec_10)) respuesta = 10; //Ready
    if (strstr(ModemBuffer, rec_11)) respuesta = 11; //Encontro IP
    if (strstr(ModemBuffer, rec_12)) respuesta = 12; //Respuesta MODEM APAGADO

//	fprintf(COMV,"\r\nRecibido=%s ",ModemBuffer);
	ClearBuffer();
//	fprintf(COMV,"\r\nresp=%i\r",respuesta);
	modem_in=0;
	TimeoutOff();
	timeoutmodem=0;	
return respuesta;
}

void InitModem(void){
int i,status;
boolean flagerror=false;
	fprintf(COMV,"\r\nInicializando Modem....");
	delay_ms(2000);	
	output_high(DSR0);
	output_high(CTS0);
	output_high(ON_RST);
	delay_ms(250);
	output_low(ON_RST);	
	delay_ms(2500);
	output_high(ON_RST);
	delay_ms(250);
	output_low(DSR0);
	output_low(CTS0);	
	delay_ms(150);
	fprintf(MODEM,"AT\r");
	status=ProccessModem(1000);	
		if(status==1){
			fprintf(COMV,"\r\n Modem Inicializado");
			mode=3;
		}
		else{
			mode=0;
			fprintf(COMV,"\r\nError en Inicializacion\r\n");
			delay_ms(1000);
			fprintf(COMV,"\r\nReiniciando...\r\n");
			fprintf(COMV,"Intento  ");
			for(i=1;i<4;i++){
				fprintf(COMV,"\b%d",i);
				fprintf(MODEM,"AT\r");
				status=ProccessModem(1000);
					if(status)
						break;
				if(i==3) flagerror=true; 						
			}
			if(flagerror){
				fprintf(COMV,"\r\nModem no detectado\r\n");
				mode=0;
			}	
			else{
				fprintf(COMV,"\r\nModem encendido\r\n");
				fprintf(MODEM,"AT\r");
				status=ProccessModem(1000);
				if(status){
					fprintf(COMV,"\r\nModem = OK\r\n");
					fprintf(MODEM,"ATE0\r");
					status=ProccessModem(2500);
					mode=3;	
					if(status)
						fprintf(COMV,"\r\nModem = OK\r\n");
					else{
						mode=0;
						fprintf(COMV,"\r\nFalla Modem\r\n");
					}
				}
				else{
					fprintf(COMV,"\r\nFalla Modem\r\n");
					mode=0;
				}
			}
		}
	//mode=0;	
	MODEM_RCV_OFF();
}

boolean InitComunication(void){
char status;
boolean conexion=false;	
	do{
	fprintf(MODEM,"AT+AIPDCONT=\"%s\",\"%s\",\"%s\"\r",apnGPRS,userGPRS,passGPRS);
	status=ProccessModem(1000);
	}while(status==1);
		switch(status){
			case 1:
				fprintf(COMV,"\r\nOK\r\n");
				delay_ms(500);
				fprintf(MODEM,"AT+AIPA=1\r");
				status=ProccessModem(2500);	
					switch(status){
						case 1:	
							fprintf(COMV,"\r\nOK\r\n");
							delay_ms(500);
							fprintf(MODEM,"AT+AIPO=1,,\"%s\",%s,0,,0,2\r",ipGPRS,portGPRS);
							status=ProccessModem(3500);
								switch(status){
									case 1:
										fprintf(COMV,"\r\nOK\r\n");
										fprintf(COMV,"\r\nConexion establecida\r\n");
										conexion=true; 
									break;	
									case 2:
										fprintf(COMV,"\r\nError de conexion\r\n");
										conexion=false;
									default: 
										fprintf(COMV,"\r\nError\r\n");
									break;	
								}
						break;
						case 2: 
							fprintf(COMV,"\r\nError de conexion\r\n");
						break;
						default: 
							fprintf(COMV,"\r\nError\r\n");
						break;
					}

			break;
			case 2:
				fprintf(COMV,"\r\nError de conexion\r\n");
			break;
			default: 
				fprintf(COMV,"Error");
			break; 
		}	
return conexion;
}

void CloseComunication(void){
	delay_ms(10000);
	fprintf(MODEM,"+++");
	delay_ms(10000);
	fprintf(MODEM,"+++");
	delay_ms(1000);	
	fprintf(MODEM,"AT+AIPC=1\r");
	delay_ms(1500);
	fprintf(MODEM,"AT+AIPA=0\r");
	delay_ms(1500);
}