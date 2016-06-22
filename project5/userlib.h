#ifndef USER_LIB_H_
#define USER_LIB_H_

int print(char *str);
int printInt(int integer);
int readChar();
int readString(char *buf);
int readSector(char *buf, int absSector);
int writeSector(char *buffer, int sector);
int readfile(char *filename, char *buf);
int deleteFile(char *fname);
int writeFile(char *fname, char *buffer, int sectors);
int executeProgram(char* name, int segment);
int strcmp(const char* s1, const char* s2);
int kill(char *segment);
void showProcesses();
int atoi(char *e);

#endif