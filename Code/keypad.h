#define KEY_INCFREC 	1
#define KEY_DECFREC 	2
#define KEY_TEST	 	4
#define KEY_MODO0		8
#define KEY_MODO1		16

#define KEY_PORT 		P1

#define isDownNow(x) ((keyNow & x) == 0)
#define isDown(x) (((keyNow & x) == 0)&&((keyPrev&x) != 0))

unsigned char keyNow=0xFF, keyPrev=0xFF;

void handleKeys(){
	keyPrev = keyNow;
	keyNow = KEY_PORT;	
}
