#include"symbol.h"
using namespace std;

SymbolTableEntry::SymbolTableEntry(){
}
SymbolTableEntry::SymbolTableEntry(string name,ste_entry_type type,j_type varType){
	if(type==ste_var){
		entry_type=type;
		this->f.var.type=varType;
		this->name = name;
	}
	else {
		cout<<"type allowd to create is var"<<endl;
	}
}
SymbolTableEntry::SymbolTableEntry(string name,ste_entry_type type,int value){
	this->name = name;
	if(type == ste_const){
		this->entry_type = type;
		this->f.constant.value = value;
	}
	else{
		cout << "integer constant type expected" << endl;
	}	
}
SymbolTableEntry::SymbolTableEntry(string name,ste_entry_type type,j_type returnType,int count){
	entry_type=type;
	if(type==ste_routine){
		this->name = name;
		this->f.routine.result_type=returnType;
		this->f.routine.formalNumber=count;
	}
	else{
		cout<<"routine type expected"<<endl;
	}
}


SymbolTable::SymbolTable(){
	prev=NULL;
}
SymbolTable::SymbolTable(SymbolTable *s){
	prev=s;
}

Scope::Scope(){
	head=new SymbolTable();
	current=head;
}

void Scope::enterScope(){
	SymbolTable *newSymbolTable= new SymbolTable(current);
	current=newSymbolTable;
}
void Scope::exitScope(){
	if(current!=NULL)
		current=current->prev;
}
SymbolTableEntry * Scope::find(string name){
	unordered_map <string , SymbolTableEntry *>::iterator t =current->table.find(name);
	SymbolTableEntry * s=new SymbolTableEntry();
	if(t==current->table.end()){
		s->entry_type=ste_undefined;
		return s;
	}
	else 
		s=t->second;
	
	return s;
}
bool Scope::insert(string name,SymbolTableEntry * entry){
	if(current->table.find(name)==current->table.end()){
		current->table[name]=entry;
		return true;
	}
	else{
		return false;
	}
}

SymbolTableEntry * Scope::getFirstOcc(string name){
	SymbolTableEntry * entry=NULL;
	SymbolTableEntry * tempEntry=NULL;
	SymbolTable * currentOld=current;
	
	while(current!=NULL){
		tempEntry=find(name);
		if(tempEntry->entry_type!=ste_undefined){
			entry=tempEntry;
			current=currentOld;
			return entry;
		}
		exitScope();
	}
	current=currentOld;
	entry= new SymbolTableEntry();
	entry->entry_type = ste_undefined;
	return entry;
}