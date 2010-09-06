#include <stdio.h>
#include <string.h>
#include "defs.h"

#define FERITE_PACKAGE    "@FERITE_PACKAGE@"
#define FERITE_CLASS_DECL "@FERITE_CLASS_DECL@"
#define FERITE_RUN        "@FERITE_RUN@"
#define FERITE_CONSTRUCT  "@FERITE_CONSTRUCT@"
#define FERITE_STACK      "@FERITE_STACK@"
#define FERITE_CLASS_NAME "@FERITE_CLASS_NAME@"

/*  The banner used here should be replaced with an #ident directive  */
/*  if the target C compiler supports #ident directives.    */
/*                  */
/*  If the skeleton is changed, the banner should be changed so that  */
/*  the altered version can easily be distinguished from the original.  */

char *banner[] = { 0 };

char *jbanner[] =
{
    "//### This file created by BYACC 1.8(/ferite extension 1.1)",
      "//### Please send bug reports to chris@ferite.org",
      "\n\n",
      0
};

char *tables[] = { 0 };

char *jtables[] =
{
    "array yylhs;",
      0
};

char *header[] = { 0 };

char *jheader[] =
{
    "\n\n\n",
      FERITE_CLASS_DECL,
      "{\n",
      "number yydebug;        //do I want debug output?",
      "number yynerrs;            //number of errors so far",
      "number yyerrflag;          //was there an error?",
      "number yychar;             //the current working character",
      "//###############################################################",
      "// method: debug",
      "//###############################################################",
      "function debug(string msg)",
      "{",
      "  if (.yydebug)",
      "    Console.println(msg);",
      "}",
      "\n//########## STATE STACK ##########",
      FERITE_STACK,
      "\narray statestk;                //state stack\n",
      "number stateptr;                 // state ptr \n",
      "number stateptrmax;              //highest index of stackptr",
      "number statemax;                 //state when highest index reached",
      "//###############################################################",
      "// methods: state stack push,pop,drop,peek",
      "//###############################################################",
      "function state_push(number state)",
      "{ ",
      "  if( .stateptr >= ",
      FERITE_CLASS_NAME,
      ".YYSTACKSIZE )         //overflowed?",
      "    return;",
      "  .statestk[++.stateptr]=state;",
      "  if( .stateptr > .statemax )",
      "  {",
      "    .statemax = state;",
      "    .stateptrmax = .stateptr;",
      "  }",
      "}\n",
      "function state_pop()",
      "{",
      "  if( .stateptr < 0 )                    //underflowed?",
      "    return -1;",
      "  return .statestk[.stateptr--];",
      "}\n",
      "function state_drop(number cnt)",
      "{",
      "  number ptr;",
      "  ptr = .stateptr - cnt;",
      "  if( ptr < 0 )",
      "    return;",
      "  .stateptr = ptr;",
      "}\n",
      "function state_peek(number relative)",
      "{",
      "  number ptr;",
      "  ptr = .stateptr - relative;",
      "  if( ptr < 0 )",
      "    return -1;",
      "  return .statestk[ptr];",
      "}\n",
      "//###############################################################",
      "// method: init_stacks : allocate and prepare stacks",
      "//###############################################################",
      "function init_stacks()",
      "{",
      "  number i = 0;",
      "  while( i++ < ",
      FERITE_CLASS_NAME,
      ".YYSTACKSIZE )",
      "    .statestk[] = 0;",
      "  .stateptr = -1;",
      "  .statemax = -1;",
      "  .stateptrmax = -1;",
      "  .val_init();",
      "  return true;",
      "}",
      "//###############################################################",
      "// method: dump_stacks : show n levels of the stacks",
      "//###############################################################",
      "function dump_stacks(number count)",
      "{",
      "  number i;",
      "  Console.println(\"=index==state====value=     s:\"+.stateptr+\"  v:\"+.valptr);",
      "  for( i=0; i < count; i++ )",
      "    Console.println(\" \"+i+\"    \"+.statestk[i]+\"      \"+.valstk[i]);",
      "  Console.println(\"======================\");",
      "}",
	"object ASTRootNode;",
	"function setASTRoot( object r )",
	"	.ASTRootNode = r;",
	"function ASTRoot()",
	"	return .ASTRootNode;",
      0
};

char *body[] =
{
    0
};

