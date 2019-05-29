#include "FileDescriptor.h"

FileDescriptor::FileDescriptor(const char *fileName) {
	fp.open(fileName, ios::in);
	char_number = 0;
	line_number = 1;
	buffer = new char[BUFFER_SIZE];
	fp.getline(buffer, BUFFER_SIZE);
	lineSize = strlen(buffer);
}
bool FileDescriptor::isOpen() {
	return fp.is_open();
}

char *FileDescriptor::getCurrLine() {
	return buffer;
}
int FileDescriptor::getLineNum() {
	return line_number;
}
int FileDescriptor::getCharNum() {
	return char_number;
}
FileDescriptor::~FileDescriptor() {
	fp.close();
	delete[]buffer;
}
bool FileDescriptor::isEOF() {
	return fp.eof();
}
char FileDescriptor::getChar() {
	char c = buffer[char_number];
	if (lineSize == char_number) {
		if (isEOF() == true)
			return EOF;
		else{
			fp.getline(buffer, BUFFER_SIZE);
			lineSize = strlen(buffer);
			char_number = 0;
			line_number++;
			return '\n';
		}
	}
	char_number++;
	return c;
}
void FileDescriptor::ungetChar(char c) {
	if (char_number == 0)
		return;
	char_number--;
	//buffer[char_number] = c;
}
void FileDescriptor::reportError(char * msg) {
	cout << msg << " on line:" << line_number << " of your code!" << endl;
	cout << buffer << endl;
	for (int i = 0; i < char_number - 1; i++) {
		if (buffer[i] == '\t')
			cout << '\t';
		else
			cout << " ";
	}
	cout << "^" << endl;
	exit(-1);
}
