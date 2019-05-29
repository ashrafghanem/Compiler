#ifndef __SYMBOL__H
#define __SYMBOL__H

#include<string>
#include<iostream>
#include <unordered_map>
using namespace std;

typedef enum{
	type_integer,
	type_string,
	type_boolean,
	type_float,
	type_none
} j_type;

/* printable versions of their names. */
static char *type_names[] = { "integer", "string", "boolean", "float", "none" };

typedef enum {
	ste_var, // a variable
	ste_const, //A constant
	ste_routine, //A routine
	ste_undefined // ste_entry
} ste_entry_type;

class SymbolTableEntry{
public:
	string name;
	ste_entry_type entry_type;

	SymbolTableEntry();
	SymbolTableEntry(string name, ste_entry_type type, int value); // constant
	SymbolTableEntry(string name, ste_entry_type type, j_type varType); // variable
	SymbolTableEntry(string name, ste_entry_type type, j_type returnType, int count); // routine     

	union{
		//l .for a variable record its type
		struct{
			j_type type;
		} var;
		// for a constant record its value
		struct{
			int value;
		} constant;
		/* for a routine, record formal parameters and result type */
		struct{
			// SteListCelll *formals;// will be defined later
			j_type result_type;
			int formalNumber;
		} routine;
	} f;
};

class SymbolTable{
public:
	SymbolTable *prev;
	unordered_map <string, SymbolTableEntry *> table;
	SymbolTable();
	SymbolTable(SymbolTable *s);
};

class Scope{
	SymbolTable *head;

public:
	SymbolTable *current;
	Scope();

	void enterScope();
	void exitScope();
	bool insert(string name, SymbolTableEntry *entry);
	SymbolTableEntry *find(string name);
	SymbolTableEntry *getFirstOcc(string name);	
};

#endif /* symbol_h */
