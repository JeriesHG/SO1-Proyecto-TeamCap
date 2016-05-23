void parseBuffer(char buffer[]);
void printScreen(char buffer[]);
int getBufferSize(char buffer[]);
int checkCommand(char command*, char buffer[] int index);
void retrieveParameter(char buffer[], char[] parameter, int index, int maxLimit);

main(){
	while(1){
		char buffer[128];

		interrupt(0x21,0x00,"\r\nShell> \0",0,0);
		interrupt(0x21,0x01,buffer,0,0);
		parseBuffer(buffer);
		
		// interrupt(0x21,0x02,buffer,0,0);
		//interrupt(0x21,0x00,"Unrecognized command\r\n\0",0,0);
	}	
} 

void parseBuffer(char buffer[]){
		int index = 0;
		int fileNameIndex;
		int test;
		int size = getBufferSize(buffer);
		char parameterName[6];
		char fileBuffer[13312];

		if(size <= 3){
			printScreen("\r\nShell> Invalid Command...\0");
		}else{

			if(checkCommand("type",buffer, index)){
				index = 4;
				if(buffer[index] == ' '){
					retrieveParameter(buffer, parameterName,index,6);
					interrupt(0x21, 0x03, parameterName, fileBuffer, 0);		
					if(fileBuffer[0] == 0x00){
						printScreen("\r\nShell> File not found...\0");
					}else{
						printScreen("\r\nShell> File Content: \0");
						interrupt(0x21, 0x00, fileBuffer, 0, 0);
					}			
				}else{
					printScreen("\r\nShell> Invalid type call...\0");
				}
			}else if(checkCommand("execute",buffer, index)){
				index = 7;
				if(buffer[index] == ' '){
						retrieveParameter(buffer, parameterName,index,6);
						interrupt(0x21, 0x04, parameterName,0x2000, test);

						if(test == -1){
							printScreen("\r\nShell> Invalid File name...\0");
						}
				}else{
					printScreen("\r\nShell> Invalid execute call...\0");
				}
			}

		}
		
}

void retrieveParameter(char buffer[], char parameter[], int index, int maxLimit){
	int paramIndex;
	for(paramIndex = 0;paramIndex<maxLimit;paramIndex++){
		parameter[paramIndex] = buffer[(index+1)+paramIndex];
	}
}

int checkCommand(char *command, char buffer[], int index){
	for(index = 0;index<getBufferSize(command);index++){
		if(command[index] == buffer[index]){
			return 1;
		}
	}

	return 0;
}

int getBufferSize(char buffer[]){
	int index = 0;
	int bools = 1;

	while(bools){
		if (buffer[index] == 0x0 || buffer[index] == "\0"){	
			bools = 0;
		}else{
			index++;
		}
	}	

	return index;
}
	
void printScreen(char buffer[]){
		interrupt(0x21,0x00,buffer,0,0);
}