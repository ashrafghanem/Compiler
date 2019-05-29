#include "scanner.h"

char *keyword[] = {
	"and", "begin", "bool", "by", "constant",
	"do", "else", "end", "false", "fi", "float", "for", "from",
	"function", "if", "integer", "not", "od", "or", "procedure",
	"program", "read", "return", "string", "then", "to", "true",
	"var", "while", "write"
};

SCANNER::SCANNER(const char *fileName){
	FileDescriptor *Fd = new FileDescriptor(fileName);
	fd = Fd;
}

TOKEN* SCANNER::Scan(){
	TOKEN* token;
	char c = fd->getChar();

	if (isStartID(c)){
		token = get_id(c);
		return token;
	}
	else if (c == '-'){
		char digit = fd->getChar();
		if (isdigit(digit) != 0){
			token = get_int(digit, '-');
			return token;
		}
		else {
			if (!isspace(digit) && (digit != EOF))
				fd->ungetChar(digit);
			token = new TOKEN();
			token->type = lx_minus;
			return token;
		}
	}
	else if (isdigit(c) != 0){
		token = new TOKEN();
		token = get_int(c, ' ');
		return token;
	}
	else if (c == '\"'){
		char letter = fd->getChar();
		token = get_string(letter);
		if (strcmp(token->str_ptr, "error") == 0){
			fd->reportError("Expected end quotation.");
			exit(-1);
		}
		return token;
	}

	else if (c == '#'){
		if (fd->getChar() == '#'){
			skip_comments();
		}
		else {
			fd->reportError("Expected another # sign to type a comment.");
			exit(-1);
		}
		return Scan();
	}

	else if (c == '('){
		token = new TOKEN();
		token->type = lx_lparen;
		return token;
	}
	else if (c == ')'){
		token = new TOKEN();
		token->type = lx_rparen;
		return token;
	}
	else if (c == ':'){
		token = new TOKEN();
		c = fd->getChar();
		if (c == '='){
			token->type = lx_colon_eq;
			return token;
		}
		else{
			token->type = lx_colon;
			fd->ungetChar(c);
			return token;
		}
	}
	else if (c == '+'){
		token = new TOKEN();
		token->type = lx_plus;
		return token;
	}

	else if (c == '*'){
		token = new TOKEN();
		token->type = lx_star;
		return token;
	}
	else if (c == '/'){
		token = new TOKEN();
		token->type = lx_slash;
		return token;
	}
	else if (c == '='){
		token = new TOKEN();
		token->type = lx_eq;
		return token;
	}
	else if (c == '!'){
		token = new TOKEN();
		c = fd->getChar();
		if (c == '='){
			token->type = lx_neq;
			return token;
		}
		else{
			fd->reportError("Expected =");
			exit(-1);
		}
	}
	else if (c == '<'){
		token = new TOKEN();
		c = fd->getChar();
		if (c == '='){
			token->type = lx_le;
			return token;
		}
		else{
			token->type = lx_lt;
			fd->ungetChar(c);
			return token;
		}
	}
	else if (c == '>'){
		token = new TOKEN();
		c = fd->getChar();
		if (c == '='){
			token->type = lx_ge;
			return token;
		}
		else{
			token->type = lx_gt;
			fd->ungetChar(c);
			return token;
		}
	}
	else if (c == '.'){
		token = new TOKEN();
		token->type = lx_dot;
		return token;
	}
	else if (c == ';'){
		token = new TOKEN();
		token->type = lx_semicolon;
		return token;
	}
	else if (c == '['){
		token = new TOKEN();
		token->type = lx_lsbracket;
		return token;
	}
	else if (c == ']'){
		token = new TOKEN();
		token->type = lx_rsbracket;
		return token;
	}
	else if (c == ','){
		token = new TOKEN();
		token->type = lx_comma;
		return token;
	}
	else if (c == '{'){
		token = new TOKEN();
		token->type = lx_lbracket;
		return token;
	}
	else if (c == '}'){
		token = new TOKEN();
		token->type = lx_rbracket;
		return token;
	}
	else if (isspace(c)){
		skip_spaces(c);
		fd->ungetChar(c);
		return Scan();
	}
	else if (c == EOF){
		token = new TOKEN();
		token->type = lx_eof;
		return token;
	}
	token = new TOKEN();
	token->type = illegal_token;
	return token;
}

void SCANNER::skip_spaces(char c){
	while (isspace(c = fd->getChar()));
}

bool SCANNER::isspace(char c){
	if ((c == ' ') || (c == '\n') || (c == '\t') || (c == '\f'))
		return true;
	else
		return false;
}
bool SCANNER::isStartID(char c){
	if (isalpha(c) || c == '_')
		return true;
	else
		return false;
}

TOKEN* SCANNER::get_int(char c, char sign){
	int i = 0;
	char *str_ptr = new char[1024];
	if (sign != ' '){
		str_ptr[0] = sign;
	}
	else{
		i = -1;
	}
	char nextChar = c;

	TOKEN* t = new TOKEN();
	do{
		str_ptr[++i] = nextChar;
	} while (isdigit(nextChar = fd->getChar()) != 0);

	if (isalpha(nextChar)){
		fd->reportError("Unrecognized integer!");
		exit(-1);
	}
	else if (nextChar == '.'){
		return get_float(fd->getChar(), str_ptr, i + 1);
	}
	else{
		str_ptr[++i] = '\0';

		if (!isspace(nextChar) && (nextChar != EOF))
			fd->ungetChar(nextChar);

		t->type = lx_integer;
		t->str_ptr = str_ptr;

		return t;
	}
}

