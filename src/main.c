#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>

#ifndef __WIN32__  /*rwj  --  make portable*/
# include <signal.h>
#else
# include <dir.h>
#endif

#include "defs.h"

char dflag;
char lflag;
char rflag;
char tflag;
char vflag;

/*##### JAVA FLAGS ####*/
char jflag;             /* Are we generating Java?  Yes/No                  */
char *jclass_name;      /* Class name of this parser.   Default = "Parser"  */
char *jpackage_name;    /* Value of "package XXXXX;"    Default = none      */
char *jextend_name;     /* Value of "implements XXXXX"  Default = none      */
char *jimplement_name;  /* Value of "extends XXXXX"     Default = none      */
char *jsemantic_type;   /* Class name of semantic value                     */
char jrun;              /* Do we provide a run()?    Yes/No  Default y      */
char jconstruct;        /* Do we provide constructors?  Yes/No  Default y   */
int  jstack_size;       /* Semantic value stack size                        */
char jlexer; /* Do we supply a lexer framework */

char *file_prefix = "y";
char *myname      = "yacc";
char *temp_form   = "yacc.XXXXXXX";

int lineno;
int outline;

char *action_file_name;
char *code_file_name;
char *defines_file_name;
char *input_file_name = "";
char *output_file_name;
char *text_file_name;
char *union_file_name;
char *verbose_file_name;

FILE *action_file;	/*  a temp file, used to save actions associated    */
			/*  with rules until the parser is written	    */
FILE *code_file;	/*  y.code.c (used when the -r option is specified) */
FILE *defines_file;	/*  y.tab.h					    */
FILE *input_file;	/*  the input file				    */
FILE *output_file;	/*  y.tab.c					    */
FILE *text_file;	/*  a temp file, used to save text until all	    */
			/*  symbols have been defined			    */
FILE *union_file;	/*  a temp file, used to save the union		    */
			/*  definition until all symbol have been	    */
			/*  defined					    */
FILE *verbose_file;	/*  y.output					    */

int nitems;
int nrules;
int nsyms;
int ntokens;
int nvars;

int   start_symbol;
char  **symbol_name;
short *symbol_value;
short *symbol_prec;
char  *symbol_assoc;

short *ritem;
short *rlhs;
short *rrhs;
short *rprec;
char  *rassoc;
short **derives;
char *nullable;

void done(int k)
{
    if (action_file) { fclose(action_file); unlink(action_file_name); }
    if (text_file) { fclose(text_file); unlink(text_file_name); }
    if (union_file) { fclose(union_file); unlink(union_file_name); }
    exit(k);
}

void onintr(int flag)
{
    done(1);
}

void set_signals(void)
{
#ifndef __WIN32__   /*rwj -- make portable*/
    if (signal(SIGINT, SIG_IGN) != SIG_IGN)
      signal(SIGINT, onintr);
    if (signal(SIGTERM, SIG_IGN) != SIG_IGN)
      signal(SIGTERM, onintr);
    if (signal(SIGHUP, SIG_IGN) != SIG_IGN)
      signal(SIGHUP, onintr);
#endif
}

void usage(void)
{
    fprintf(stderr,
	    "usage:\n %s [-dlrtvj] [-b file_prefix] [-Joption] filename\n", myname);
    fprintf(stderr,"  where -Joption is one or more of:\n");
    fprintf(stderr,"   -J\n");
    fprintf(stderr,"   -Jclass=className\n");
    fprintf(stderr,"   -Jpackage=packageName\n");
    fprintf(stderr,"   -Jextends=extendName\n");
    fprintf(stderr,"   -Jimplements=implementsName\n");
    fprintf(stderr,"   -Jsemantic=semanticType\n");
    fprintf(stderr,"   -Jnorun\n");
    fprintf(stderr,"   -Jnoconstruct\n");
    fprintf(stderr,"   -Jstack=SIZE   (default 500)\n");
	fprintf(stderr,"   -Jnolexer (don't supply setLexer and yylex functions)\n");
    exit(1);
}

