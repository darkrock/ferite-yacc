/*	$Id$	*/

/*
 * Copyright (c) 1997 Sandro Sigala <ssigala@globalnet.it>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * ISO C parser.
 *
 * Based on the ISO C 9899:1990 international standard.
 */

%{
	
	uses "console";
	uses "array";
	uses "AST";
	
%}

%token IDENTIFIER TYPEDEF_NAME INTEGER FLOATING CHARACTER STRING

%token ELLIPSIS ADDEQ SUBEQ MULEQ DIVEQ MODEQ XOREQ ANDEQ OREQ SL SR
%token SLEQ SREQ EQ NOTEQ LTEQ GTEQ ANDAND OROR PLUSPLUS MINUSMINUS ARROW

%token AUTO BREAK CASE CHAR CONST CONTINUE DEFAULT DO DOUBLE ELSE ENUM
%token EXTERN FLOAT FOR GOTO IF INT LONG REGISTER RETURN SHORT SIGNED SIZEOF
%token STATIC STRUCT SWITCH TYPEDEF UNION UNSIGNED VOID VOLATILE WHILE INLINE
%token ASM

// Extra tokens
%token T_INDEX T_DOT T_UNARY_OP T_CAST T_MULT T_DIV T_MOD T_ADD T_SUB T_LT T_GT T_AND T_OR T_XOR T_COND
%token T_EQ T_LIST T_DECLARATION T_POINTER T_PARAM T_FUNCTION T_DECL_LIST T_BLOCK T_RETURN_DECL T_ROOT
%token T_TYPE T_INBRACKET T_THEN T_BLOCK_WRAPPER T_INIT T_TEST T_INCR T_BRACKETS
%token MODIFIER T_FUNCTION_CALL T_DECLARATOR

%start program_entry

%%

/* B.2.1 Expressions. */

string_list:
	STRING													{ $$ = Tree.value(STRING,$1.sval); }
|	string_list STRING										{ $$ = $1; $$.addSibling(Tree.value(STRING,$2.sval)); }
primary_expression:
	identifier
|	INTEGER													{ $$ = Tree.value(INTEGER, $1.nval); }
|	CHARACTER												{ $$ = Tree.value(CHARACTER, $1.sval); }
|	FLOATING												{ $$ = Tree.value(FLOATING, $1.nval); }
|	string_list												{ $$ = Tree.unary(T_LIST, $1); }
|	'(' expression ')'										{ $$ = Tree.unary(T_BRACKETS,$2); }
	;

identifier:
	IDENTIFIER												{ $$ = Tree.value(IDENTIFIER, $1.sval); }
	;

postfix_expression:
	primary_expression
|	postfix_expression '[' expression ']'					{ $$ = Tree.binary(T_INDEX, $1, $3); }
|	postfix_expression '(' argument_expression_list ')'		{ $$ = Tree.binary(T_FUNCTION_CALL, $1, Tree.unary(T_BRACKETS,$3)); }
|	postfix_expression '(' ')'								{ $$ = Tree.binary(T_FUNCTION_CALL, $1, Tree.node(T_BRACKETS)); }
|	postfix_expression '.' identifier						{ $$ = Tree.binary(T_DOT,$1, $3); }
|	postfix_expression ARROW identifier						{ $$ = Tree.binary(ARROW,$1,$3); }
|	postfix_expression PLUSPLUS								{ $$ = Tree.unary(PLUSPLUS,$1); }
|	postfix_expression MINUSMINUS							{ $$ = Tree.unary(MINUSMINUS,$1); }
	;

argument_expression_list:
	assignment_expression
|	argument_expression_list ',' assignment_expression		{ $$ = $1; $$.addSibling($3); }
	;

unary_expression:
	postfix_expression
|	PLUSPLUS unary_expression
|	MINUSMINUS unary_expression
|	unary_operator cast_expression							{ $$ = $1; $$.add($2); }
|	SIZEOF unary_expression									{ $$ = Tree.unary(SIZEOF,$2); }
|	SIZEOF '(' type_name ')'								{ $$ = Tree.unary(SIZEOF,$2); }
	;

