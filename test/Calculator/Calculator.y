%{

uses "math";
uses "console";
uses "string";
uses "ferite-lex";

%}

/* YACC Declarations */
%token NUM
%left '-' '+'
%left '*' '/'
%left NEG /* negation--unary minus */
%right '^' /* exponentiation */
  
/* Grammar follows */
%%
  input: /* empty string */
| input line
  ;

line: 
  '\n'
| exp '\n' { Console.println(" " + $1.nval + " "); }
  ;

exp: 
  NUM { $$ = $1; }
| exp '+' exp { $$.nval = $1.nval + $3.nval; }
| exp '-' exp { $$.nval = $1.nval - $3.nval; }
| exp '*' exp { $$.nval = $1.nval * $3.nval; }
| exp '/' exp { $$.nval = $1.nval / $3.nval; } 
| '-' exp %prec NEG { $$.nval = -$2.nval; }
| exp '^' exp { $$.nval = Number.pow($1.nval, $3.nval).round(); }
| '(' exp ')' { $$.nval = $2.nval; }
  ;
 
%%
  
function yyerror(string s)
     Console.println("error: $s");
