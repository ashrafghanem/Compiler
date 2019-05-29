%{
	#include<stdio.h>
    void yyerror(char *);
%}

idStart 			[a-zA-Z_]
digit  				[0-9]
White_space			[ \n\f\t]*

%%
	/* keywords */
begin|end|procedure|function|return|not|program|var|constant|integer|bool|string|float|true|false|if|fi|then|else|while|do|od|and|or|read|write|for|from|to|by {
	printf("keyword: %s\n",yytext);
}

	/* Strings */
\"[^\"\n]*\" {printf("String\n");}

	/* String Error */
\"[^\"\n]* {printf("Expected End Quotation!\n");}

	/* identifiers */
{idStart}({idStart}|{digit})* {printf("Identifier\n");}

	/* Integer_literals */
(\-)?{digit}+ {printf("Integer Literal: %d\n",atoi(yytext));}

	/* Integer Error */
(\-)?{digit}+{idStart}+.* {printf("Unrecognized Integer!\n");}

	/* Float_literals */
(\-)?{digit}+"."{digit}+ {printf("Float Literal: %f\n",atof(yytext));}

	/* Float Error */
(\-)?{digit}+"."|(\-)?{digit}+"."{digit}+{idStart}+.* {printf("Unrecognized Float!\n");}

	/* Operators */
"("|")"|":"|":="|"+"|"-"|"*"|"/"|"="|"!="|"<"|"<="|">"|">="|"."|";"|"["|"]"|","|"{"|"}" {
	printf("Operator: %s\n",yytext);
}

	/* Comments */
"##".+("##")?   printf("Comment\n");

	/* Comments Error */
"#".+ 			printf("Wrong Comment\n");

	/* White_space */
[ \t\n]+  		/* eat up white spaces */	

.				printf("Invalid token!\n");

%%

int yywrap(void)
{
    return 1;
}
int main(){
	yylex();
	return 0;
}
