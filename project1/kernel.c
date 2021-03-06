void putChar(char *letter,int color, int x, int y);
void putStr(char *letter, int x, int y, int color);


int main(){
	putInMemory(0xB000, 0x8140, 'H');
	putInMemory(0xB000, 0x8141, 0x0F);
	putInMemory(0xB000, 0x8142, 'E');
	putInMemory(0xB000, 0x8143, 0x0F);
	putInMemory(0xB000, 0x8144, 'L');
	putInMemory(0xB000, 0x8145, 0x0F);
	putInMemory(0xB000, 0x8146, 'L');
	putInMemory(0xB000, 0x8147, 0x0F);
	putInMemory(0xB000, 0x8148, 'O');
	putInMemory(0xB000, 0x8149, 0x0F);
	putInMemory(0xB000, 0x8150, ' ');
	putInMemory(0xB000, 0x8151, 0x0F);
	putInMemory(0xB000, 0x8152, 'W');
	putInMemory(0xB000, 0x8153, 0x0F);
	putInMemory(0xB000, 0x8154, 'O');
	putInMemory(0xB000, 0x8155, 0x0F);
	putInMemory(0xB000, 0x8156, 'R');
	putInMemory(0xB000, 0x8157, 0x0F);
	putInMemory(0xB000, 0x8158, 'L');
	putInMemory(0xB000, 0x8159, 0x0F);
	putInMemory(0xB000, 0x8160, 'D');
	putInMemory(0xB000, 0x8161, 0x0F);

	putChar('H',0x4F, 12, 50);	
	putChar('E',0x4F, 12, 52);
	putChar('L',0x4F, 12, 54);
	putChar('L',0x4F, 12, 56);
	putChar('O',0x4F, 12, 58);
	putChar(' ',0x4F, 12, 60);
	putChar('W',0x4F, 12, 62);
	putChar('O',0x4F, 12, 64);
	putChar('R',0x4F, 12, 66);
	putChar('L',0x4F, 12, 68);
	putChar('D',0x4F, 12, 70);

	putStr("Hello World",0xF4, 25, 74);
	while(1 == 1){};
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
