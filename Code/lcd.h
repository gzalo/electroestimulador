#define LCD_RS 1
#define LCD_EN 2
#define LCD_PORT P2

const unsigned char characters[8*8] = {
156,148,148,151,128,128,128,128, // MONOFASICO
156,148,148,149,133,133,135,128, // BIFASICO
128,128,128,159,159,159,159,128, // MEDIO
 0x00, 0x00, 0x0A, 0x1F, 0x1F, 0x0E, 0x04, 0x00,
142,149,149,149,145,145,142,128, // RELOJ: arriba
142,145,145,151,145,145,142,128, // RELOJ: derecha
142,145,145,149,149,149,142,128, // RELOJ: abajo
142,145,145,157,145,145,142,128, // RELOJ: izquierda
};

void lcdWait(){
	unsigned char i = 1;
	for(i=0;i<32;i++){
		__asm
			nop
		__endasm;
	}
}

void lcdSendCommand(char n){
	lcdWait();
	LCD_PORT = (n & 0xF0)|LCD_EN;
	LCD_PORT = (n & 0xF0);

	LCD_PORT = (n << 4)|LCD_EN;
	LCD_PORT = (n << 4);
}

void lcdWrite(char n){
	lcdWait();	
	LCD_PORT = (n & 0xF0)|LCD_EN|LCD_RS;
	LCD_PORT = (n & 0xF0)|LCD_RS;
	
	LCD_PORT = (n << 4)|LCD_EN|LCD_RS;
	LCD_PORT = (n << 4)|LCD_RS;
}

void putchar(char n){
	lcdWrite(n);
}

void lcdPuts(char* n){
	while(*n){
		lcdWrite(*n);		
		n++;
	}
}

void lcdPutuc(unsigned char n){
	unsigned char cent = n / 100;
	unsigned char dece = (n / 10)%10;
	unsigned char unid = n % 10;
	
	lcdWrite(cent+'0');
	lcdWrite(dece+'0');
	lcdWrite(unid+'0');
}

void lcdPutuc2(unsigned char n){
	unsigned char dece = (n / 10)%10;
	unsigned char unid = n % 10;
	
	lcdWrite(dece+'0');
	lcdWrite(unid+'0');
}

void lcdGotoxy(unsigned char x, unsigned char y){
	if(y==0) lcdSendCommand(0x80|(0 + x));
	if(y==1) lcdSendCommand(0x80|(64 + x));
	if(y==2) lcdSendCommand(0x80|(20 + x));
	if(y==3) lcdSendCommand(0x80|(84 + x));		
}

void lcdInit(){
	unsigned char n,j;

	for(n=0;n<250;n++)
		for(j=0;j<16;j++){
			__asm;
				nop
			__endasm;
		}
		
	LCD_PORT = 0x20|LCD_EN;
	LCD_PORT = 0x20;
	
	lcdSendCommand(0x0C);
	lcdSendCommand(0x06);
	lcdSendCommand(0x28);

	for(n=0;n<8*8;n++){
		lcdSendCommand(0x40 + n);
		lcdWrite(characters[n]);
	}
	
	lcdGotoxy(0,0);
	for(n=0;n<20*4;n++) lcdWrite(' ');
	lcdGotoxy(0,0);
	
}
