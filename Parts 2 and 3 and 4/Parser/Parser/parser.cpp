#include "parser.h"
using namespace std;

bool Parser::match(TOKEN * token, LEXEME_TYPE lexemeType){
	if (token->type == lexemeType){
		current = scanner->Scan();
		return true;
	}
	return false;
}
SymbolTableEntry * Parser::parse_id_var(j_type type, TOKEN * t) {
	SymbolTableEntry * entry;
	entry = new SymbolTableEntry(t->str_ptr, ste_var, type);
	if (!scope.insert(t->str_ptr, entry)){
		scanner->fd->reportError("Already Declared Variable!");
		exit(-1);
	}
	return entry;
}
SymbolTableEntry * Parser::parse_id_cons(TOKEN * t, int value) {
	SymbolTableEntry * entry;
	entry = new SymbolTableEntry(t->str_ptr, ste_const, value);
	if (!scope.insert(t->str_ptr, entry)){
		scanner->fd->reportError("Already Declared Constant!");
		exit(-1);
	}
	return entry;
}
SymbolTableEntry * Parser::parse_id_routine(TOKEN * t, j_type returnType, int count) {
	SymbolTableEntry * entry;
	entry = new SymbolTableEntry(t->str_ptr, ste_routine, returnType, count);
	if (!scope.insert(t->str_ptr, entry)){
		scanner->fd->reportError("Already Declared Function!");
		exit(-1);
	}
	return entry;
}
j_type Parser::parse_type() {
	if (match(current, kw_integer)) {
		return type_integer;
	}
	else if (match(current, kw_string)) {
		return type_string;
	}
	else if (match(current, kw_bool)) {
		return type_boolean;
	}
	else {
		scanner->fd->reportError("Unexpected type!");
	}
}
ast_list * Parser::parse_decl_list(){
	ast_list * decl_list = NULL;

	AST * decl = parse_decl();
	if (match(current, lx_semicolon)){
		decl_list = parse_decl_list();
		decl_list = const_ast(decl, decl_list);
	}
	return decl_list;
}
AST * Parser::parse_decl(){
	AST * dec = parse_var_decl();

	// if dec is variable or constant
	if (dec != NULL)
		return dec;

	if (match(current, kw_function)){
		TOKEN * id = current;
		if (match(id, lx_identifier)) {
			int count = 0;
			scope.enterScope();

			// should be list of STEs
			ast_list *list_formal = parse_formal_list(count);
			if (match(current, lx_colon)) {
				j_type returnType = parse_type();
				AST * body = parse_block();
				scope.exitScope();
				SymbolTableEntry * entry = parse_id_routine(id, returnType, count);
				return make_ast_node(5, ast_routine_decl, entry, list_formal, returnType, body);
			}
			else {
				scanner->fd->reportError("Expected Colon!");
			}
		}
		else {
			scanner->fd->reportError("Expected Identifier!");
		}
	}
	else if (match(current, kw_procedure)){
		TOKEN * id = current;
		if (match(id, lx_identifier)) {
			int count = 0;
			scope.enterScope();
			ast_list * list_formal = parse_formal_list(count);
			j_type returnType = type_none;
			AST * body = parse_block();
			scope.exitScope();
			SymbolTableEntry * entry = parse_id_routine(id, returnType, count);
			return make_ast_node(5, ast_routine_decl, entry, list_formal, returnType, body);
		}
		else {
			scanner->fd->reportError("Expected Identifier!");
		}
	}
	return NULL;
}
ast_list * Parser::parse_formal_list(int &count) {
	if (match(current, lx_lparen)) {
		return parse_formal_list_bar(count);
	}
	else
		scanner->fd->reportError("Expected Left Parantheses!");

	return NULL;
}
ast_list * Parser::parse_formal_list_bar(int &count) {
	if (match(current, lx_rparen)){
		return NULL;
	}
	ast_list *list = parse_formals(count);

	if (match(current, lx_rparen)){
		return list;
	}
	else {
		scanner->fd->reportError("Expected Right Parantheses!");
	}
}
ast_list *Parser::parse_formals(int &count) {
	TOKEN * id = current;
	if (match(id, lx_identifier)) {
		if (match(current, lx_colon)) {
			j_type varType = parse_type();
			SymbolTableEntry * entry = parse_id_var(varType, id);
			AST * node = make_ast_node(3, ast_var_decl, entry, varType);
			ast_list * list = parse_formals_bar(count);
			count = 1 + count;
			return const_ast(node, list);
		}
		else {
			scanner->fd->reportError("Expected Colon!");
		}
	}
	else {
		scanner->fd->reportError("Expected Identifier!");
	}
	return NULL;
}
ast_list * Parser::parse_formals_bar(int &  count) {
	if (match(current, lx_comma)) {
		TOKEN * id = current;
		if (match(id, lx_identifier)) {
			if (match(current, lx_colon)) {
				j_type varType = parse_type();
				SymbolTableEntry * entry = parse_id_var(varType, id);
				AST * node = make_ast_node(3, ast_var_decl, entry, varType);
				ast_list * list = parse_formals_bar(count);
				count = 1 + count;
				return const_ast(node, list);
			}
			else {
				scanner->fd->reportError("Expected Colon!");
			}
		}
		else {
			scanner->fd->reportError("Expected Identifier!");
		}
	}
	return NULL;
}
AST * Parser::parse_block(){
	if (match(current, kw_begin)) {
		ast_list * var_list = parse_var_decl_list();
		ast_list * stmt_list = parse_stmt_list();
		if (match(current, kw_end)) {
			return make_ast_node(3, ast_block, var_list, stmt_list);
		}
		else {
			scanner->fd->reportError("Expected End!");
		}
	}
	return NULL;
}
ast_list * Parser::parse_var_decl_list(){
	ast_list * var_decl_list = NULL;
	AST * var_decl = parse_var_decl();
	if (var_decl == NULL)
		return NULL;

	if (match(current, lx_semicolon)){
		var_decl_list = parse_var_decl_list();
		var_decl_list = const_ast(var_decl, var_decl_list);
	}
	else {
		scanner->fd->reportError("Expected Colon!");
	}
	return var_decl_list;
}
AST * Parser::parse_var_decl() {
	TOKEN* id = NULL;

	id = current;
	if (match(current, lx_identifier)) {
		if (match(current, lx_colon)) {
			j_type varType = parse_type();
			SymbolTableEntry * entry = parse_id_var(varType, id);
			return make_ast_node(3, ast_var_decl, entry, varType);
		}
		else if (match(current, lx_colon_eq)){
			AST * exp = parse_expr();
			SymbolTableEntry * e = scope.getFirstOcc(id->str_ptr);
			if (e->entry_type == ste_undefined) {
				scanner->fd->reportError("Undefined Variable!");
			}
			else {
				return make_ast_node(3, ast_assign, e, exp);
			}
		}
		else if (current->type== lx_lparen){
			int count = 0;
			ast_list * args = parse_arg_list(count);
			SymbolTableEntry * e = scope.getFirstOcc(id->str_ptr);
			if (e->entry_type == ste_undefined) {
				scanner->fd->reportError("Undefined Function!");
			}
			else if (count != e->f.routine.formalNumber) {
				scanner->fd->reportError("Wrong Number of arguments!");
			}
			else {
				return make_ast_node(3, ast_call, e, args);
			}
		}
		else {
			scanner->fd->reportError("Expected Colon!");
		}
	}
	else if(match(current,kw_constant)){
		id = current;
		if (match(current, lx_identifier)) {
			if (match(current, lx_eq)) {
				AST * exp=parse_expr();
				int value=eval_ast_expr(scanner->fd, exp);
				SymbolTableEntry * entry=parse_id_cons(id,value);
				return make_ast_node(3,ast_const_decl,entry,value);
			}
			else {
				scanner->fd->reportError("Expected =");
			}
		}
		else{
			scanner->fd->reportError("Expected Identifer!");
		}
	}
	return  NULL;
}
ast_list * Parser::parse_stmt_list(){
	ast_list * stmt_list = NULL;

	AST * stmt = parse_stmt();
	if (stmt != NULL){
		if (match(current, lx_semicolon)){			
			stmt_list = parse_stmt_list();
			stmt_list = const_ast(stmt, stmt_list);
		}
		else{
			scanner->fd->reportError("Expected Semicolon!");
		}
	}	
	return stmt_list;
}
AST * Parser::parse_call_assign(TOKEN * id) {
	// assign (:= <expr>)
	if (match(current, lx_colon_eq)) {
		AST * exp = parse_expr();
		SymbolTableEntry * e = scope.getFirstOcc(id->str_ptr);
		if (e->entry_type == ste_undefined) {
			scanner->fd->reportError("Undefined Variable!");
		}
		else {
			return make_ast_node(3, ast_assign, e, exp);
		}
	}
	// routine call
	else {
		int count = 0;
		ast_list * args = parse_arg_list(count);
		SymbolTableEntry * e = scope.getFirstOcc(id->str_ptr);
		if (e->entry_type == ste_undefined) {
			scanner->fd->reportError("Undefined Function!");
		}
		else if (count != e->f.routine.formalNumber) {
			scanner->fd->reportError("Wrong Number of arguments!");
		}
		else {
			return make_ast_node(3, ast_call, e, args);
		}
	}
}
AST * Parser::parse_ifstmt() {
	AST * exp = parse_expr();
	if (match(current, kw_then)) {
		AST * consequance = parse_stmt();
		if (match(current, kw_fi)) {
			return make_ast_node(4, ast_if, exp, consequance, NULL);
		}
		// if-else stmt
		else if (match(current, kw_else)) {
			AST * alt = parse_stmt();
			if (match(current, kw_fi)) {
				return make_ast_node(4, ast_if, exp, consequance, alt);
			}
			else {
				scanner->fd->reportError("Expected fi");
			}
		}
		else {
			scanner->fd->reportError("Expected fi or else");
		}
	}
	else {
		scanner->fd->reportError("Expected then");
	}
}
AST * Parser::parse_stmt() {
	TOKEN * id = current;
	if (match(current, lx_identifier)) {
		// Wbar
		return parse_call_assign(id);
	}
	else if (match(current, kw_if)) {
		return parse_ifstmt();
	}
	else if (match(current, kw_while)) {
		AST * predicate = parse_expr();
		if (match(current, kw_do)) {
			AST * body = parse_stmt();
			if (match(current, kw_od)) {
				return make_ast_node(4, ast_while, predicate, body);
			}
			else {
				scanner->fd->reportError("Expected od!");
			}
		}
		else {
			scanner->fd->reportError("Expected do!");
		}
	}

	else if (match(current, kw_for)) {
		TOKEN * id = current;
		if (match(id, lx_identifier)) {
			AST * assign = parse_call_assign(id);

			if (assign->type == ast_assign) {
				AST * lower = assign;
				if (match(current, kw_to)) {
					AST * upper = parse_expr();
					if (match(current, kw_do)) {
						AST * body = parse_stmt();
						if (match(current, kw_od))
							return make_ast_node(5, ast_for, lower->f.a_assign.lhs, lower, upper, body);
						else {
							scanner->fd->reportError("Expected od!");
						}
					}
					else {
						scanner->fd->reportError("Expected do!");
					}
				}
				else {
					scanner->fd->reportError("Expected to!");
				}
			}
			else {
				scanner->fd->reportError("Expected :=");
			}
		}
		else{
			scanner->fd->reportError("Expected Identifier!");
		}
	}
	else if (match(current, kw_read)) {
		if (match(current, lx_lparen)) {
			TOKEN * id = current;
			if (match(id, lx_identifier)) {
				SymbolTableEntry * entry = scope.getFirstOcc(id->str_ptr);
				if (entry->entry_type == ste_undefined) {
					scanner->fd->reportError("Undefined Variable!");
				}
				if (match(current, lx_rparen)) {
					return make_ast_node(2, ast_read, entry);
				}
				else {
					scanner->fd->reportError("Expected Right Parantheses!");
				}
			}
			else {
				scanner->fd->reportError("Expected Indentifier!");
			}
		}
		else {
			scanner->fd->reportError("Expected Left Parantheses!");
		}
	}
	else if (match(current, kw_write)) {
		if (match(current, lx_lparen)) {
			TOKEN * id = current;
			if (match(id, lx_identifier)) {
				SymbolTableEntry * entry = scope.getFirstOcc(id->str_ptr);
				if (entry->entry_type == ste_undefined) {
					scanner->fd->reportError("Undefined Variable!");
				}
				if (match(current, lx_rparen)) {
					return make_ast_node(2, ast_write, entry);
				}
				else {
					scanner->fd->reportError("Expected Right Parantheses!");
				}
			}
			else {
				scanner->fd->reportError("Expected Indentifier!");
			}
		}
		else {
			scanner->fd->reportError("Expected Left Parantheses!");
		}
	}
	else if (match(current, kw_return)) {
		if (match(current, lx_lparen)) {
			AST * exp = parse_expr();
			if (match(current, lx_rparen)) {
				return make_ast_node(2, ast_return, exp);
			}
			else {

				scanner->fd->reportError("Expected Right Parantheses!");
			}
		}
		else {
			scanner->fd->reportError("Expected Left Parantheses!");
		}
	}
	else if (current->type == kw_begin){
		return parse_block();
	}
	return NULL;
}

