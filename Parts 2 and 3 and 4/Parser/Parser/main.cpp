#include "parser.h"

int main(int argc, const char * argv[]) {

	Parser parser("file.txt");
	ast_list *program = parser.parse_decl_list();
	FILE * fp = fopen("output.txt", "w");
	print_ast_list(fp, program, "", 0);

	return 0;
}