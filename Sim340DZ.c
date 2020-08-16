/*
 * File:   Yelcho_PIC32.c
 * Author: 2M Electronica Ltda.
 * Controlador para modulo SIM340DZ
 *
 */
#include "Pines.h"                      //Definiciones de pines
#include "Definiciones.h"               //Definiciones generales
#include "rs232_ext.c"
//#use rs232(baud=57600,xmit=,rcv=,stream=MODEM)

void configModem(void){
    char estado = 0;
    char tempstring1[50];

//    mU1RXIntEnable(0); //Deshabilita interrupcion en UART1
//    Debug_WriteString(">Configurando...\n\r");

    sprintf(tempstring1, "AT\r"); //Consulta estado
    Comm_WriteString(tempstring1);
    estado = RecibeU1(500);
    if(estado<1){
        Debug_WriteString("Encendiendo modem...\r");
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

char initGPRS(void){
    char estado = 0;
    char tempstring1[50];

	SwUartInit();

    mU1RXIntEnable(0); //Deshabilita interrupcion en UART1
    TipoResp = 4; //Indica a parser que respuestas son normales
 //   Debug_WriteString(">Iniciando GPRS...\n\r");
	fprintf(MODEM,">Iniciando GPRS...\n\r");
 //   Debug_WriteString("-Limpiando buffer\r"); //Limpia buffer de recepcion
 	fprintf(MODEM,"-Limpiando buffer\r");
 //   estado = RecibeU1(500);
 //    sprintf(tempstring1, "AT+CSTT=\"%s\",\"%s\",\"%s\" \r", apnGPRS, userGPRS, passGPRS); //Configura APN, Usuario y Password
 //    Comm_WriteString(tempstring1);
	fprintf(COM,"AT+CSTT=\"%s\",\"%s\",\"%s\" \r", apnGPRS, userGPRS, passGPRS);	
    Debug_WriteString(tempstring1);
 //   estado = RecibeU1(1000);
    if (estado == 1) {
        sprintf(tempstring1, "AT+CIICR\r"); //Activa conexion GPRS acorde a contexto establecido
        Comm_WriteString(tempstring1);
        Debug_WriteString(tempstring1);
        estado = RecibeU1(5000);
        if (estado == 1) {
            sprintf(tempstring1, "AT+CIFSR\r"); //Activa conexion GPRS acorde a contexto establecido
            Comm_WriteString(tempstring1);
            Debug_WriteString(tempstring1);
            estado = RecibeU1(5000);
            if (estado == 11) {
                TipoResp = 8;
                sprintf(tempstring1, "AT+CIPSTART=\"tcp\",\"%s\",\"%s\"\r", ipGPRS, portGPRS); //Inicia conexion GPRS a servidor
                Comm_WriteString(tempstring1);
                Debug_WriteString(tempstring1);
                estado = RecibeU1(30000); //Le da 30 segundos para conectar
            }
        }
    }

void UART1ClearErrors(void); //Borra cualquier error que se haya producido
	mU1RXClearIntFlag(); //Asegura no haya disparo en falso
	mU1RXIntEnable(1);
return(estado); //Retorna con valor de la ultima operacion
}