unary_operator:
	'&'														{ $$ = Tree.value(T_UNARY_OP,'&'); }
|	'*'														{ $$ = Tree.value(T_UNARY_OP,'*'); }
|	'+'														{ $$ = Tree.value(T_UNARY_OP,'+'); }
|	'-'														{ $$ = Tree.value(T_UNARY_OP,'-'); }
|	'~'														{ $$ = Tree.value(T_UNARY_OP,'~'); }
|	'!'														{ $$ = Tree.value(T_UNARY_OP,'!'); }
	;

cast_expression:
	unary_expression
|	'(' type_name ')' cast_expression						{ $$ = Tree.binary(T_CAST,$2,$4); }
	;

multiplicative_expression:
	cast_expression
|	multiplicative_expression '*' cast_expression			{ $$ = Tree.binary(T_MULT,$1,$3); }
|	multiplicative_expression '/' cast_expression			{ $$ = Tree.binary(T_DIV,$1,$3); }
|	multiplicative_expression '%' cast_expression			{ $$ = Tree.binary(T_MOD,$1,$3); }
	;

additive_expression:
	multiplicative_expression
|	additive_expression '+' multiplicative_expression		{ $$ = Tree.binary(T_ADD,$1,$3); }
|	additive_expression '-' multiplicative_expression		{ $$ = Tree.binary(T_SUB,$1,$3); }
	;

shift_expression:
	additive_expression
|	shift_expression SL additive_expression					{ $$ = Tree.binary(SL,$1,$3); }
|	shift_expression SR additive_expression					{ $$ = Tree.binary(SR,$1,$3); }
	;

relational_expression:
	shift_expression
|	relational_expression '<' shift_expression				{ $$ = Tree.binary(T_LT,$1,$3); }
|	relational_expression '>' shift_expression				{ $$ = Tree.binary(T_GT,$1,$3); }
|	relational_expression LTEQ shift_expression				{ $$ = Tree.binary(LTEQ,$1,$3); }
|	relational_expression GTEQ shift_expression				{ $$ = Tree.binary(GTEQ,$1,$3); }
	;

equality_expression:
	relational_expression
|	equality_expression EQ relational_expression			{ $$ = Tree.binary(EQ,$1,$3); }
|	equality_expression NOTEQ relational_expression			{ $$ = Tree.binary(NOTEQ,$1,$3); }
	;

and_expression:
	equality_expression
|	and_expression '&' equality_expression					{ $$ = Tree.binary(T_AND,$1,$3); }
	;

exclusive_or_expression:
	and_expression
|	exclusive_or_expression '^' and_expression				{ $$ = Tree.binary(T_XOR,$1,$3); }
	;

inclusive_or_expression:
	exclusive_or_expression
|	inclusive_or_expression '|' exclusive_or_expression		{ $$ = Tree.binary(T_OR,$1,$3); }
	;

logical_and_expression:
	inclusive_or_expression
|	logical_and_expression ANDAND inclusive_or_expression	{ $$ = Tree.binary(ANDAND,$1,$3); }
	;

logical_or_expression:
	logical_and_expression
|	logical_or_expression OROR logical_and_expression		{ $$ = Tree.binary(OROR,$1,$3); }
	;

conditional_expression:
	logical_or_expression
|	logical_or_expression '?' expression ':' conditional_expression { Tree.ternary(T_COND,$1,$2,$3); }
	;

assignment_expression:
	conditional_expression
|	unary_expression assignment_operator assignment_expression	{ $$ = $2; $$.add($1); $$.add($3); }
	;

assignment_operator:
	'='														{ $$ = Tree.node(T_EQ); }
|	MULEQ													{ $$ = Tree.node(MULEQ); }
|	DIVEQ													{ $$ = Tree.node(DIVEQ); }
|	MODEQ													{ $$ = Tree.node(MODEQ); }
|	ADDEQ													{ $$ = Tree.node(ADDEQ); }
|	SUBEQ													{ $$ = Tree.node(SUBEQ); }
|	SLEQ													{ $$ = Tree.node(SLEQ); }
|	SREQ													{ $$ = Tree.node(SREQ); }
|	ANDEQ													{ $$ = Tree.node(ANDEQ); }
|	XOREQ													{ $$ = Tree.node(XOREQ); }
|	OREQ													{ $$ = Tree.node(OREQ); }
	;

