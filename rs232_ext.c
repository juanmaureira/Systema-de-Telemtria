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

char data;
char BufferDataIn[MAXRECEIVE];
int contrcv;

#int_ext
void ext_isr(void){
int i;
	SwUartOff();
	if(kbhit(MODEM)){
		for(i=0;i<9;i++){	
			shift_right(&data,1,input(RX_EXT));
			delay_us(Time);
		}
	}
		BufferDataIn[contrcv++]=data;
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
	SwUartOn();
	enable_interrupts(global);
	ext_int_edge(H_to_L);	
}