ast_list * Parser::parse_arg_list(int &count) {
	if (match(current, lx_lparen)) {
		return parse_arg_list_bar(count);
	}
	else
		scanner->fd->reportError("Expected Left Parantheses!");

	return NULL;
}
ast_list * Parser::parse_arg_list_bar(int & count) {
	if (match(current, lx_rparen)){
		return NULL;
	}
	ast_list * list = parse_args(count);

	if (match(current, lx_rparen)){
		return list;
	}
	else {
		scanner->fd->reportError("Expected Right Parantheses");
	}
}
ast_list * Parser::parse_args(int & count) {
	AST * exp = parse_expr();
	ast_list * list = parse_args_bar(count);
	count = 1 + count;
	return const_ast(exp, list);
}
ast_list * Parser::parse_args_bar(int &  count) {
	if (match(current, lx_comma)) {
		ast_list * args = parse_args(count);
		return args;
	}
	return NULL;
}

AST * Parser::parse_expr() {
	AST * r = parse_rel();
	return parse_expr_bar(r);
}
AST * Parser::parse_expr_bar(AST * expr_l) {
	AST * expr_r;
	AST * expr;
	if (match(current, kw_and)) {
		expr_r = parse_rel();
		expr = make_ast_node(3, ast_and, expr_l, expr_r);
		return parse_expr_bar(expr);
	}
	else if (match(current, kw_or)) {
		expr_r = parse_rel();
		expr = make_ast_node(3, ast_or, expr_l, expr_r);
		return parse_expr_bar(expr);
	}
	return expr_l;
}
AST * Parser::parse_rel() {
	AST * arith = parse_arith();
	return parse_rel_bar(arith);
}
AST * Parser::parse_rel_bar(AST * rel_l) {
	TOKEN * t = current;
	AST * rel_r;
	AST * rel = NULL;
	if (match(t, lx_le)) {
		rel_r = parse_arith();
		rel = make_ast_node(3, ast_le, rel_l, rel_r);
		return parse_rel_bar(rel);
	}
	else if (match(t, lx_gt)){
		rel_r = parse_arith();
		rel = make_ast_node(3, ast_gt, rel_l, rel_r);
		return parse_rel_bar(rel);
	}
	else if (match(t, lx_ge)){
		rel_r = parse_arith();
		rel = make_ast_node(3, ast_ge, rel_l, rel_r);
		return parse_rel_bar(rel);
	}
	else if (match(t, lx_lt)){
		rel_r = parse_arith();
		rel = make_ast_node(3, ast_lt, rel_l, rel_r);
		return parse_rel_bar(rel);
	}
	else if (match(t, lx_eq)){
		rel_r = parse_arith();
		rel = make_ast_node(3, ast_eq, rel_l, rel_r);
		return parse_rel_bar(rel);
	}
	else if (match(t, lx_neq)) {
		rel_r = parse_arith();
		rel = make_ast_node(3, ast_neq, rel_l, rel_r);
		return parse_rel_bar(rel);
	}
	return rel_l;
}
AST * Parser::parse_arith() {
	AST * unary = parse_unary();
	return parse_arith_bar(unary);
}
AST * Parser::parse_arith_bar(AST * arith_l) {
	TOKEN * t = current;
	AST * arith_r;
	AST * arith;
	if (match(t, lx_plus)) {
		arith_r = parse_unary();
		arith = make_ast_node(3, ast_plus, arith_l, arith_r);
		return parse_arith_bar(arith);
	}
	else if (match(t, lx_minus)){
		arith_r = parse_unary();
		arith = make_ast_node(3, ast_minus, arith_l, arith_r);
		return parse_arith_bar(arith);
	}
	else if (match(t, lx_star)) {
		arith_r = parse_unary();
		arith = make_ast_node(3, ast_times, arith_l, arith_r);
		return parse_arith_bar(arith);
	}
	else if (match(t, lx_slash)) {
		arith_r = parse_unary();
		arith = make_ast_node(3, ast_divide, arith_l, arith_r);
		return parse_arith_bar(arith);
	}
	return arith_l;
}
AST * Parser::parse_unary(){
	AST_type type;
	AST * f;
	if (match(current, kw_not)) {
		type = ast_not;
		f = parse_f();
		return make_ast_node(2, type, f);
	}
	else if (match(current, lx_minus)) {
		type = ast_uminus;
		f = parse_f();
		return make_ast_node(2, type, f);
	}
	return parse_f();
}
AST * Parser::parse_call_id(TOKEN * id) {

	if (current->type == lx_lparen) {
		int count = 0;
		ast_list * args = parse_arg_list(count);
		SymbolTableEntry * e = scope.getFirstOcc(id->str_ptr);
		if (e->entry_type == ste_undefined) {
			scanner->fd->reportError("Undefined Function!");
		}
		else if (count != e->f.routine.formalNumber) {
			scanner->fd->reportError("Wrong Number of arguments!");
		}
		else {
			return make_ast_node(3, ast_call, e, args);
		}
	}
	else {
		SymbolTableEntry * e = scope.getFirstOcc(id->str_ptr);
		if (e->entry_type == ste_undefined) {
			scanner->fd->reportError("Undefined Variable!");
		}
		else {
			return make_ast_node(2, ast_var, e);
		}
	}
}
AST * Parser::parse_f() {
	TOKEN * t = current;
	if (match(t, lx_identifier)) {
		return parse_call_id(t);
	}
	else if (match(current, lx_integer)) {
		return make_ast_node(2, ast_integer, atoi(t->str_ptr));
	}
	else if (match(current, lx_string)) {
		return make_ast_node(2, ast_string, t->str_ptr);
	}
	else if (match(current, kw_true)) {
		return make_ast_node(2, ast_boolean, 1);
	}
	else if (match(current, kw_false)) {

		return make_ast_node(2, ast_boolean, 0);
	}
	else if (match(current, lx_lparen)) {
		AST * expr = parse_expr();
		if (match(current, lx_rparen)) {
			return expr;
		}
		else {
			scanner->fd->reportError("Expected Right Parantheses!");
		}
	}
	else {
		scanner->fd->reportError("Invalid Expression!");
	}
}