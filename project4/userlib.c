#include "userlib.h"

int print(char *word){
	int i ;

	for(i = 0;word[i];i++){
		interrupt(0x10,  0x0E * 256 + word[i],0,0,0);
	}

	return 1;
}
int readChar(){
	return interrupt(0x16, 0, 0, 0,0);
}
int readString(char *buffer){
	int i = 0;
	char c = readChar();
	
	while(c != 0x0D){
		//printString
		if(c != 0x08){
			interrupt(0x10,  0x0E * 256 + c,0,0,0);
			buffer[i] = c;
			i++;
		}else{
			if(i>0){
				
				interrupt (0x10, 0x0e * 256 + c, 0, 0, 0);
				i--;
				
			}
		}
		c = readChar();		
	}

	buffer[i] = 0x00;

	return 1;
}

int readSector(char *buffer, int absSector){
	int relSector = MOD(absSector, 18) + 1;
	int head = MOD(absSector / 18, 2);
	int track = absSector / 36;

	
	interrupt(0x13, 512+1, buffer, track*256+relSector, head*256);
	return 1;
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

int readfile(char *filename, char *buffer){
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
				print("No more sectors...");
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
		print("No space available");
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

int MOD(int number, int divisor){
	int result = number;
	while(result >= divisor){
		result = result - divisor;
	}
	return result;
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

