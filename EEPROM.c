void EEPROM_WriteByte(unsigned char data,unsigned char address){
	EEADRH=address>>8;
	EEADR=address;
	EEDATA=data;
	EPGD=0;
	CFGS=0;
	WREN=1;
	GIE=0;
	EECON2=55h;
	EECON2=0xAA;
	WR=1;
	//GIE=1;
	
	WREN=0;
}

unsigned char EEPROM_ReadByte(unsigned char address){
	EEADRH=address>>8;
	EEADR=address;
	EEPGD=0;
	CFGS=0;
	RD=1;
	unsigned char rtn=EEDATA;
	return rtn;
}