void setJavaDefaults(void)
{
    jflag          =1;
    jclass_name    ="Parser"; /* Class name of this parser.   Default = "Parser"  */
    jpackage_name  ="";       /* Value of "package XXXXX;"    Default = none      */
    jextend_name   ="";       /* Value of "implements XXXXX"  Default = none      */
    jimplement_name="";       /* Value of "extends XXXXX"     Default = none      */
    jsemantic_type ="";       /* Type of semantic value       Default = none      */
    jrun           =1;        /* Provide a default run method                     */
    jconstruct     =1;        /* Provide default constructors                     */
    jstack_size    =500;      /* Default stack size                               */
	jlexer         =1;
}

void getJavaArg(char *option)
{
    int len;
    jflag=1;
    if (!option || !(*option))
      return;
    len=strlen(option);
    if (strncmp("class=",option,6)==0 && len>6)
      jclass_name=&(option[6]);
    else if (strncmp("package=",option,8)==0 && len>8)
      jpackage_name=&(option[8]);
    else if (strncmp("extends=",option,8)==0 && len>8)
      jextend_name=&(option[8]);
    else if (strncmp("implements=",option,11)==0 && len>11)
      jimplement_name=&(option[11]);
    else if (strncmp("semantic=",option,9)==0 && len>9)
      jsemantic_type=&(option[9]);
    else if (strcmp("norun",option)==0)
      jrun=0;
    else if (strcmp("noconstruct",option)==0)
      jconstruct=0;
    else if (strncmp("stack=",option,6)==0 && len>6)
      jstack_size=atoi(&(option[6]));
  else if (strcmp("nolexer",option)==0)
    jlexer = 0;
}

void getargs(int argc,char **argv)
{
    int i;
    char *s;

    setJavaDefaults();   /* rwj */

    if (argc > 0) myname = argv[0];
    for (i = 1; i < argc; ++i)
    {
	s = argv[i];
	if (*s != '-') break;
	switch (*++s)
	{
	  case '\0':
	    input_file = stdin;
	    if (i + 1 < argc) usage();
	    return;

	  case '-':
	    ++i;
	    goto no_more_options;

	  case 'b':
	    if (*++s)
	      file_prefix = s;
	    else if (++i < argc)
	      file_prefix = argv[i];
	    else
	      usage();
	    continue;

	  case 'd':
	    dflag = 1;
	    break;

	  case 'l':
	    lflag = 1;
	    break;

	  case 'r':
	    rflag = 1;
	    break;

	  case 't':
	    tflag = 1;
	    break;

	  case 'v':
	    vflag = 1;
	    break;

	  case 'J':     /* rwj -- for Java!  */
	    jflag = 1;
            getJavaArg(&(s[1]));
	    continue;

	  default:
	    usage();
	}

	for (;;)      /*single letter options     ex:   yacc -iJr file.y */
	{
	    switch (*++s)
	    {
	      case '\0':
		goto end_of_option;

	      case 'd':
		dflag = 1;
		break;

	      case 'l':
		lflag = 1;
		break;

	      case 'r':
		rflag = 1;
		break;

	      case 't':
		tflag = 1;
		break;

	      case 'v':
		vflag = 1;
		break;

	      case 'J':        /* rwj -- for java*/
		jflag = 1;
		break;

	      default:
		usage();
	    }
	}
	end_of_option:;
    }

    no_more_options:;
    if (i + 1 != argc) usage();
    input_file_name = argv[i];
}

char *allocate(unsigned n)
{
    char *p;

    p = NULL;
    if (n)
    {
	p = CALLOC(1, n);
	if (!p) no_space();
    }
    return (p);
}

