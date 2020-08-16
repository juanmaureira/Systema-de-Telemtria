/*
 * File:   User.c
 * Author: 2M Electronica Ltda.
 * Diversas rutinas de retardo y transmisión
 *
 */


int RcvModem(int y) {
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


    //Variables utilizadas por la funcion
    char rcvchar = 0, Modem_RX[100], monitor[50];
    short control = 0, puntero = 0, respuesta = 0, temp = 0;
    timeout = 0; //Borra control tiempo fuera

    while (timeout < y) {
        if (DataRdyUART1()) {
            rcvchar = ReadUART1();
            if (((rcvchar == 0x0A) || (rcvchar == 0x0D)) && (puntero < 99)) {
                Modem_RX[puntero++] = 0x20;
                control++;
            }
            else {
                if (puntero < 99) {
                    Modem_RX[puntero++] = rcvchar;
                }
            }
            if (control == TipoResp)timeout = y; //Finaliza si llegan TipoResp char control
        }
    }
    Off_Tuart1();
    for(puntero=100-puntero;puntero<=99;puntero++){
        Modem_RX[puntero] = 0x00;
    }

    respuesta = 0;
    if (strstr(Modem_RX, rec_1)) respuesta = 1; //Ok
    if (strstr(Modem_RX, rec_2)) respuesta = 2; //Error
    if (strstr(Modem_RX, rec_3)) respuesta = 3; //Connect
    if (strstr(Modem_RX, rec_4)) respuesta = 4; //No carrier
    if (strstr(Modem_RX, rec_5)) respuesta = 5; //IP status
    if (strstr(Modem_RX, rec_6)) respuesta = 6; //IP initial
    if (strstr(Modem_RX, rec_7)) respuesta = 7; //Shut ok
    if (strstr(Modem_RX, rec_8)) respuesta = 8; //Closed
    if (strstr(Modem_RX, rec_9)) respuesta = 9; //Deact
    if (strstr(Modem_RX, rec_10)) respuesta = 10; //Ready
    if (strstr(Modem_RX, rec_11)) respuesta = 11; //Encontro IP
    if (strstr(Modem_RX, rec_12)) respuesta = 12; //Respuesta MODEM APAGADO

    sprintf(monitor, "%s\r", Modem_RX); //Envia dato procesado a debug
    Debug_WriteString(monitor);
    for (temp = 0; temp < 100; temp++)Modem_RX[temp] = 0x00; //Limpia buffer de recepcion

    sprintf(monitor, "Ans=%d\n\r", respuesta); //Envia dato procesado a debug
    Debug_WriteString(monitor);
    void UART1ClearErrors(void); //Borra cualquier error que se haya producido
    mU1RXClearIntFlag(); //Asegura no haya disparo en falso
    mU1RXIntEnable(1); //Habilita interrupcion USART1
    mPORTFClearBits(Led);
    return (respuesta);
}