expression:
	assignment_expression
|	expression ',' assignment_expression					{ $$ = $1; $$.addSibling($3);}
	;

constant_expression:
	conditional_expression
	;

declaration:
	declaration_specifiers init_declarator_list ';'			{ 
																$$ = Tree.binary(T_DECLARATION,Tree.unary(T_TYPE,$1),Tree.unary(T_LIST,$2));
																if( $$.canFind('<typedef />') ) {
																	$2.find('<identifier capture="name" />') using ( nodes, captures ) {
																		string TypeName = captures['name'].value;
																		.container.addType(TypeName);
																	};
																}
															}
|	declaration_specifiers ';'								{ $$ = Tree.unary(T_DECLARATION,Tree.unary(T_TYPE,$1)); }
	;

declaration_specifiers:
	storage_class_specifier declaration_specifiers			{ $$ = $1; $$.addSibling($2); }
|	storage_class_specifier
|	type_specifier declaration_specifiers					{ $$ = $1; $$.addSibling($2); }
|	type_specifier
|	type_qualifier declaration_specifiers					{ $$ = $1; $$.addSibling($2); }
|	type_qualifier
	;

init_declarator_list:
	init_declarator
|	init_declarator_list ',' init_declarator				{ $$ = $1; $$.addSibling($3); }
	;

init_declarator:
	declarator
|	declarator '=' initializer								{ $$ = $1; $$.add(Tree.unary(T_EQ,$3)); }
	;

storage_class_specifier:
	TYPEDEF													{ $$ = Tree.node(TYPEDEF); }
|	EXTERN													{ $$ = Tree.node(EXTERN); }
|	STATIC													{ $$ = Tree.node(STATIC); }
|	AUTO													{ $$ = Tree.node(AUTO); }
|	REGISTER												{ $$ = Tree.node(REGISTER); }
|   INLINE													{ $$ = Tree.node(INLINE); }
	;

type_specifier:
	VOID													{ $$ = Tree.node(VOID); }
|	CHAR													{ $$ = Tree.node(CHAR); }
|	SHORT													{ $$ = Tree.node(SHORT); }
|	INT														{ $$ = Tree.node(INT); }
|	LONG													{ $$ = Tree.node(LONG); }
|	FLOAT													{ $$ = Tree.node(FLOAT); }
|	DOUBLE													{ $$ = Tree.node(DOUBLE); }
|	SIGNED													{ $$ = Tree.node(SIGNED); }
|	UNSIGNED												{ $$ = Tree.node(UNSIGNED); }
|	struct_or_union_specifier
|	enum_specifier
|	TYPEDEF_NAME											{ $$ = Tree.value(TYPEDEF_NAME, $1.sval); }
	;

struct_or_union_specifier:
	struct_or_union identifier '{' struct_declaration_list '}' { $$ = $1; $$.add($2); $$.add($4); }
|	struct_or_union '{' struct_declaration_list '}'			{ $$ = $1; $$.add($3); }
|	struct_or_union identifier								{ $$ = $1; $$.add($2); }
	;

struct_or_union:
	STRUCT													{ $$ = Tree.node(STRUCT); }
|	UNION													{ $$ = Tree.node(UNION); }
	;

struct_declaration_list:
	struct_declaration
|	struct_declaration_list struct_declaration				{ $$ = $1; $$.addSibling($2); }
	;

struct_declaration:
	specifier_qualifier_list struct_declarator_list ';'		{ $$ = Tree.binary(T_DECLARATION,Tree.unary(T_LIST,$1),$2); }
	;

specifier_qualifier_list:
	type_specifier specifier_qualifier_list					{ $$ = $1; $$.addSibling($2); }
