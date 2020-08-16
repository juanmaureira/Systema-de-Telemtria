void WriteWord(long int dir,long int data){
int hi, low;
		low=data;
		hi=data>>8;
		write_eeprom(dir,hi);
		delay_cycles(2);
		dir++;
		write_eeprom(dir,low);
		delay_cycles(2);
}

long int ReadWord(long int dir){
long int data;
int low,hi;
	hi=read_eeprom(dir);
	dir++;
	low=read_eeprom(dir);
	data=hi;
	data=(data<<8)|low;
return data;	
}

void WriteArray(long int dir, char *data, int size){
int i;
	for(i=0;i<size;i++,dir++){
		write_eeprom(dir,*(data+i));
		delay_cycles(5);
	}
	write_eeprom(dir,0x00);
	delay_cycles(5);
}

void ReadArray(long int dir,char *array1, int size){
int i;
	for(i=0;i<size;i++,dir++){
		*(array1+i)=read_eeprom(dir);
		 delay_ms(5);	
	}	
}