char *jbody[] =
{
    "//###############################################################",
      "// method: yylexdebug : check lexer state",
      "//###############################################################",
      "function yylexdebug(number state, number ch)",
      "{",
      "  string s = \"\";",
      "  if (ch < 0) ch=0;",
      "  if (ch <= ",
      FERITE_CLASS_NAME,
      ".YYMAXTOKEN) //check index bounds",
      "     s = .yyname[ch];    //now get it",
      "  if (s==\"\")",
      "    s = \"illegal-symbol\";",
      "  .debug(\"state \"+state+\", reading \"+ch+\" (\"+s+\")\");",
      "}\n\n\n",
      "\n",
      "//The following are now global, to aid in error reporting",
      "number yyn;       //next next thing to do",
      "number yym;       //",
      "number yystate;   //current parsing state from state table",
      "string yys;    //current token string",
      "\n",
      "//###############################################################",
      "// method: parse : parse input and execute indicated items",
      "//###############################################################",
      "function parse()",
      "{",
      "  number doaction;",
      "  .init_stacks();",
      "  .yynerrs = 0;",
      "  .yyerrflag = 0;",
      "  .yychar = -1;          // impossible char forces a read",
      "  .yystate = 0;          // initial state",
      "  .state_push(.yystate);   // save it",
      "  while( true )          // until parsing is done, either correctly, or w/error",
      "  {",
      "      doaction = true;",
      "      if( .yydebug )",
      "           .debug( \"loop\" ); ",
      "",
      "      //#### NEXT ACTION (from reduction table)",
      "      for( .yyn = .yydefred[.yystate]; .yyn == 0; .yyn = .yydefred[.yystate] )",
      "      {",
      "          if( .yydebug )",
      "              .debug( \"yyn:\"+.yyn+\"  state:\"+.yystate+\"  yychar:\"+.yychar);",
      "          if( .yychar < 0 )      // we want a char?",
      "          {",
      "              .yychar = .yylex();  // get next token",
      "              if( .yydebug )",
      "                  .debug( \" next yychar:\"+.yychar );",
      "              //#### ERROR CHECK ####",
      "              if( .yychar < 0 )    // it it didn't work/error",
      "              {",
      "                  .yychar = 0;      // change it to default string (no -1!)",
      "                  if (.yydebug)",
      "                      .yylexdebug(.yystate,.yychar);",
      "              }",
      "          } // yychar < 0",
      "          .yyn = .yysindex[.yystate];  // get amount to shift by (shift index)",
      "          if( (.yyn != 0) && (.yyn += .yychar) >= 0 && .yyn <= ",
      FERITE_CLASS_NAME,
      ".YYTABLESIZE && .yycheck[.yyn] == .yychar)",
      "          {",
      "              if( .yydebug )",
      "                  .debug(\"state \"+.yystate+\", shifting to state \"+.yytable[.yyn]);",
      "              //#### NEXT STATE ####",
      "              .yystate = .yytable[.yyn];//we are in a new state",
      "              .state_push(.yystate);   //save it",
      "              .val_push(.yylval);      //push our lval as the input for next rule",
      "              .yychar = -1;           //since we have 'eaten' a token, say we need another",
      "              if( .yyerrflag > 0 )     //have we recovered an error?",
      "                  --.yyerrflag;        //give ourselves credit",
      "              doaction = false;        //but don't process yet",
      "              break;   //quit the yyn=0 loop",
      "          }",
      "",
      "          .yyn = .yyrindex[.yystate];  //reduce",
      "          if( (.yyn !=0) && (.yyn += .yychar) >= 0 && .yyn <= ",
      FERITE_CLASS_NAME,
      ".YYTABLESIZE && .yycheck[.yyn] == .yychar)",
      "          {   //we reduced!",
      "              if( .yydebug )",
      "                  .debug(\"reduce\");",
      "              .yyn = .yytable[.yyn];",
      "              doaction = true; //get ready to execute",
      "              break;         //drop down to actions",
      "          }",
      "          else //ERROR RECOVERY",
      "          {",
      "              if( .yyerrflag == 0 )",
      "              {",
      "                  .yyerror(\"Syntax error\");",
      "                  .yynerrs++;",
      "              }",
      "              if( .yyerrflag < 3 ) //low error count?",
      "              {",
      "                  .yyerrflag = 3;",
      "                  while( true )   //do until break",
      "                  {",
      "                      if( .stateptr < 0 )   //check for under & overflow here",
      "                      {",
      "                          .yyerror(\"stack underflow. aborting...\");  //note lower case 's'",
      "                          return 1;",
      "                      }",
      "                      .yyn = .yysindex[.state_peek(0)];",
      "                      if( (.yyn != 0) && (.yyn += ",
      "YYERRCODE) >= 0 && .yyn <= ",
      FERITE_CLASS_NAME,
      ".YYTABLESIZE && .yycheck[.yyn] == ",
      "YYERRCODE)",
      "                     {",
      "                         if( .yydebug )",
      "                             .debug(\"state \"+.state_peek(0)+\", error recovery shifting to state \"+.yytable[.yyn]+\" \");",
      "                         .yystate = .yytable[.yyn];",
      "                         .state_push(.yystate);",
      "                         .val_push(.yylval);",
      "                         doaction = false;",
      "                         break;",
      "                     }",
      "                     else",
      "                     {",
      "                         if( .yydebug )",
      "                            .debug(\"error recovery discarding state \"+.state_peek(0)+\" \");",
      "                         if( .stateptr < 0 )   //check for under & overflow here",
      "                         {",
      "                            .yyerror(\"Stack underflow. aborting...\");  //capital 'S'",
      "                            return 1;",
      "                         }",
      "                         .state_pop();",
      "                         .val_pop();",
      "                     }",
      "                }",
      "           }",
      "           else            //discard this token",
      "           {",
      "               if( .yychar == 0 )",
      "                   return 1; //yyabort",
      "               if( .yydebug )",
      "               {",
      "                   .yys = \"\";",
      "                   if (.yychar <= ",
      FERITE_CLASS_NAME,
      ".YYMAXTOKEN )",
      "                       .yys = .yyname[.yychar];",
      "                   if( .yys == \"\" )",
      "                       .yys = \"illegal-symbol\";",
      "                   .debug( \"state \"+.yystate+\", error recovery discards token \"+.yychar+\" (\"+.yys+\")\");",
      "               }",
      "               .yychar = -1;  //read another",
      "           }",
      "       } // end error recovery",
      "    } // yyn = 0 loop",
      "",
      "    if( !doaction )   //any reason not to proceed?",
      "        continue;      //skip action",
      "    .yym = .yylen[.yyn];          //get count of terminals on rhs",
      "    if (.yydebug)",
      "        .debug(\"state \"+.yystate+\", reducing \"+.yym+\" by rule \"+.yyn+\" (\"+.yyrule[.yyn]+\")\");",
      "    if (.yym>0)                 //if count of rhs not 'nil'",
      "        .yyval = .val_peek(.yym-1); //get current semantic value",
      "    switch( .yyn )",
      "    {",
      "//########## USER-SUPPLIED ACTIONS ##########",
      0
};