|	type_specifier
|	type_qualifier specifier_qualifier_list					{ $$ = $1; $$.addSibling($2); }
|	type_qualifier
	;

struct_declarator_list:
	struct_declarator
|	struct_declarator_list ',' struct_declarator			{ $$ = $1; $$.addSibling($3); }
	;

struct_declarator:
	declarator
|	 ':' constant_expression								{ $$ = Tree.unary(T_DECLARATION,Tree.unary(T_EQ,$2)); }
|	declarator ':' constant_expression						{ $$ = $1; $$.add(Tree.unary(T_EQ,$3)); }
	;

enum_specifier:
	ENUM identifier '{' enumerator_list '}'					{ $$ = Tree.binary(ENUM, $1, Tree.unary(T_LIST,$3)); }
|	ENUM '{' enumerator_list '}'							{ $$ = Tree.unary(ENUM, Tree.unary(T_LIST,$2)); }
|	ENUM identifier											{ $$ = Tree.unary(ENUM, $2); }
	;

enumerator_list:
	enumerator
|	enumerator_list ',' enumerator							{ $$ = $1; $$.addSibling($3); }
	;

enumerator:
	identifier
|	identifier '=' constant_expression						{ $$ = $1; $$.add(Tree.unary(T_EQ,$3)); }
	;

type_qualifier:
	CONST													{ $$ = Tree.node(CONST); }
|	VOLATILE												{ $$ = Tree.node(VOLATILE); }
	;

declarator:
	pointer direct_declarator								{ $$ = Tree.binary(T_DECLARATOR,$1,$2); }
|	direct_declarator										{ $$ = $1; }
	;

direct_declarator:
	identifier												{ $$ = Tree.unary(T_DECLARATOR,$1); }
|	'(' declarator ')'										{ $$ = Tree.unary(T_INBRACKET,$2); }
|	direct_declarator '[' constant_expression ']'			{ $$ = Tree.binary(T_DECLARATOR,$1,Tree.unary(T_INDEX,$3)); }
|	direct_declarator '[' ']'								{ $$ = Tree.binary(T_DECLARATOR,$1,Tree.node(T_INDEX)); }
|	direct_declarator '(' parameter_type_list ')'			{ $$ = Tree.binary(T_DECLARATOR,$1,Tree.unary(T_LIST,$3)); }
|	direct_declarator '(' identifier_list ')'				{ $$ = Tree.binary(T_DECLARATOR,$1,Tree.unary(T_LIST,$3)); }
|	direct_declarator '(' ')'								{ $$ = Tree.binary(T_DECLARATOR,$1,Tree.node(T_LIST)); }
	;

pointer:
	'*' type_qualifier_list									{ $$ = Tree.unary(T_POINTER,Tree.unary(T_LIST,$2)); }
|	'*'														{ $$ = Tree.node(T_POINTER); }
|	'*' type_qualifier_list pointer							{ $$ = Tree.binary(T_POINTER,Tree.unary(T_LIST,$2),$3); }
|	'*' pointer												{ $$ = Tree.unary(T_POINTER,$2); }
	;

type_qualifier_list:
	type_qualifier
|	type_qualifier_list type_qualifier						{ $$ = $1; $$.addSibling($2); }
	;

parameter_type_list:
	parameter_list
|	parameter_list ',' ELLIPSIS								{ $$ = $1; $$.addSibling(Tree.node(ELLIPSIS)); }
	;

parameter_list:
	parameter_declaration
|	parameter_list ',' parameter_declaration				{ $$ = $1; $$.addSibling($3); }
	;

parameter_declaration:
	declaration_specifiers declarator						{ $$ = Tree.binary(T_PARAM,Tree.unary(T_TYPE,$1),$2); }
|	declaration_specifiers abstract_declarator				{ $$ = Tree.binary(T_PARAM,Tree.unary(T_TYPE,$1),$2); }
|	declaration_specifiers									{ $$ = Tree.unary(T_PARAM,$1); }
	;

identifier_list:
	identifier
|	identifier_list ',' identifier							{ $$ = $1; $$.addSibling($3); }
	;

type_name:
	specifier_qualifier_list
