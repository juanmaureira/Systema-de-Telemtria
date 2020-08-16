#include <stdlib.h>
#include "write_eeprom_int.c"



char apnGPRS[30]; //APN del GPRS
char userGPRS[30]; //Usuario del GPRS
char passGPRS[30]; //Password del GPRS
char ipGPRS[30]; //IP del centro de control para GPRS
char portGPRS[30]; //Puerto GPRS remoto
char timerpt[5];
int panel;
int contrpt;
int16 timerptval;
int modetransmicion;

#int_ext
void isr_RB0(void){
	mode=1;	
}

void MenuUser(void){
char c=0x00;
int i;
	disable_interrupts(int_ext);
	fprintf(COMV,"\n\r----Menu servicio Yelcho Diperk---- \n\r");
	fprintf(COMV," A-Seleccionar Panel\n\r");
	fprintf(COMV," B-Configurar tiempo de reporte\n\r");
	fprintf(COMV," C-Seleccionar Modo de Transmision\n\r");
	fprintf(COMV," D-Configurar Parametros de Red\n\r");
	fprintf(COMV," E-Ver datos ingresados\n\r");
	fprintf(COMV," F-Borrar datos de memoria\n\r");
	fprintf(COMV," G-Tunel a Modem\n\r");
	fprintf(COMV," Q-Guardar y salir \n\r");
	do{
			c=getc(COMV);
		switch(c){
				case 'A': case 'a': 
		           fprintf(COMV,"\n\r 1-EasyGen\n\r");
		           fprintf(COMV,"\n\r 2-Power Wizard\n\r");
		           fprintf(COMV,"\n\r 3-P6000\n\r");
		           fprintf(COMV,"\n\r 4-RGK60\n\r");
					
					do{
						panel=getc(COMV);
						switch(panel){
							case '1': fprintf(COMV,"Seleccionado EasyGen\n\r"); break;
							case '2': fprintf(COMV,"Seleccionado Power Wizard\n\r"); break;
							case '3': fprintf(COMV,"Seleccionado P6000\n\r"); break;
							case '4': fprintf(COMV,"Seleccionado RGK60\n\r"); break;
							default:  fprintf(COMV,"Opcion no valida\n\r"); break;						 
						}
					}while(panel!='1'&&panel!='2'&&panel!='3'&&panel!='4');			
							write_eeprom(0,panel);
							delay_ms(5);
				break;
				
				case 'B': case 'b':
					contrpt=0;
					for(i=0;i<5;i++) timerpt[i]=0x00;
					fprintf(COMV,"\n\r Ingrese tiempo de reporte en segundos:  ");
					do{
						c=getc(COMV);
						putc(c,COMV);
							if(c!=0x08&&c!=0x0D)
								timerpt[contrpt++]=c;
							else{
								if(contrpt>0&&c!=0x0D)
									timerpt[--contrpt]=0x00;
							} 	
					}while(c!=0x0D);		
						timerptval=atol(timerpt);
						fprintf(COMV,"%lu",timerptval);
						WriteWord(10,timerptval);
				break;

				case 'C': case 'c': 
					fprintf(COMV,"\n\r1.- GPRS");
					fprintf(COMV,"\n\r2.- ETHERNET");
					do{
						modetransmicion=getc(COMV);
						switch(modetransmicion){
							case '1': fprintf(COMV,"\n\rSeleccion GPRS"); break;
							case '2': fprintf(COMV,"\n\rSeleccion ETHERNET"); break;
							default:  fprintf(COMV,"Opcion no valida\n\r"); break;
						}
					}while(modetransmicion!='1'&&modetransmicion!='2');
							write_eeprom(20,modetransmicion);
							delay_ms(5);
				break;
					/******************************************************/
				case 'D': case 'd':
					contrpt=0;
					for(i=0;i<30;i++) apnGPRS[i]=0x00; 
					fprintf(COMV,"\r\nIngrese nuevo APN:  ");
					do{
						c=getc(COMV);
						putc(c,COMV);
							if(c!=0x08&&c!=0x0D)
									apnGPRS[contrpt++]=c;
							else{
								if(contrpt>0&&c!=0x0D)
									apnGPRS[--contrpt]=0x00;
							}	
					}while(c!=0x0D);	
					fprintf(COMV,"\r\nConfirmar cambio [S/N]?:  ");
					do{
						c=getc(COMV);
						putc(c,COMV);	
					}while((c!='S'&&c!='s')&&(c!='N'&&c!='n'));
						switch(c){
							case 'S': case 's':
								//fprintf(COMV,"\r\nContador=%i",contrpt);
								fprintf(COMV,"\r\nRealizando cambios...");
							
								for(i=30;i<61;i++)
									write_eeprom(i,0x00);	
	
								delay_ms(5);
								Write_eeprom(30,contrpt);
								delay_ms(5);
								WriteArray(31,apnGPRS,contrpt);
								delay_ms(5);
								ReadArray(31,apnGPRS,contrpt);
								delay_ms(5);
								fprintf(COMV,"\r\nAPN Cambiado a: %s",apnGPRS);
							break;

							case 'N': case 'n': default:fprintf(COMV,"\r\n"); break;
								
						}
					/******************************************************/	
					contrpt=0;
					for(i=0;i<30;i++) userGPRS[i]=0x00; 
					fprintf(COMV,"\r\nIngrese nuevo USER:  ");
					do{
						c=getc(COMV);
						putc(c,COMV);
							if(c!=0x08&&c!=0x0D)
									userGPRS[contrpt++]=c;
							else{
								if(contrpt>0&&c!=0x0D)
									userGPRS[--contrpt]=0x00;
							}	
					}while(c!=0x0D);	
						fprintf(COMV,"\r\nConfirmar cambio [S/N]?:  ");
							do{
								c=getc(COMV);
								putc(c,COMV);	
							}while((c!='S'&&c!='s')&&(c!='N'&&c!='n'));	
						switch(c){
							case 'S': case 's':
										fprintf(COMV,"\r\nRealizando cambios...");

										for(i=80;i<121;i++)
											write_eeprom(i,0xFF);

										delay_ms(5);
										Write_eeprom(80,contrpt);	
										delay_ms(5);				
										WriteArray(81,userGPRS,contrpt);
										delay_ms(5);
										ReadArray(81,userGPRS,contrpt);
										delay_ms(5);
										fprintf(COMV,"\r\nUSER Cambiado a: %s",userGPRS);
							break;
							
							case 'N': case 'n': default:fprintf(COMV,"\r\n"); break;
						}	
					/******************************************************/
					contrpt=0;
					for(i=0;i<30;i++) passGPRS[i]=0x00; 
					fprintf(COMV,"\r\nIngrese nuevo PASS:  ");
					do{
						c=getc(COMV);
						putc(c,COMV);
							if(c!=0x08&&c!=0x0D)
									passGPRS[contrpt++]=c;
							else{
								if(contrpt>0&&c!=0x0D)
									passGPRS[--contrpt]=0x00;
							}	
					}while(c!=0x0D);	
					
						fprintf(COMV,"\r\nConfirmar cambio [S/N]?:  ");
							do{
								c=getc(COMV);
								putc(c,COMV);	
							}while((c!='S'&&c!='s')&&(c!='N'&&c!='n'));	
						switch(c){
							case 'S': case 's':
									fprintf(COMV,"\r\nRealizando cambios...");
	
									for(i=120;i<151;i++)
										write_eeprom(i,0xFF);
									
									delay_ms(5);
									write_eeprom(120,contrpt);
									delay_ms(5);	
									WriteArray(121,passGPRS,contrpt);
									delay_ms(5);
									ReadArray(121,passGPRS,contrpt);
									delay_ms(5);
									fprintf(COMV,"\r\nPASWORD Cambiado a: %s",passGPRS);
							break;

							case 'N': case 'n': default:fprintf(COMV,"\r\n"); break;
					  	}
					/******************************************************/
					contrpt=0;
					for(i=0;i<30;i++) ipGPRS[i]=0x00; 
					fprintf(COMV,"\n\rIngrese nueva IP:  ");
					do{
						c=getc(COMV);
						putc(c,COMV);
							if(c!=0x08&&c!=0x0D)
									ipGPRS[contrpt++]=c;
							else{
								if(contrpt>0&&c!=0x0D)
									ipGPRS[--contrpt]=0x00;
							}	
					}while(c!=0x0D);	

						fprintf(COMV,"\r\nConfirmar cambio [S/N]?:  ");
							do{
								c=getc(COMV);
								putc(c,COMV);	
							}while((c!='S'&&c!='s')&&(c!='N'&&c!='n'));	
						switch(c){
							case 'S': case 's':
									fprintf(COMV,"\r\nRealizando cambios...");
	
									for(i=160;i<191;i++)
										write_eeprom(i,0xFF);

									delay_ms(5);
									write_eeprom(160,contrpt);
									delay_ms(5);
									WriteArray(161,ipGPRS,contrpt);
									delay_ms(5);
									ReadArray(161,ipGPRS,contrpt);
									delay_ms(5);
									fprintf(COMV,"\r\nIP Cambiado a: %s",ipGPRS);
							break;
							case 'N': case 'n': default:fprintf(COMV,"\r\n"); break;
						}
					/******************************************************/
					contrpt=0;
					for(i=0;i<30;i++) portGPRS[i]=0x00; 
					fprintf(COMV,"\n\rIngrese nuevo Puerto:  ");
					do{
						c=getc(COMV);
						putc(c,COMV);
							if(c!=0x08)
									portGPRS[contrpt++]=c;
							else{
								if(contrpt>0)
									portGPRS[--contrpt]=0x00;
							}	
					}while(c!=0x0D);	

						fprintf(COMV,"\r\nConfirmar cambio [S/N]?:  ");
							do{
								c=getc(COMV);
								putc(c,COMV);	
							}while((c!='S'&&c!='s')&&(c!='N'&&c!='n'));	

						switch(c){
							case 'S': case 's':
									fprintf(COMV,"\r\nRealizando cambios...");
	
									for(i=210;i<241;i++)
										write_eeprom(i,0xFF);

									delay_ms(5);
									write_eeprom(210,contrpt);
									delay_ms(5);
									WriteArray(211,portGPRS,contrpt);
									delay_ms(5);
									ReadArray(211,portGPRS,contrpt);
									delay_ms(5);
									fprintf(COMV,"Puero cambiado a: %s",portGPRS);
							break;
							case 'N': case 'n': default:fprintf(COMV,"\r\n"); break;
						}		
				break;
						/******************************************************/
				case 'E': case 'e':
							fprintf(COMV,"\r\nLeyendo datos de eeprom...\r\n");
							
									contrpt=read_eeprom(30);
									delay_ms(5);
										if(contrpt>0&&contrpt<=30){
											ReadArray(31,apnGPRS,contrpt);
											delay_ms(5);
										}
									
									contrpt=read_eeprom(80);
									delay_ms(5);
									
										if(contrpt>0&&contrpt<=30){
											ReadArray(81,userGPRS,contrpt);
											delay_ms(5);
										}
									
									contrpt=read_eeprom(120);
									delay_ms(5);
									
										if(contrpt>0&&contrpt<=30){
											ReadArray(121,passGPRS,contrpt);
											delay_ms(5);
										}
									
									contrpt=read_eeprom(160);
									delay_ms(5);
									
										if(contrpt>0&&contrpt<=30){	
											ReadArray(161,ipGPRS,contrpt);
											delay_ms(5);
										}
									
									contrpt=read_eeprom(210);
									delay_ms(5);
									
										if(contrpt>0&&contrpt<=30){
											ReadArray(211,portGPRS,contrpt);
											delay_ms(5);
										}
									
									panel=read_eeprom(0);
									delay_ms(5);
									
									modetransmicion=read_eeprom(20);		 
									delay_ms(5);
									
									timerptval=ReadWord(10);
									delay_ms(5);

							fprintf(COMV,"\r\nPanel=%c\r\n",panel);
							fprintf(COMV,"\r\nTiempo de reporte= %lu\r\n",timerptval); 
							fprintf(COMV,"\r\nModo de Transmision = %c\r\n",modetransmicion);			
							fprintf(COMV,"\r\nAPN: %s\r\n",apnGPRS);
							fprintf(COMV,"\r\nUSER: %s\r\n",userGPRS);
							fprintf(COMV,"\r\nPASWORD: %s\r\n",passGPRS);
							fprintf(COMV,"\r\nIP: %s\r\n",ipGPRS);
							fprintf(COMV,"\r\nPort: %s\r\n",portGPRS);

				break;			

				case 'F': case'f':
						fprintf(COMV,"\r\nBorrando Memoria EEprom....");	
						for(i=0;i<255;i++){
							write_eeprom(i,0xFF);
						}
						fprintf(COMV,"\r\nMemoria borrada\r\n");
				break;
				
				case 'G': case 'g':
				//#use rs232(baud=57600, UART1, parity=N, stream=MODEM)
						fprintf(COMV,"\r\nIngrese comandos AT\r\n");
						fprintf(COMV,"\r\nPara salir de modo presione Q\r\n");
						
						do{
							do{
								c=getc(COMV);
								putc(c,MODEM);
								putc(c,COMV);
							}while(c!=0x0D);
							do{
								c=getc(MODEM);
								putc(c,COMV);
							}while(c!=0x0D);		
						}while(c!=0x1B);
				break;

				case 'Q': case'q':
						mode=0;
						enable_interrupts(int_ext);
						reset_cpu();
				break;

				default: fprintf(COMV,"Opcion no valida\n\r"); break;
		}
		c='Q';
	}while(c!='Q'&&c!='q');			
}
