#define SCHED_US 1000

void schedIsr() __interrupt(TF2_VECTOR) __using(0){
	unsigned char i;
	for(i=0;i<NUMTASKS;i++){
		if(taskEnabled[i]){
			if(taskOffset[i] != 0){
				taskOffset[i]--;
			}else{
				taskRun[i]++;
				taskOffset[i] = taskInterval[i];
			}
		}
	}
	P3_4 = !P3_4;	
	TF2 = 0;
}

void rtosInit(){
	RCAP2L = (0xFFFF-2*SCHED_US) & 0xFF;
	RCAP2H = (0xFFFF-2*SCHED_US) >> 8; 
	
	IE = 0x80; ET2 = 1; TR2 = 1;
}