char *jlexerbody[] = 
{
	"object lexer;",
	"function yylex() {",
	"	.lexer.lex();",
	"	.yylval = .lexer.attachedObject;",
	"	return .lexer.lastToken;",
	"}",
	"function setLexer( object l ) {",
	"	.lexer = l;",
	"}",
	0
};

char *trailer[] = { 0 };

char *jtrailer[] =
{
    "//########## END OF USER-SUPPLIED ACTIONS ##########",
      "    } // switch",
      "    //#### Now let's reduce... ####",
      "    if( .yydebug )",
      "         .debug(\"reduce\");",
      "    .state_drop(.yym);               // we just reduced yylen states",
      "    .yystate = .state_peek(0);       // get new state",
      "    .val_drop(.yym);                 // corresponding value drop",
      "    .yym = .yylhs[.yyn];             // select next TERMINAL(on lhs)",
      "    if( .yystate == 0 && .yym == 0 ) // done? 'rest' state and at first TERMINAL",
      "    {",
      "        .debug(\"After reduction, shifting from state 0 to state \"+",
      FERITE_CLASS_NAME,
      ".YYFINAL+\"\");",
      "        .yystate = ",
      FERITE_CLASS_NAME,
      ".YYFINAL;         //explicitly say we're done",
      "        .state_push(",
      FERITE_CLASS_NAME,
      ".YYFINAL);                          // and save it",
      "        .val_push(.yyval);           // also save the semantic value of parsing",
      "        if( .yychar < 0 )           // we want another character?",
      "        {",
      "           .yychar = .yylex();      // get next character",
      "           if( .yychar < 0 )",
      "               .yychar = 0;         // clean, if necessary",
      "           if( .yydebug )",
      "               .yylexdebug( .yystate, .yychar );",
      "        }",
      "        if( .yychar == 0 )          // Good exit (if lex returns 0 ;-)",
      "           break;                   // quit the loop--all DONE",
      "    } //if yystate",
      "    else                            // else not done yet",
      "    {                               // get next state and push, for next yydefred[]",
      "        .yyn = .yygindex[.yym];       //find out where to go",
      "        if( (.yyn != 0) && (.yyn += .yystate) >= 0 &&",
      "            .yyn <= ",
      FERITE_CLASS_NAME,
      ".YYTABLESIZE && .yycheck[.yyn] == .yystate)",
      "           .yystate = .yytable[.yyn]; //get new state",
      "        else",
      "           .yystate = .yydgoto[.yym]; //else go to new defred",
      "        .debug(\"after reduction, shifting from state \"+.state_peek(0)+\" to state \"+.yystate+\"\");",
      "        .state_push(.yystate);     //going again, so push state & val...",
      "        .val_push(.yyval);         //for next action",
      "     }",
      "  }//main loop",
      "  return 0; //yyaccept!!",
      "}",
      "//## end of method parse() ######################################",
      "\n\n",
      "//## run() --- for Thread #######################################",
      FERITE_RUN,
      "//## end of method run() ########################################",
      "\n\n",
      "//## Constructors ###############################################",
      FERITE_CONSTRUCT,
      "//###############################################################",
      "\n\n",
      "}",
      "//################### END OF CLASS ##############################",
      0
};

