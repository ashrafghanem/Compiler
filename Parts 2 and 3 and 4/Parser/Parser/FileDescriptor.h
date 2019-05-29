#ifndef __FD__H
#define __FD__H

#include<iostream>
#include<fstream>
#include<string.h>
using namespace std;

#define BUFFER_SIZE 255

class  FileDescriptor {
private:
	ifstream fp;
	int line_number;
	int lineSize;
	int char_number;
	char *buffer;
public:
	bool isEOF();
	bool isOpen();
	char *getCurrLine();
	int getLineNum();
	int getCharNum();
	FileDescriptor(const char *fileName);
	~FileDescriptor();
	char getChar();
	void reportError(char *msg);
	void  ungetChar(char c);
};
#endif