|	specifier_qualifier_list abstract_declarator			{ $$ = $1; $$.addSibling($2); }
	;

abstract_declarator:
	pointer													{ $$ = Tree.unary(T_DECLARATOR,$1); }
|	direct_abstract_declarator								{ $$ = Tree.unary(T_DECLARATOR,$1); }
|	pointer direct_abstract_declarator						{ $$ = Tree.binary(T_DECLARATOR,$1,$2); }
	;

direct_abstract_declarator:
	'(' abstract_declarator ')'								{ $$ = Tree.unary(T_BRACKETS,$2); }
|	'[' ']'													{ $$ = Tree.node(T_INDEX); }
|	'[' constant_expression ']'								{ $$ = Tree.unary(T_INDEX,$2); }
|	direct_abstract_declarator '[' ']'						{ $$ = $1; $$.add(Tree.node(T_INDEX)); }
|	direct_abstract_declarator '[' constant_expression ']'	{ $$ = $1; $$.add(Tree.unary(T_INDEX,$3)); }
|	'(' ')'													{ $$ = Tree.node(T_BRACKETS); }
|	'(' parameter_type_list ')'								{ $$ = Tree.unary(T_BRACKETS,$2); }
|	direct_abstract_declarator '(' ')'						{ $$ = $1; $$.add(Tree.node(T_BRACKETS)); }
|	direct_abstract_declarator '(' parameter_type_list ')'	{ $$ = $1; $$.add(Tree.unary(T_BRACKETS,$3)); }
	;

initializer:
	assignment_expression
|	'{' initializer_list '}'								{ $$ = Tree.unary(T_LIST,$2); }
|	'{' initializer_list ',' '}'							{ $$ = Tree.unary(T_LIST,$2); }
	;

initializer_list:
	initializer
|	initializer_list ',' initializer						{ $$ = $1; $$.addSibling($3); }
	;

/* B.2.3 Statements. */

statement:
	labeled_statement
|	compound_statement
|	expression_statement
|	selection_statement
|	iteration_statement
|	jump_statement
|	asm_statement
	;

labeled_statement:
	identifier ':' statement								{ $$ = Tree.binary(T_DECLARATION,$1,$3); }
|	CASE constant_expression ':' statement					{ $$ = Tree.unary(CASE,Tree.unary(T_TEST,$2)); $$.addSibling($4); }
|	DEFAULT ':' statement									{ $$ = Tree.node(DEFAULT); $$.addSibling($3); }
	;

compound_statement:
	'{' '}'
|	'{' statement_list '}'									{ $$ = Tree.unary(T_BLOCK,$2); }
|	'{' declaration_list '}'								{ $$ = Tree.unary(T_BLOCK,Tree.unary(T_DECL_LIST,$2)); }
|	'{' declaration_list statement_list '}'					{ $$ = Tree.binary(T_BLOCK,Tree.unary(T_DECL_LIST,$2),$3); }
	;

declaration_list:
	declaration
|	declaration_list declaration							{ $$ = $1; $$.addSibling($2); }
	;

statement_list:
	statement
|	statement_list statement								{ $$ = $1; $$.addSibling($2); }
	;

expression_statement:
	';'
|	expression ';'											{ $$ = $1; }
	;

selection_statement:
	IF '(' expression ')' statement							{ $$ = Tree.binary(IF, Tree.unary(T_TEST,$3), Tree.unary(T_THEN,$5)); }
|	IF '(' expression ')' statement ELSE statement			{ $$ = Tree.ternary(IF, Tree.unary(T_TEST,$3), Tree.unary(T_THEN,$5), Tree.unary(ELSE,$7)); }
|	SWITCH '(' expression ')' statement						{ $$ = Tree.binary(SWITCH, Tree.unary(T_TEST,$3), $5); }
	;

iteration_statement:
	WHILE '(' expression ')' statement						{ $$ = Tree.binary(WHILE, Tree.unary(T_TEST,$3), Tree.unary(T_BLOCK_WRAPPER,$5)); }