void write_section(char **section)
{
    int i;
    FILE *fp;
    fp = code_file;
    for (i = 0; section[i]; ++i)
    {
        ++outline;
        if (strcmp(section[i],FERITE_PACKAGE)==0)  /*Java package name, if any*/
        {
            if (jpackage_name && strlen(jpackage_name)>0)
              fprintf(fp,"package %s;\n",jpackage_name);
        }
        else if (strcmp(section[i],FERITE_CLASS_DECL)==0)
        {
            if (jclass_name && strlen(jclass_name)>0)
              fprintf(fp,"class %s\n",jclass_name);
            else
              fprintf(fp,"class Parser\n");
            if (jextend_name && strlen(jextend_name)>0)
              fprintf(fp,"extends %s\n",jextend_name);
        }
        else if( strcmp( section[i], FERITE_CLASS_NAME) == 0 )
        {
            if (jclass_name && strlen(jclass_name)>0)
              fprintf(fp,"%s",jclass_name);
            else
              fprintf(fp,"Parser");
        }
        else if (strcmp(section[i],FERITE_RUN)==0)
        {
            if (jrun)
            {
                fprintf(fp,"/**\n");
                fprintf(fp," * A default run method, used for operating this parser\n");
                fprintf(fp," * object in the background.  It is intended for extending Thread\n");
                fprintf(fp," * or implementing Runnable.  Turn off with -Jnorun .\n");
                fprintf(fp," */\n");
                fprintf(fp,"public function run()\n");
                fprintf(fp,"{\n");
                fprintf(fp,"  .parse();\n");
                fprintf(fp,"}\n");
            }
            else
            {
                fprintf(fp,"//## The -Jnorun option was used ##\n");
            }
        }
        else if (strcmp(section[i],FERITE_CONSTRUCT)==0)
        {
            if (jconstruct)
            {
                fprintf(fp,"/**\n");
                fprintf(fp," * Default constructor.  Turn off with -Jnoconstruct .\n\n");
                fprintf(fp," */\n");
                fprintf(fp,"constructor()\n",jclass_name);
                fprintf(fp,"{\n");
                output_array_initialisations( fp );
                fprintf(fp,"}\n");
                fprintf(fp,"\n\n");
                fprintf(fp,"/**\n");
                fprintf(fp," * Create a parser, setting the debug to true or false.\n");
                fprintf(fp," * @param debugMe true for debugging, false for no debug.\n");
                fprintf(fp," */\n");
                fprintf(fp,"constructor( number debugMe )\n",jclass_name);
                fprintf(fp,"{\n");
                fprintf(fp,"  self();\n", jclass_name );
                fprintf(fp,"  .yydebug = debugMe;\n");
                fprintf(fp,"}\n");
            }
            else
            {
                fprintf(fp,"//## The -Jnoconstruct option was used ##\n");
            }
        }
        else if (strcmp(section[i],FERITE_STACK)==0)
        {
            fprintf(fp,
                    "final static number YYSTACKSIZE = %d;  //maximum stack size",
                    jstack_size);
        }
        else
          fprintf(fp, "%s\n", section[i]);
    }
}

