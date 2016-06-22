#define MAIN

#include "userlib.h"
#include "proc.h"


void putChar(char *letter,int color, int x, int y);
void putStr(char *letter, int x, int y, int color);
int handleInterrupt21(int ax, int bx, int cx, int dx);
int executeProgram(char* name);
void terminate();
void handleTimerInterrupt(int segment, int stackPointer);
void returnFromTimer(int segment, int stackPointer);
void kStrCopy(char *src, char *dest, int len);
void yield();


int main(){
	/*
	Step 1 - Timer Interrupt Basico
	*/
	/*makeTimerInterrupt();
	handleTimerInterrupt(0x3000, 1);*/

	//executeProgram Test (After changing implementation)
	//makeInterrupt21();
	//interrupt(0x21, 0x04, "shell\0",0, 0);

	/*
	Step 2 - Implementing Multiprogramming
	*/
	makeInterrupt21();
	makeTimerInterrupt();
	initializeProcStructures();

	interrupt(0x21, 0x04, "shell\0",0, 0);
	while(1 == 1){};
}

void yield(){
	struct PCB *pcb;
	char name[7];

	setKernelDataSegment();
	// kStrCopy((*running).name, name, 7);
	// pcb = running;
	// releasePCB(running);
	// (*pcb).state = READY;
	// kStrCopy(name, (*running).name, 7);
	// addToReady(pcb);
	addToReady(running);
	restoreDataSegment();
}

void handleTimerInterrupt(int segment, int stackPointer){
	struct PCB *pcb;

	setKernelDataSegment();

	(*running).stackPointer = stackPointer;
	(*running).segment = segment;

	if((*running).state != DEFUNCT && (*running).name != "idle\0"){
		(*running).state = READY;
		addToReady(running);
	}
	
	pcb = removeFromReady();

	if(pcb == NULL){
		running = &idleProc;
	}else{
		(*pcb).state = RUNNING;
		running = pcb;
	}

	returnFromTimer((*running).segment,(*running).stackPointer);
	restoreDataSegment();
}

int handleInterrupt21(int ax, int bx, int cx, int dx){
	if(ax == 0x00){
		return print(bx);
	} else if (ax == 0x11){
		bx = readChar();
		return 1;
	}else if (ax == 0x01){
		return readString(bx);
	}else if(ax == 0x03){
		return readfile(bx,cx);
	}else if(ax == 0x04){
		dx = executeProgram(bx);
		return dx;
	}else if(ax == 0x05){
		terminate();
		return 1;
	} else if (ax == 0x07){
		return deleteFile(bx);
	}else if (ax == 0x08){
		return writeFile(bx, cx, dx);
	}else if (ax == 0x09){
		yield();
		return 1;
	}else if(ax == 0x0A){
		showProcesses();
		return 1;
	}else if(ax == 0x0B){
		return kill(bx);
	}else{
		print("Quit Interrupting\0");
		return -1;
	}
}

void terminate(){
	resetSegments();
	setKernelDataSegment();
	releasePCB(running);
	restoreDataSegment();
	setKernelDataSegment();
	releaseMemorySegment((*running).segment);
	restoreDataSegment();
	while(1 == 1){}
}

int executeProgram(char* name){
	char buffer[13312];
	int i;
	int segment;
	struct PCB *pcb;

	setKernelDataSegment();
	segment = getFreeMemorySegment();
	restoreDataSegment();

	if(segment == -1){
		print("No Memory Available\0");
		return -2;
	}

	if(segment == 0x0000 || segment == 0x1000){
		print("Invalid Segment\0");
		return -1; 
	}

	interrupt(0x21, 0x03, name, buffer, 0);

	if(buffer[0] == 0x00){
		print("File not found\0");
		return -1;
	}

	for(i = 0;i<13312;i++){
		putInMemory(segment, i, buffer[i]);
	}

	setKernelDataSegment();
	pcb = getFreePCB();
	restoreDataSegment();

	if(pcb == NULL){
		return -2;
	}

	kStrCopy(name,(*pcb).name, 5);

	setKernelDataSegment();
	(*pcb).segment = segment;
	(*pcb).stackPointer = 0xFF00;
	addToReady(pcb);
	restoreDataSegment();
	
	initializeProgram(segment);

	return 1;
}

/* kStrCopy(char *src, char *dest, int len) copy at most len
 * characters from src which is addressed relative to thecurrent 
 * data segment into dest which is addressed relative to the 
 * kernel's data segment(0x1000).
 */
void kStrCopy(char *src, char *dest, int len) {
	int i=0; 
	for (i=0; i<len; i++) {
		putInMemory(0x1000, dest+i, src[i]);
		if (src[i] == 0x00) {
			return;
		}
	}
}

void putChar(char letter,int color, int x, int y){
	int row = 0x8000 + ((80 * (x - 1)) * 2);
	putInMemory(0xB000, y + row+1, color);
	putInMemory(0xB000, y +  row, letter);
};   


void putStr(char *letter,int color, int x, int y){

	int row = 0x8000 + ((80 * (x - 1)) * 2);
	char *character = letter;

	while(*character != '\0') {
		putInMemory(0xB000, y + row+1, color);
		putInMemory(0xB000, y +  row, *(character));
		row = row + 2;
		character++;
	}
};

