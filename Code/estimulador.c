#include <at89x52.h>
#include "lcd.h"
#include "keypad.h"

#define NUMTASKS 4
unsigned char taskRun[NUMTASKS] = {0,0,0,0};
void (*taskFcn[4])() = {taskBcycle, taskAcycle, taskReadKeypad, taskRefreshLcd};
unsigned char taskEnabled[NUMTASKS] = {1,0,1,1};
unsigned char taskOffset[NUMTASKS] = {0,1,0,1};
unsigned char taskInterval[NUMTASKS] = {250,20,50,50};

unsigned char paused = 1, cursor = 0, contractionTime = 4, relaxationTime = 4, bypass = 0;
unsigned char quarterSeconds = 0, programQuarterSeconds = 0, timerSeconds = 0, timerMinutes = 0;

void taskReadKeypad(){
	handleKeys();

	if(isDown(KEY_CURSOR)) cursor = (cursor+1)%2;
	if(isDown(KEY_PAUSE)) paused = 1-paused;

	if(isDownNow(KEY_BYPASS)){
		bypass = 1;
		taskEnabled[1] = 1;
	} else if (bypass) {
		bypass = 0;
		taskEnabled[1] = 0;
	}
	
	if(cursor == 0){
		if(isDownNow(KEY_INC) && contractionTime < 40) contractionTime++;
		if(isDownNow(KEY_DEC) && contractionTime > 4) contractionTime--;
	} else if(cursor == 1){
		if(isDownNow(KEY_INC) && relaxationTime < 40) relaxationTime++;
		if(isDownNow(KEY_DEC) && relaxationTime > 4) relaxationTime--;
	}
}

void taskRefreshLcd(){
	lcdGotoxy(0,0);
	lcdPuts("C:");
	lcdPutuc(contractionTime);
	lcdPuts("/R:");
	lcdPutuc(relaxationTime);

	lcdGotoxy(15,0);
	lcdWrite(3);
	
	lcdGotoxy(0,1);
	lcdWrite(quarterSeconds+4);
	lcdWrite(' ');
	lcdPutuc2(timerMinutes);
	lcdWrite(':');
	lcdPutuc2(timerSeconds);

	lcdGotoxy(12,1);
	if(bypass){
		lcdPuts("Mica");
	} else {
		lcdPuts("    ");
	}
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
	unsigned char i;
	for(i=0;i<36;i++){
		P3_2 = 0; // 112
		__asm
			NOP_16
			NOP_16
			NOP_16
			NOP_16
			NOP_16
			NOP_16
			NOP_16
			NOP_16
			NOP_16
			NOP_16
			NOP_16
			NOP_16
			NOP_16
			NOP_16
		__endasm;
		
		P3_2 = 1; // 15
		__asm
			NOP_16
			NOP_16
		__endasm;

		P3_3 = 0; // 135
		__asm
			NOP_16
			NOP_16
			NOP_16
			NOP_16
			NOP_16
			NOP_16
			NOP_16
			NOP_16
			NOP_16
			NOP_16
			NOP_16
			NOP_16
			NOP_16
			NOP_16
			NOP_16
			NOP_16
			NOP_16
		__endasm;
		P3_3 = 1; // 19
		__asm
			NOP_16
			NOP_16
			NOP_4
			NOP_4
		__endasm;
	}
}

void taskBcycle(){
	if(paused){
		taskEnabled[1] = 0;
		return;
	}
	quarterSeconds++;
	if(quarterSeconds == 4){
		quarterSeconds = 0;
		
		timerSeconds++;
		
		if(timerSeconds == 60){
			timerSeconds = 0;
			timerMinutes++;
		}		
	}
	
	programQuarterSeconds++;
	if(programQuarterSeconds <= contractionTime){
		taskEnabled[1] = 1;
	} else {
		taskEnabled[1] = 0;
	}
	if(programQuarterSeconds > contractionTime + relaxationTime) programQuarterSeconds = 0;
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