|	DO statement WHILE '(' expression ')' ';'				{ $$ = Tree.binary(DO,Tree.unary(T_BLOCK_WRAPPER,$2),Tree.unary(T_TEST,$5)); }
|	FOR '(' ';' ';' ')' statement							{ $$ = Tree.unary(FOR,Tree.unary(T_BLOCK_WRAPPER,$6)); }
|	FOR '(' expression ';' ';' ')' statement				{ $$ = Tree.binary(FOR,Tree.unary(T_INIT,$3),Tree.unary(T_BLOCK_WRAPPER,$7)); }
|	FOR '(' ';' expression ';' ')' statement				{ $$ = Tree.binary(FOR,Tree.unary(T_TEST,$3),Tree.unary(T_BLOCK_WRAPPER,$7)); }
|	FOR '(' expression ';' expression ';' ')' statement		{ $$ = Tree.ternary(FOR,Tree.unary(T_INIT,$3),Tree.unary(T_TEST,$5),Tree.unary(T_BLOCK_WRAPPER,$8)); }
|	FOR '(' ';' ';' expression ')' statement				{ $$ = Tree.binary(FOR,Tree.unary(T_INCR,$3),Tree.unary(T_BLOCK_WRAPPER,$7)); }
|	FOR '(' expression ';' ';' expression ')' statement		{ $$ = Tree.ternary(FOR,Tree.unary(T_INIT,$3),Tree.unary(T_INCR,$6),Tree.unary(T_BLOCK_WRAPPER,$8)); }
|	FOR '(' ';' expression ';' expression ')' statement		{ $$ = Tree.ternary(FOR,Tree.unary(T_TEST,$4),Tree.unary(T_INCR,$6),Tree.unary(T_BLOCK_WRAPPER,$8)); }
|	FOR '(' expression ';' expression ';' expression ')' statement {
																$$ = Tree.node(FOR);
																$$.add(Tree.unary(T_INIT,$3));
																$$.add(Tree.unary(T_TEST,$5));
																$$.add(Tree.unary(T_INCR,$7));
																$$.add(Tree.unary(T_BLOCK_WRAPPER,$9));
															}
	;

jump_statement:
	GOTO identifier ';'										{ $$ = Tree.unary(GOTO,$2); }
|	CONTINUE ';'											{ $$ = Tree.node(CONTINUE); }
|	BREAK ';'												{ $$ = Tree.node(BREAK); }
|	RETURN ';'												{ $$ = Tree.node(RETURN); }
|	RETURN expression ';'									{ $$ = Tree.unary(RETURN,$2); }
	;

asm_statement:
	ASM '('
		string_list ':' STRING '(' identifier ')'
	')'														{ $$ = Tree.ternary(ASM,Tree.unary(T_LIST,$3),Tree.value(STRING,$5.sval),Tree.unary(T_LIST,$7)); }
	;
/* B.2.4 External definitions. */

program_entry:
	translation_unit										{ Tree.root = Tree.unary(T_ROOT,$1); }
	;
	
translation_unit:
	external_declaration
|	translation_unit external_declaration					{ $$ = $1; $$.addSibling($2); }
	;

external_declaration:
	function_definition										{ $$ = $1; }
|	declaration												{ $$ = $1; }
	;

function_definition:
	declaration_specifiers declarator declaration_list compound_statement 	{ 
																				$$ = Tree.node(T_FUNCTION);
																				$$.add(Tree.unary(T_RETURN_DECL,$1));
																				$$.add($2);
																				$$.add($3);
																				$$.add($4);
																			}
|	declaration_specifiers declarator compound_statement	{ $$ = Tree.ternary(T_FUNCTION,Tree.unary(T_RETURN_DECL,$1),$2,$3); }
|	declarator declaration_list compound_statement			{ $$ = Tree.ternary(T_FUNCTION,$1,$2,$3); }
|	declarator compound_statement							{ $$ = Tree.binary(T_FUNCTION,$1,$2); }	
	;

%%

object container;
constructor( object container ) {
	.container = container;
	self();
}
function yyerror(string s)
	raise new Error("error: ${.lexer.line}: $s");