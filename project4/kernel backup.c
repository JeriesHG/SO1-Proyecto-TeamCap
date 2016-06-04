void putChar(char *letter,int color, int x, int y);
void putStr(char *letter, int x, int y, int color);
int printString(char *str);
int readChar();
int readString(char *buf);
int readSector(char *buf, int absSector);
int writeSector(char *buffer, int sector);
int MOD(int number, int divisor);
int handleInterrupt21(int ax, int bx, int cx, int dx);
int readfile(char *filename, char *buf);
int deleteFile(char *fname);
int strcmp(const char* s1, const char* s2);
int executeProgram(char* name, int segment);
int writeFile(char *fname, char *buffer, int sectors);
void terminate();

int main(){
	
	/*
	* Step 1. Write Sector Test
	*/

	// char buffer[512];
	// buffer[0] = "H";
	// writeSector(buffer,0);
	// printString("Sector Written");

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

	while(1 == 1){};
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
	} else if (ax == 0x07){
		return deleteFile(bx);
	}else if (ax == 0x08){
		return writeFile(bx, cx, dx);
	}else{
		printString("Quit Interrupting\0");
		return -1;
	}
}

int writeFile(char *fname, char *buffer, int sectors){
	char map[512];
	char directory[512]; 
	char tempBuff[512];
	int spaceAvailable = 0;
	int nameIndex = 0;
	int spaceIndex = 0;
	int sectorNumber;
	int i = 0;
	int b = 0;


	readSector(map,1);
	readSector(directory,2);

		//Verificar si hay un espacio disponible
	for (spaceIndex=0;spaceIndex<16;spaceIndex++){
		if(directory[32+spaceIndex] == 0x00){
			spaceAvailable = 1;
			break;
		}	
	}

	if(spaceAvailable != 0){

		//Guardando nombre
		for(nameIndex = 0;nameIndex<6;nameIndex++){	
			directory[32*spaceIndex+nameIndex] = fname[nameIndex];		
		}

		for(i = 0;i<sectors;i++){

			sectorNumber = 0;

			while(map[sectorNumber] != 0x00){
				sectorNumber++;
			}

			if(sectorNumber == 26){
				printString("No more sectors...");
				return -2;
			}

			map[sectorNumber] = 0xFF;
			directory[32*spaceIndex+6+i] = sectorNumber;

			for(b = 0;b<512;b++){
				tempBuff[b] = buffer[b*(i+1)];
			}
			writeSector(tempBuff, sectorNumber);
		}
		

	}else{
		printString("No space available");
		return -1;
	}

	writeSector(map,1);
	writeSector(directory,2);

	return 1;


}

int deleteFile(char *fname){
	char map[512];
	char directory[512];
	int fileFound, index;

	readSector(map,1);
	readSector(directory,2);
	
	fileFound = strcmp(directory,fname);
	
	if(fileFound != 0){
		int j = 0;
		int i = 0;
		
		for (i=0;i<6;i++){
			directory[fileFound*32+i] = 0x00;
			
		}

		index = fileFound*32+6;
		for (j=0;j<26;j++){
			map[index+j] = 0x00;
			
		}

		writeSector(map,1);
		writeSector(directory,2);
		return 1;
	}else{
		return -1;
	}
}

int writeSector(char *buffer, int sector){
	int relSector = MOD(sector, 18) + 1;
	int head = MOD(sector / 18, 2);
	int track = sector / 36;
	int ah = 0x03;
	int al = 0x01;
	int ax = ah * 256 + al;
	
	interrupt(0x13, ax, buffer, track*256+relSector, head*256);
	return 1;
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

