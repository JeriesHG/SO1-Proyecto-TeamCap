void putChar(char *letter,int color, int x, int y);
void putStr(char *letter, int x, int y, int color);
int printString(char *str);
int readChar();
int readString(char *buf);
int readSector(char *buf, int absSector);
int MOD(int number, int divisor);
int handleInterrupt21(int ax, int bx, int cx, int dx);

int main(){
	/**
	* 1. Imprimir a pantalla
	*/
	//printString("Hello World \0");

	/**
	* 2. Leer un caracter del teclado
	*/
	/*char *buf = "*****\0";
	char ch;
	int fullCh;
	
	printString("Type a char: \0");
	ch = readChar();
	buf[2] = ch;
	printString("Read: \0");
	printString(buf);
	printString("\n\r\0");*/

	/**
	* 3. Leer strings del disco
	*/
	char line[20];
	printString("Enter a line: \0");
	readString(line);
	printString("\n\0");
	printString(line);

	/**
	* 4. Leer sector del disco (Message.txt)
	*/
	/*char buffer[512];
	readSector(buffer,30);
	printString(buffer);

	/**
	* 5. Crear propio interrupt
	*/
	/*makeInterrupt21();
	interrupt(0x21,0x00,0,0,0);*/


	/**
	* 6. Manejar los interrupts dependiendo de lo que se envie
	*/
	/*char line[80];
	char ch[1];

	makeInterrupt21();

	// setup ISR for interrupt 0x21
	interrupt(0x21,0x00, "Type>\0", 0, 0);

	// display prompt
	interrupt(0x21,0x11,ch,0,0); //read char

	line[0] = ch[0];
	line[1] = 0x00;
	interrupt(0x21,0x00,line,0,0);// print string
	printString("\n\0");*/
	while(1 == 1){};
}
int handleInterrupt21(int ax, int bx, int cx, int dx){
	if(ax == 0x00){
		printString("PS\0");
		return printString(bx);
	} else if (ax == 0x11){
		printString("CH\0");
		bx = readChar();
		return 1;
	}else if (ax == 0x01){
		printString("RS\0");
		return readString(bx);
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

