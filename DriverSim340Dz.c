/**************************************************************************
					Driver Para Modem Sim340DZ
***************************************************************************
*/

unsigned char apnGPRS[30]; //APN del GPRS
unsigned char userGPRS[30]; //Usuario del GPRS
unsigned char passGPRS[20]; //Password del GPRS
unsigned char ipGPRS[16]; //IP del centro de control para GPRS
unsigned char portGPRS[10]; //Puerto GPRS remoto
unsigned char serie[10]; //Numero de serie poste

void configModem(void){
    char estado = 0;
    char tempstring1[50];

//    mU1RXIntEnable(0); //Deshabilita interrupcion en UART1
//    Debug_WriteString(">Configurando...\n\r");
	fprintf(COM,">Configurando...\n\r"):
//    sprintf(tempstring1, "AT\r"); //Consulta estado
//    Comm_WriteString(tempstring1);
	fprintf(COM,"AT\r"):
    estado = RcvModem(50);
    if(estado<1){
        fprintf(COM,"Encendiendo modem...\r");
        delay_ms(500); //Enciende el modulo
        mPORTGClearBits(PWRMDM);
        delay_ms(1500);
        mPORTGSetBits(PWRMDM);
        TipoResp = 12;
    }
    else Debug_WriteString("Modem encendido\r");

    sprintf(tempstring1, "AT\r"); //Consulta estado para debug
    Comm_WriteString(tempstring1);
    estado = RecibeU1(500);

    sprintf(tempstring1, "Clearing buffer...\r"); //Elimina eco
    Comm_WriteString(tempstring1);
    Debug_WriteString(tempstring1);
    estado = RecibeU1(6000);
    TipoResp = 4; //Indica a parser que respuestas son con 4 control char
    sprintf(tempstring1, "ATE0\r"); //Elimina eco
    Comm_WriteString(tempstring1);
    Debug_WriteString(tempstring1);
    estado = RecibeU1(500);
    sprintf(tempstring1, "AT+IPR=57600\r"); //Fija velocidad en 57600
    Comm_WriteString(tempstring1);
    Debug_WriteString(tempstring1);
    estado = RecibeU1(500);
    sprintf(tempstring1, "AT+CIURC=0\r"); //Elimina mensaje de entrada
    Comm_WriteString(tempstring1);
    Debug_WriteString(tempstring1);
    estado = RecibeU1(500);
    sprintf(tempstring1, "AT&C1\r"); //Linea DCD indica "En Linea"
    Comm_WriteString(tempstring1);
    Debug_WriteString(tempstring1);
    estado = RecibeU1(500);
    sprintf(tempstring1, "AT&D1\r"); //Linea DTR controla estado On-line/Comando
    Comm_WriteString(tempstring1);
    Debug_WriteString(tempstring1);
    estado = RecibeU1(500);
    sprintf(tempstring1, "AT+CIPSHUT\r"); //Asegura no contexto activo
    Comm_WriteString(tempstring1);
    Debug_WriteString(tempstring1);
    estado = RecibeU1(500);
    sprintf(tempstring1, "AT+CIPMODE=1\r"); //Activa modo transparente para conexiÛn
    Comm_WriteString(tempstring1);
    Debug_WriteString(tempstring1);
    estado = RecibeU1(500);
    TipoResp = 12;
    sprintf(tempstring1, "AT+CFUN=1\r"); //Activa modulo en maximo nivel
    Comm_WriteString(tempstring1);
    Debug_WriteString(tempstring1);
    estado = RecibeU1(3500);

    void UART1ClearErrors(void); //Borra cualquier error que se haya producido
    mU1RXClearIntFlag(); //Asegura no haya disparo en falso
    mU1RXIntEnable(1);
}