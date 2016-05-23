void putChar(char *letter,int color, int x, int y);
void putStr(char *letter, int x, int y, int color);
int printString(char *str);
int readChar();
int readString(char *buf);
int readSector(char *buf, int absSector);
int MOD(int number, int divisor);
int handleInterrupt21(int ax, int bx, int cx, int dx);
int readfile(char *filename, char *buf);
int strcmp(const char* s1, const char* s2);
int executeProgram(char* name, int segment);
void terminate();

int main(){
	
	/*char buffer[13312];
	makeInterrupt21();  


	interrupt(0x21, 0x03, "messag\0", buffer, 0);


	interrupt(0x21, 0x00, buffer, 0, 0); */

	//Step 2
	makeInterrupt21();
	interrupt(0x21, 0x04, "shell\0",0x2000, 0);
	//interrupt(0x21, 0x00, "done\n\r\0", 0, 0);
	while(1 == 1){};
}

void terminate(){
	resetSegments();
	printString("I am back...");
	while(1 == 1){}
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

	launchProgram(segment);

	return 1;
}

int readfile(char *filename, char *buffer){//24
	int fileFound, index;
	int sectors[27];
	int k = 0;
	int bufferAddress = 0;

	readSector(buffer,2);
	
	fileFound = strcmp(buffer,filename);
	
	if(fileFound != 0){
		int j = 0;
		index = fileFound*32+6;
		for (j=0;j<26;j++){
			sectors[j] = buffer[index+j];
			
		}

		sectors[26] = 0;
		
		while(sectors[k]!=0x0){
			readSector(buffer+bufferAddress,sectors[k]);
			bufferAddress += 512;
			k++;
		}

		return 1;
	}else{
		return -1;
	}



}

int strcmp(char* buffer, char* fileName){ 
	int i, j;

	int checkFound = 0;

	for (i = 0; i < 16; i++)
	{
		 /* If there is a file at the location */
		if (buffer[32*i] != 0x0){
			/* Check the first characters */
			for (j=0; j < 6; j++){

				if (buffer[j+32] == 0x0 || buffer[j+32] == '\r' || buffer[j+32] == '\n' || fileName[j] == 0x0 || fileName[j] == '\r' || fileName[j] == '\n'){
					break;
				} else if (buffer[j+32*i] == fileName[j]){
					checkFound = 1;	
				}else {
					checkFound = 0;
					break;
				}
				
			}
			if (checkFound == 1){
				return i;
			}
		}
	}
	if (checkFound == 0){
		for (i=0;i<13312;i++){
			buffer[i] = 0x0;
		}


		return 0;
	}


}


int handleInterrupt21(int ax, int bx, int cx, int dx){
	if(ax == 0x00){
		//printString("PS\0");
		return printString(bx);
	} else if (ax == 0x11){
		//printString("CH\0");
		bx = readChar();
		return 1;
	}else if (ax == 0x01){
	//	printString("RS\0");
		return readString(bx);
	}else if(ax == 0x03){
		//printString("RF\0");
		return readfile(bx,cx);
	}else if(ax == 0x04){
		dx = executeProgram(bx, cx);
		return dx;
	}else if(ax == 0x05){
		terminate();
		return 1;
	}else{
		printString("Quit Interrupting\0");
		return -1;
	}
}

//int interrupt(char irq, int ax, int bx, int cx, int dx);
int readSector(char *buffer, int absSector){
	int relSector = MOD(absSector, 18) + 1;
	int head = MOD(absSector / 18, 2);
	int track = absSector / 36;

	
	interrupt(0x13, 512+1, buffer, track*256+relSector, head*256);
	return 1;
}

int MOD(int number, int divisor){
	int result = number;
	while(result >= divisor){
		result = result - divisor;
	}
	return result;
}

int readString(char *buf){
	int i = 0;
	char c = readChar();
	
	while(c != 0x0D){
		//printString
		if(c != 0x08){
			interrupt(0x10,  0x0E * 256 + c,0,0,0);
			buf[i] = c;
			i++;
		}else{
			if(i>0){
				
				interrupt (0x10, 0x0e * 256 + c, 0, 0, 0);
				i--;
				
			}
		}
		c = readChar();		
	}

	buf[i] = 0x00;
}

int printString(char *word){
	int i ;
	for(i = 0;word[i];i++){
		interrupt(0x10,  0x0E * 256 + word[i],0,0,0);
	}
};

int readChar(){
	return interrupt(0x16, 0, 0, 0,0);
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

