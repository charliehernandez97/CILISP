%{
    #include "cilisp.h"
    #define ylog(r, p) {fprintf(flex_bison_log_file, "BISON: %s ::= %s \n", #r, #p); fflush(stdout);}
    int yylex();
    void yyerror(char*, ...);
%}

%union {
    char *cval;
    double dval;
    int ival;
    struct ast_node *astNode;
    struct symbol_table_node *symTblNode;
}

%token <cval> SYMBOL
%token <ival> FUNC TYPE
%token <dval> INT DOUBLE
%token QUIT EOL EOFT LPAREN RPAREN LET

%type <astNode> s_expr f_expr s_expr_list s_expr_section number
%type <symTblNode> let_section let_list let_elem


%%

program:
    s_expr EOL {
        ylog(program, s_expr EOL);
        if ($1) {
            printRetVal(eval($1));
            freeNode($1);
        }
        YYACCEPT;
    }
    | s_expr EOFT {
        ylog(program, s_expr EOFT);
        if ($1) {
            printRetVal(eval($1));
            freeNode($1);
        }
        exit(EXIT_SUCCESS);
    }
    | EOL {
        ylog(program, EOL);
        YYACCEPT;  // paranoic; main skips blank lines
    }
    | EOFT {
        ylog(program, EOFT);
        exit(EXIT_SUCCESS);
    };


s_expr:
    f_expr {
    	ylog(s_expr, f_expr);
	$$ = $1;
    }
    | number {
        ylog(s_expr, number);
    	$$ = $1;
    }
    | SYMBOL {
    	ylog(s_expr, SYMBOL);
    	$$ = createSymbolNode($1);
    }
    | LPAREN let_section s_expr RPAREN {
    	ylog(s_expr, LPAREN let_section s_expr RPAREN);
    	$$ = createScopeNode($2, $3);
    }
    | QUIT {
        ylog(s_expr, QUIT);
        exit(EXIT_SUCCESS);
    }
    | error {
        ylog(s_expr, error);
        yyerror("unexpected token");
        $$ = NULL;
    };



f_expr:
    LPAREN FUNC s_expr_section RPAREN {
	ylog(f_expr, LPAREN FUNC s_expr_section RPAREN);
	$$ = createFunctionNode($2, $3);
    };

s_expr_section:
    s_expr_list {
    	ylog(s_expr_section, s_expr_list);
    	$$ = $1;
    }
    | /* empty */ {
    	$$ = NULL;
    };

s_expr_list:
    s_expr {
    	ylog(s_expr_list, s_expr);
    	$$ = addExpressionToList($1, NULL);
    }
    | s_expr s_expr_list {
    	ylog(s_expr_list, s_expr s_expr_list);
    	$$ = addExpressionToList($1, $2);
    };

number:
    INT {
    	ylog(number, INT);
    	$$ = createNumberNode($1, INT_TYPE);
    }
    | DOUBLE {
    	ylog(number, DOUBLE);
    	$$ = createNumberNode($1, DOUBLE_TYPE);
    };

let_section:
    LPAREN LET let_list RPAREN {
    	ylog(let_section, LPAREN LET let_list RPAREN);
    	$$ = $3;
    };

let_list:
    let_elem {
    	ylog(let_list, let_elem);
    	$$ = let_list($1, NULL);
    }
    | let_elem let_list {
    	ylog(let_list, let_elem let_list);
    	$$ = let_list($1, $2);
    };

let_elem:
    LPAREN TYPE SYMBOL s_expr RPAREN {
    	ylog(let_elem, LPAREN TYPE SYMBOL s_expr RPAREN);
    	$$ = let_elem($2, $3, $4);
    };




%%