TOKEN* SCANNER::get_float(char c, char *str, int i){
	str[i] = '.';
	char digit = c;
	int index = i;

	if (isdigit(c)){
		while (isdigit(digit) != 0){
			str[++index] = digit;
			digit = fd->getChar();
		}
		if (!isspace(digit) && (digit != EOF))
			fd->ungetChar(digit);

		if (isalpha(digit)){
			fd->reportError("Unrecognized float");
			exit(-1);
		}
		else{
			str[++index] = '\0';
			TOKEN *token = new TOKEN();
			token->str_ptr = str;
			token->type = lx_float;

			return token;
		}
	}
	else{
		fd->reportError("Expected integer value");
		exit(-1);
	}
}

TOKEN* SCANNER::get_string(char c){
	char *str_ptr = new char[1024];
	int i = -1;
	char nextChar = c;
	TOKEN* t = new TOKEN();
	while ((nextChar != '"'&& nextChar != '\n' && nextChar != EOF)){
		str_ptr[++i] = nextChar;
		nextChar = fd->getChar();
	}
	if (nextChar == '\"'){
		str_ptr[++i] = '\0';
		t->str_ptr = str_ptr;
		t->type = lx_string;
		return t;
	}
	t->str_ptr = "error";
	return t;
}
TOKEN* SCANNER::get_id(char c){
	char *str_ptr = new char[1024];
	int i = 0;
	str_ptr[0] = c;
	char nextChar = fd->getChar();
	TOKEN* t = new TOKEN();
	while (isalpha(nextChar) || nextChar == '_' || isdigit(nextChar)){
		str_ptr[++i] = nextChar;
		nextChar = fd->getChar();
	}

	str_ptr[++i] = '\0';

	if (!isspace(nextChar) && (nextChar != EOF))
		fd->ungetChar(nextChar);

	t->str_ptr = str_ptr;
	bool isKeyword = check_keyword(str_ptr);
	if (!isKeyword)
		t->type = lx_identifier;
	else{
		if (strcmp(str_ptr, "and") == 0)
			t->type = kw_and;
		else if (strcmp(str_ptr, "program") == 0)
			t->type = kw_program;
		else if (strcmp(str_ptr, "var") == 0)
			t->type = kw_var;
		else if (strcmp(str_ptr, "constant") == 0)
			t->type = kw_constant;
		else if (strcmp(str_ptr, "integer") == 0)
			t->type = kw_integer;
		else if (strcmp(str_ptr, "bool") == 0)
			t->type = kw_bool;
		else if (strcmp(str_ptr, "string") == 0)
			t->type = kw_string;
		else if (strcmp(str_ptr, "float") == 0)
			t->type = kw_float;
		else if (strcmp(str_ptr, "true") == 0)
			t->type = kw_true;
		else if (strcmp(str_ptr, "false") == 0)
			t->type = kw_false;
		else if (strcmp(str_ptr, "if") == 0)
			t->type = kw_if;
		else if (strcmp(str_ptr, "fi") == 0)
			t->type = kw_fi;
		else if (strcmp(str_ptr, "then") == 0)
			t->type = kw_then;
		else if (strcmp(str_ptr, "else") == 0)
			t->type = kw_else;
		else if (strcmp(str_ptr, "while") == 0)
			t->type = kw_while;
		else if (strcmp(str_ptr, "do") == 0)
			t->type = kw_do;
		else if (strcmp(str_ptr, "od") == 0)
			t->type = kw_od;
		else if (strcmp(str_ptr, "or") == 0)
			t->type = kw_or;
		else if (strcmp(str_ptr, "read") == 0)
			t->type = kw_read;
		else if (strcmp(str_ptr, "write") == 0)
			t->type = kw_write;
		else if (strcmp(str_ptr, "for") == 0)
			t->type = kw_for;
		else if (strcmp(str_ptr, "from") == 0)
			t->type = kw_from;
		else if (strcmp(str_ptr, "to") == 0)
			t->type = kw_to;
		else if (strcmp(str_ptr, "by") == 0)
			t->type = kw_by;
		else if (strcmp(str_ptr, "begin") == 0)
			t->type = kw_begin;
		else if (strcmp(str_ptr, "end") == 0)
			t->type = kw_end;
		else if (strcmp(str_ptr, "function") == 0)
			t->type = kw_function;
		else if (strcmp(str_ptr, "procedure") == 0)
			t->type = kw_procedure;
		else if (strcmp(str_ptr, "return") == 0)
			t->type = kw_return;
		else if (strcmp(str_ptr, "not") == 0)
			t->type = kw_not;
	}
	return t;
}
bool SCANNER::check_keyword(char* str){
	for (int i = 0; i < 30; i++){
		if (strcmp(str, keyword[i]) == 0){
			return true;
		}
	}
	return false;
}

void SCANNER::skip_comments(){
	while (true){
		char c = fd->getChar();
		while (c != '#' && c != '\n' && c != EOF)
			c = fd->getChar();

		if (c == '\n')
			return;
		if (c == EOF){
			return;
		}

		if (c == '#'){
			c = fd->getChar();
			if (c != '#'){
				fd->reportError("Unexpected end of comment.");
				exit(-1);
			}
			return;
		}
	}
}