void create_file_names(void)
{
    int i, len;
    char *tmpdir;

    tmpdir = getenv("TMPDIR");
#ifdef __WIN32__  /*rwj -- make portable*/
    if (tmpdir == 0) tmpdir = ".";
#else
    if (tmpdir == 0) tmpdir = "/tmp";
#endif

    len = strlen(tmpdir);
    i = len + 13;
    if (len && tmpdir[len-1] != '/')
      ++i;

    action_file_name = MALLOC(i);
    if (action_file_name == 0) no_space();
    text_file_name = MALLOC(i);
    if (text_file_name == 0) no_space();
    union_file_name = MALLOC(i);
    if (union_file_name == 0) no_space();

    strcpy(action_file_name, tmpdir);
    strcpy(text_file_name, tmpdir);
    strcpy(union_file_name, tmpdir);

    if (len && tmpdir[len - 1] != '/')
    {
	action_file_name[len] = '/';
	text_file_name[len] = '/';
	union_file_name[len] = '/';
	++len;
    }

    strcpy(action_file_name + len, temp_form);
    strcpy(text_file_name + len, temp_form);
    strcpy(union_file_name + len, temp_form);

    action_file_name[len + 5] = 'a';
    text_file_name[len + 5] = 't';
    union_file_name[len + 5] = 'u';

    mktemp(action_file_name);
    mktemp(text_file_name);
    mktemp(union_file_name);

    if (jflag)/*rwj*/
    {
	len = strlen(jclass_name);

	output_file_name = MALLOC(len + 6);/*rwj for '.java\0' */
	if (output_file_name == 0) no_space();
	strcpy(output_file_name, jclass_name);
	strcpy(output_file_name + len, JAVA_OUTPUT_SUFFIX);
    }
    else
    {
	len = strlen(file_prefix);

	output_file_name = MALLOC(len + 7);
	if (output_file_name == 0) no_space();
	strcpy(output_file_name, file_prefix);
	strcpy(output_file_name + len, OUTPUT_SUFFIX);
    }

    if (rflag)
    {
	code_file_name = MALLOC(len + 8);
	if (code_file_name == 0)
	  no_space();
	strcpy(code_file_name, file_prefix);
	strcpy(code_file_name + len, CODE_SUFFIX);
    }
    else
      code_file_name = output_file_name;

    if (dflag)
    {
	defines_file_name = MALLOC(len + 7);
	if (defines_file_name == 0)
	  no_space();
	strcpy(defines_file_name, file_prefix);
	strcpy(defines_file_name + len, DEFINES_SUFFIX);
    }

    if (vflag)
    {
	verbose_file_name = MALLOC(len + 8);
	if (verbose_file_name == 0)
	  no_space();
	strcpy(verbose_file_name, file_prefix);
	strcpy(verbose_file_name + len, VERBOSE_SUFFIX);
    }
}

void open_files(void)
{
    create_file_names();

    if (input_file == 0)
    {
	input_file = fopen(input_file_name, "r");
	if (input_file == 0)
	  open_error(input_file_name);
    }

    action_file = fopen(action_file_name, "w");
    if (action_file == 0)
      open_error(action_file_name);

    text_file = fopen(text_file_name, "w");
    if (text_file == 0)
      open_error(text_file_name);

    if (vflag)
    {
	verbose_file = fopen(verbose_file_name, "w");
	if (verbose_file == 0)
	  open_error(verbose_file_name);
    }

    if (dflag)
    {
	defines_file = fopen(defines_file_name, "w");
	if (defines_file == 0)
	  open_error(defines_file_name);
	union_file = fopen(union_file_name, "w");
	if (union_file ==  0)
	  open_error(union_file_name);
    }

    output_file = fopen(output_file_name, "w");
    if (output_file == 0)
      open_error(output_file_name);

    if (rflag)
    {
	code_file = fopen(code_file_name, "w");
	if (code_file == 0)
	  open_error(code_file_name);
    }
    else
      code_file = output_file;
}

int main(int argc,char **argv)
{
    set_signals();
    getargs(argc, argv);
    open_files();
    reader();
    lr0();
    lalr();
    make_parser();
    verbose();
    output();
    done(0);
    /*NOTREACHED*/
    return 1;
}

