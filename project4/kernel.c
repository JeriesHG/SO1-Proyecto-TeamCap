#include "userlib.h"

void putChar(char *letter,int color, int x, int y);
void putStr(char *letter, int x, int y, int color);
int handleInterrupt21(int ax, int bx, int cx, int dx);
int executeProgram(char* name, int segment);
void terminate();


int main(){
	
	/*
	* Step 1. Write Sector Test
	*/

	/*char buffer[512];
	buffer[0] = "H";
	writeSector(buffer,0);
	print("Sector Written");

	/*
	* Step 2. Delete File
	*/
	/*char buffer[13312];
	char buffer1[13312];
	makeInterrupt21();  
	interrupt(0x21, 0x03, "messag\0", buffer, 0);
	interrupt(0x21, 0x00, buffer, 0, 0);

	interrupt(0x21, 0x07, "messag\0",0x2000, 0);
	interrupt(0x21, 0x00, "File Deleted...\0", 0, 0);

	interrupt(0x21, 0x00, "Trying to call messag...\0", 0, 0);
	interrupt(0x21, 0x03, "messag\0", buffer1, 0);
	interrupt(0x21, 0x00, buffer1, 0, 0);*/

	/*
	* Step 3. Write File
	*/
	/*char buffer[13312];
	char buffer1[13312];
	makeInterrupt21();  
	//read and print
	interrupt(0x21, 0x03, "messag\0", buffer, 0);
	interrupt(0x21, 0x00, buffer, 0, 0);
	//Write file
	interrupt(0x21, 0x08, "test\0", buffer, 2);
	//read new file
	interrupt(0x21, 0x03, "test\0", buffer1, 0);
	//print buffer1
	interrupt(0x21, 0x00, buffer1, 0, 0);*/

	/*
	* Step 4. Shell commands
	*/
	makeInterrupt21();
	interrupt(0x21, 0x04, "shell\0",0x2000, 0);

	while(1 == 1){};
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
		dx = executeProgram(bx, cx);
 		return dx;
	}else if(ax == 0x05){
		terminate();
		return 1;
	} else if (ax == 0x07){
		return deleteFile(bx);
	}else if (ax == 0x08){
		return writeFile(bx, cx, dx);
	}else{
		print("Quit Interrupting\0");
		return -1;
	}
}

void terminate(){
	resetSegments();
	interrupt(0x21, 0x04, "shell\0",0x2000, 0);
}

int executeProgram(char* name, int segment){
	char buffer[13312];
	int i;

	if(segment == 0x0000 || segment == 0x1000){
		return -1; 
	}

	interrupt(0x21, 0x03, name, buffer, 0);

	for(i = 0;i<13312;i++){
		putInMemory(segment, i, buffer[i]);
	}

	return launchProgram(segment);
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

