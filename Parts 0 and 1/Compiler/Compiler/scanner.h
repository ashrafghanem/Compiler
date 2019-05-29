/* scanner .h*/
#ifndef __SCANNER__H
#define __SCANNER__H
#include "FileDescriptor.h"

//Token types enumeration
typedef enum
{
	/* Literals */
	lx_identifier, lx_integer, lx_string, lx_float,
	/* Keywords */
	kw_program,	kw_var, kw_constant, kw_integer, kw_bool, kw_string, kw_float,
	kw_true, kw_false, kw_if, kw_fi, kw_then, kw_else,	kw_while, kw_do, kw_od,
	kw_and, kw_or,	kw_read, kw_write,	kw_for, kw_from, kw_to, kw_by,
	kw_function, kw_procedure, kw_return, kw_not, kw_begin, kw_end,
	/* Operators */
	lx_lparen, lx_rparen, lx_lbracket, lx_rbracket, lx_lsbracket, lx_rsbracket,
	Ix_colon, lx_dot, lx_semicolon, lx_comma, Ix_colon_eq,
	lx_plus, lx_minus, lx_star, lx_slash,
	lx_eq, lx_neq, lx_lt, lx_le, lx_gt, lx_ge, lx_eof,
	/* Illegal */
	illegal_token
}LEXEME_TYPE;

int keys = 30; /* number of keywords */
char *keyword[] = {
	"and", "begin", "bool", "by", "constant",
	"do", "else", "end", "false", "fi", "float", "for", "from",
	"function", "if", "integer", "not", "od", "or", "procedure",
	"program", "read", "return", "string", "then", "to", "true",
	"var", "while", "write"
};

// Definition of TOKEN
class TOKEN
{
public:
	LEXEME_TYPE type;
	char *str_ptr;
	TOKEN(){
		str_ptr = new char[1024];
		str_ptr[0] = '\0';
	}
};


class SCANNER{
private:
	FileDescriptor *fd;
	void skip_comments();
	bool check_keyword(char* str);
	TOKEN *get_id(char c);
	TOKEN *get_string(char c);
	TOKEN *get_int(char c, char sign);
	TOKEN *get_float(char c, char* str,int i);
	void skip_spaces(char c);
	bool isspace(char c);
	bool isStartID(char c);

public:
	SCANNER(char *fileName);
	TOKEN *Scan();
};

#endif




