#ifndef __PARSER__H
#define __PARSER__H

#include <stdio.h>
#include "scanner.h"
#include "ast.h"

class Parser {
public:
	Scope scope;
	SCANNER *scanner;

	Parser::Parser(string fileName) {
		scanner = new SCANNER(fileName.c_str());
		current = new TOKEN();
		current = scanner->Scan();
	}
	ast_list * parse_formals(int &);
	TOKEN * current;

	bool match(TOKEN * token, LEXEME_TYPE lexemeType);

	SymbolTableEntry * parse_id_var(j_type type, TOKEN * t);
	SymbolTableEntry * parse_id_cons(TOKEN * t, int value);
	SymbolTableEntry * parse_id_routine(TOKEN * t, j_type returnType, int count);

	j_type parse_type();
	
	ast_list * parse_formal_list(int &);
	ast_list * parse_arg_list(int & count);
	ast_list * parse_arg_list_bar(int & count);
	ast_list * parse_args(int & count);
	ast_list * parse_args_bar(int & count);
	ast_list * parse_formal_list_bar(int &);
	ast_list * parse_formals_bar(int &);
	ast_list * parse_var_decl_list();
	ast_list * parse_stmt_list();
	ast_list * parse_decl_list();

	AST * parse_decl();
	AST * parse_expr();
	AST * parse_expr_bar(AST * expr);
	AST * parse_rel();
	AST * parse_rel_bar(AST * arith);
	AST * parse_arith();
	AST * parse_arith_bar(AST * unary);
	AST * parse_unary();
	AST * parse_call_id(TOKEN * id);
	AST * parse_f();	
	AST * parse_block();	
	AST * parse_var_decl();	
	AST * parse_call_assign(TOKEN * id);
	AST * parse_ifstmt();
	AST * parse_stmt();
};

#endif
