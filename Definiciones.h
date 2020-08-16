/*
 * File:   Definiciones.h
 * Author: 2M Electronica Ltda.
 * Definiciones globales de variables
 *
 */

/*Define UART que es Debug*/
#define Debug_WriteString Uart2_WriteString

/*Define UART que es Comm*/
#define Comm_WriteString Uart1_WriteString

/*Define UART que es Local*/
//#define ModbusSerial_WriteString Uart2_WriteString

/*Definiciones para Modbus_pic32*/
//#define MODBUS_TYPE MODBUS_TYPE_MASTER
//#define MODBUS_SERIAL_TYPE MODBUS_RTU
//#define MODBUS_SERIAL_INT_SOURCE MODBUS_INT_RDA
//#define MODBUS_SERIAL_RX_BUFFER_SIZE 64
//#define MODBUS_SERIAL_RX_PIN PIN_C6
//#define MODBUS_SERIAL_TX_PIN PIN_C7
//#define MODBUS_SERIAL_ENABLE_PIN PIN_E0
//#define MODBUS_SERIAL_RX_ENABLE PIN_E1
//#define MODBUS_SERIAL_BAUD 9600 //Velocidad de comunicaciones con esclavo
//#define MODBUS_SLAVE_ADDRESS 0x01 //Direccion del esclavo

/*Estado máquina para operación general, comentar los que no se usan*/
#define SM_CFG	00  //No esta configurado, no esta haciendo nada
#define SM_IDL  10  //Esperando tiempo reporte
#define SM_ERR	20  //Error en ultima operacion
#define SM_ALM	30  //Panel con alarma
#define SM_NOA	40  //Panel sin alarma
#define SM_RPT  50  //Tiempo de reporte
#define SM_ACT  60  //Activo
#define SM_NACT 70  //No activo

/*Estado de maquina para delay cooperativo*/
#define	SM_SET  110
#define	SM_WAIT 120
#define	SM_DONE	130
#define	SM_NOP	140

#define Estado1 (SMact==SM_ALM && SMant==SM_NOA)
#define Estado2 (SMact==SM_NOA && SMant==SM_ALM)

/*Generales*/
#define FOSC 80E6 //Frecuencia de operación



/*Variables interna de uso global*/
short TemStat; //Estado de maquina para control de temporizador cooperativo
short SelTimers; //Llave de habilitacion temporizadores 1, 2, 4, 8, 10, 20
int intVarCoop; //Variable interna rutina cooperativa (no tocar)
int delay; //Delay utilizado por rutina cooperativa 1ms tick (no tocar)
short delay1; //Variable para 1 uS tick (no tocar)
short delay2; //Delay utilizado por rutina no cooperativa 1ms tick (no tocar)
short GenStat; //Variable para registro eventos generales
short timeout; //Variable para control tiempo fuera Hadrware USART1 (no tocar)
short timeout1; //Variable para control tiempo fuera Hadrware USART2 (no tocar)
short timeout2; //Variable para control tiempo fuera Virtual USART (no tocar)
unsigned int AdoffSet; //Variable para lectura del ADC
int CNBControl; //Variable para lectura de pines con CN en puerto B
int CNDControl; //Variable para lectura de pines con CN en puerto D
char TipoResp; //Variable para recepcion de comandos con forma CR LF data1 CR LF CR LF data2 CR LF

//Variables especificas del firmware
int Ninfo; //Variable de uso generico
char SMant; //Estado de maquina anterior
char SMact; //Estado de maquina actual
char SMbkp; //Estado de maquina respaldo
int TReporte; //Variable para control reporte
char tempstring[30]; //String temporal para procesos

//Variables para GPRS
unsigned char apnGPRS[30]; //APN del GPRS
unsigned char userGPRS[30]; //Usuario del GPRS
unsigned char passGPRS[20]; //Password del GPRS
unsigned char ipGPRS[16]; //IP del centro de control para GPRS
unsigned char portGPRS[10]; //Puerto GPRS remoto
unsigned char serie[10]; //Numero de serie poste