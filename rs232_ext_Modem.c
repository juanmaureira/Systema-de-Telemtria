/*
					Rutina para resepcion serial 
					  por interrupcion externa 

	Para 9600 baudios Time = 100 us
	Para 57600 baudios	Time = 15 us
	Calculado para Cristal de 20Mhz
							
*/
#define	RX_EXT			PIN_B0
#define	TX_EXT			PIN_B1
#define	EXTBAUDRATE		57600
#define Time			15
#define	MAXRECEIVE		64

#use rs232(baud=EXTBAUDRATE,rcv=RX_EXT,xmit=TX_EXT,stream=MODEM)

void SwUartOn(void);
void SwUartOff(void);
void SwUartInit(void);
void RcvModem(void);

char data;
char BufferDataIn[MAXRECEIVE];
int contrcv;
boolean FlagRcv=false;
int ContTimer2;

#int_timer2
void isr_timer2(void){  /// Cada 10ms
	ContTimer2++;
}

#int_ext
void ext_isr(void){
int i,ContStopModem=0;
	SwUartOff();
	if(kbhit(MODEM)){
		for(i=0;i<9;i++){	
			shift_right(&data,1,input(RX_EXT));
			delay_us(Time);
		}
	}
	if((data==0x0A||data==0x0D)&&contrcv<=MAXRECEIVE){
		BufferDataIn[contrcv++]=0x20;
		ContStopModem++;
			if(ContStopModem==2)
				FlagRcv=true;
			else
				FlagRcv=false;
	}
	else{
		if(contrcv<MAXRECEIVE)
			BufferDataIn[contrcv++]=data;
	}
		if(contrcv>=MAXRECEIVE) contrcv=0;
	SwUartOn();
}

void SwUartOn(void){
	enable_interrupts(int_ext);
}

void SwUartOff(void){
	disable_interrupts(int_ext);
}

void SwUartInit(void){
int i;
	for(i=0;i<MAXRECEIVE;i++)
		BufferDataIn[i]=0x00;

	SwUartOn();
	setup_timer_2(T2_DIV_BY_16,196,16);
	enable_interrupts(global);
	ext_int_edge(H_to_L);	
}

int RcvModem(int timeout){
int respuesta,i;
    //Cadenas que busca en respuesta de modem
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
	enable_interrupts(int_timer2);
		while(FlagRcv&&timeout<ContTimer2);
	ContTimer2=0;	
	disable_interrupts(int_timer2);	

    respuesta = 0;

    if (strstr(BufferDataIn, rec_1)) respuesta = 1; //Ok
    if (strstr(BufferDataIn, rec_2)) respuesta = 2; //Error
    if (strstr(BufferDataIn, rec_3)) respuesta = 3; //Connect
    if (strstr(BufferDataIn, rec_4)) respuesta = 4; //No carrier
    if (strstr(BufferDataIn, rec_5)) respuesta = 5; //IP status
    if (strstr(BufferDataIn, rec_6)) respuesta = 6; //IP initial
    if (strstr(BufferDataIn, rec_7)) respuesta = 7; //Shut ok
    if (strstr(BufferDataIn, rec_8)) respuesta = 8; //Closed
    if (strstr(BufferDataIn, rec_9)) respuesta = 9; //Deact
    if (strstr(BufferDataIn, rec_10)) respuesta = 10; //Ready
    if (strstr(BufferDataIn, rec_11)) respuesta = 11; //Encontro IP
    if (strstr(BufferDataIn, rec_12)) respuesta = 12; //Respuesta MODEM APAGADO	
	
	fprintf(COM,"%s",BufferDataIn);

	for(i=0;i<MAXRECEIVE;i++)
		BufferDataIn[i]=0x00;

return respuesta;
}



