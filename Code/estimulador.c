#include <at89x52.h>
#include "lcd.h"
#include "keypad.h"

#define NUMTASKS 4
unsigned char taskRun[NUMTASKS] = {0,0,0,0};
void (*taskFcn[4])() = {taskBcycle, taskAcycle, taskReadKeypad, taskRefreshLcd};
unsigned char taskEnabled[NUMTASKS] = {1,0,1,1};
unsigned char taskOffset[NUMTASKS] = {0,1,0,1};
unsigned char taskInterval[NUMTASKS] = {250,250,50,50};

unsigned char cycle = 0, frec = 4, mode = 1;
unsigned char qseg = 0, pseg = 0, cseg = 0, cmin = 0;

void taskReadKeypad(){
	handleKeys();
	
	if(isDownNow(KEY_INCFREC) && frec < 150) frec++;		
	if(isDownNow(KEY_DECFREC) && frec > 4) frec--;
	
	if(isDownNow(KEY_MODO0)) cycle = 0;
	if(isDownNow(KEY_MODO1)) cycle = 2;
	if((!isDownNow(KEY_MODO0)) && !(isDownNow(KEY_MODO1))) cycle = 1;
	
	if(isDown(KEY_TEST)) mode = (mode+1)%2;
	
	taskInterval[1] = 1000/frec;
}

void taskRefreshLcd(){
	/*lcdGotoxy(0,0);
	lcdPutuc(taskInterval[1]);
	lcdPuts("ms");*/
	
	lcdGotoxy(14,0);
	lcdWrite(mode);
	
	if(cycle==0)
		lcdWrite('_');
	else
		lcdWrite(cycle+1);
	
	lcdGotoxy(0,0);
	lcdPutuc(frec);
	lcdPuts("Hz ");
	
	if(frec<10){
		lcdPuts("Relax  ");
	}else if(frec<20){
		lcdPuts("Aerob. ");
	}else if(frec<50){
		lcdPuts("Tono   ");
	}else if(frec<70){
		lcdPuts("Volum. ");
	}else if(frec<120){
		lcdPuts("Maxima ");
	}else if(frec<150){
		lcdPuts("Explos.");
	}
	
	lcdGotoxy(0,1);
	lcdPutuc(cmin);
	lcdWrite(':');
	lcdPutuc2(cseg);
	
	lcdGotoxy(15,1);
	lcdWrite(qseg+4);
}

#define NOP_2 nop \
			 nop
#define NOP_4 NOP_2 \
			 NOP_2
#define NOP_8 NOP_4 \
			 NOP_4
#define NOP_16 NOP_8 \
			 NOP_8
void taskAcycle(){	
	
	P3_2 = 0;
	
	__asm
		NOP_16
		NOP_16
		NOP_16
		NOP_16
		NOP_16
		NOP_16
	__endasm;
	
	P3_2 = 1;
	
	/*__asm
		NOP_16
		NOP_16
		NOP_16
		NOP_16
		NOP_16
		NOP_16
	__endasm;*/
	if(mode==1){
	P3_3 = 0;
	
	__asm
		NOP_16
		NOP_16
		NOP_16
		NOP_16
		NOP_16
		NOP_16
	__endasm;
	
	P3_3 = 1;	
	}
	
	/*P3_0 = 0;
	P3_1 = 1;
	#define PATA P3_2
	#include "seno.h"
	
	P3_0 = 1;
	P3_1 = 0;
	#define PATA P3_2
	#include "seno.h"*/
}

void taskBcycle(){
	qseg++;
	if(qseg == 4){
		qseg = 0;
		
		cseg++;
		
		if(cseg == 60){
			cseg = 0;
			cmin++;
		}		
	}
	
	pseg++;
	if(pseg <= 4){
		taskEnabled[1] = 1;
	}else{
		taskEnabled[1] = 0;
	}
	if(cycle == 0 && pseg > 8+3) pseg = 0;
	if(cycle == 1 && pseg > 6+3) pseg = 0;
	if(cycle == 2 && pseg > 4+3) pseg = 0;
}

#include "rtos.h"

void dispatcher(){	
	char i;
	while(1){
		i=0;
		while(i<NUMTASKS){
			if(taskRun[i]){
				taskRun[i]--;
				taskFcn[i]();
				i=0;
			}else{
				i++;
			}
		}
	}
}

void main(){
	lcdInit();
	rtosInit();
	dispatcher();
}
