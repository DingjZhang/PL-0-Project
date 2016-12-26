#include <stdio.h>

#define NRW        28     // number of reserved words
#define TXMAX      500    // length of identifier table
#define MAXNUMLEN  14     // maximum number of digits in numbers
#define NSYM       14     // maximum number of symbols in array ssym and csym
#define MAXIDLEN   10     // length of identifiers

#define MAXADDRESS 32767  // maximum address
#define MAXLEVEL   32     // maximum depth of nesting block
#define CXMAX      500    // size of code array

#define MAXSYM     30     // maximum number of symbols  

#define STACKSIZE  1000   // maximum storage
#define MAXDIM     5      // maximum dim of array
#define MAX_ROW 1000 //程序文件的最大行数
#define MAX_COL 100	//程序文件的最大列数

#define BIN_PATH "D:\\学习\\大三上\\编译原理\\compiler_project\\working_dir\\bin.txt"

enum symtype {
	SYM_NULL,
	SYM_IDENTIFIER, /*1*/
	SYM_NUMBER, /*2*/
	SYM_PLUS,
	SYM_MINUS,
	SYM_TIMES,
	SYM_SLASH,
	SYM_ODD,
	SYM_EQU,
	SYM_NEQ,
	SYM_LES,
	SYM_LEQ, /*11*/
	SYM_GTR,
	SYM_GEQ, 
	SYM_LPAREN,/*14*/
	SYM_RPAREN,
	SYM_COMMA,
	SYM_SEMICOLON, /*17*/
	SYM_PERIOD,
	SYM_BECOMES,/*19*/
    SYM_BEGIN, /*20*/
	SYM_END,
	SYM_IF, /*22*/
	SYM_THEN, /*23*/
	SYM_WHILE,
	SYM_DO,
	SYM_CALL,
	SYM_CONST,
	SYM_VAR, /*28*/
	SYM_PROCEDURE,
	//自己添加
	SYM_ELSE, /*30*/
	SYM_ENDIF,
	SYM_FOR,
	SYM_EXIT,
	SYM_PRINTF,
	SYM_SCANF,
	SYM_READ,
	SYM_WRITE,
	SYM_REPEAT,
	SYM_UNTIL,
	SYM_BY,
	SYM_DOWNTO,
	SYM_TO,
	SYM_ARR,
	SYM_LBRACKET,          //[
	SYM_RBRACKET,           //]
	SYM_BREAK,
	SYM_NOT,
	SYM_AND,
	SYM_OR
};

enum idtype {
	ID_CONSTANT, ID_VARIABLE, ID_PROCEDURE
};

enum opcode {
	LIT, OPR, LOD, STO, CAL, INT, JMP, JPC, WRT, RED, NOT,AND,OR,JPD,JPE
};

enum oprcode {
	OPR_RET, OPR_NEG, OPR_ADD, OPR_MIN,
	OPR_MUL, OPR_DIV, OPR_ODD, OPR_EQU,
	OPR_NEQ, OPR_LES, OPR_LEQ, OPR_GTR,
	OPR_GEQ, OPR_NOT, OPR_AND, OPR_OR
};

//jpc 栈顶是 0 则跳转

typedef struct {//类PCODE指令类型，包含三个字段：指令f、层差l和另一个操作数a
	int var_addr;    //变量和常量在符号表中的地址
	int f; // function code
	int l; // level
	int a; // displacement address
} instruction;

//////////////////////////////////////////////////////////////////////
char* err_msg[] =
{
/*  0 */    "",
/*  1 */    "Found '==' when expecting '='.",
/*  2 */    "There must be a number to follow '='.",
/*  3 */    "There must be an '=' to follow the identifier.",
/*  4 */    "There must be an identifier to follow 'const', 'var', or 'procedure'.",
/*  5 */    "Missing ',' or ';'.",
/*  6 */    "Incorrect procedure name.",
/*  7 */    "Statement expected.",
/*  8 */    "Follow the statement is an incorrect symbol.",
/*  9 */    "'.' expected.",
/* 10 */    "';' expected.",
/* 11 */    "Undeclared identifier.",
/* 12 */    "Illegal assignment.",
/* 13 */    "'=' expected.",
/* 14 */    "There must be an identifier to follow the 'call'.",
/* 15 */    "A constant or variable can not be called.",
/* 16 */    "'then' expected.",
/* 17 */    "';' or 'end' expected.",
/* 18 */    "'do' expected.",
/* 19 */    "Incorrect symbol.",
/* 20 */    "Relative operators expected.",
/* 21 */    "Procedure identifier can not be in an expression.",
/* 22 */    "Missing ')'.",
/* 23 */    "The symbol can not be followed by a factor.",
/* 24 */    "The symbol can not be as the beginning of an expression.",
/* 25 */    "The number is too great.",
/* 26 */    "'endif' expected.",
/* 27 */    "'(' expected.",
/* 28 */    "';'; expected.",
/* 29 */    "'&' expected.",
/* 30 */    "'|' expected.",
/* 31 */    "",
/* 32 */    "There are too many levels."
};

//////////////////////////////////////////////////////////////////////
char ch;         // last character read
int  sym;        // last symbol read存放最近一次识别出来的token 的类型
char id[MAXIDLEN + 1]; // last identifier read
int  num;        // last number read
int  cc;         // character count
int  ll;         // line length
int  kk;
int  err;
int  cx;         // index of current instruction to be generated.
int  cx_and[10];
int  cx_or[10];
int  i_and = 0;
int  i_or = 0;
int  level = 0;
int  tx = 0;


char line[150];

instruction code[CXMAX];

char* word[NRW + 1] =//保留字表
{
	"", /* place holder */
	"begin", "call", "const", "do", "end","if",
	"odd", "procedure", "then", "var", "while",
	//自己添加
	"else", "endif", "for", "exit", "printf", "scanf", "read",
	"write", "repeat", "until", "by", "downto", "to", "break","not","and","or"
};

int wsym[NRW + 1] = {
	SYM_NULL, SYM_BEGIN, SYM_CALL, SYM_CONST, SYM_DO, SYM_END,
	SYM_IF, SYM_ODD, SYM_PROCEDURE, SYM_THEN, SYM_VAR, SYM_WHILE,
	//自己添加
	SYM_ELSE, SYM_ENDIF, SYM_FOR, SYM_EXIT, SYM_PRINTF, SYM_SCANF,SYM_READ,
	SYM_WRITE,
	SYM_REPEAT,
	SYM_UNTIL,
	SYM_BY,
	SYM_DOWNTO,
	SYM_TO,
	SYM_BREAK,
	SYM_NOT,
	SYM_AND,
	SYM_OR
};

int ssym[NSYM + 1] = {
	SYM_NULL, SYM_PLUS, SYM_MINUS, SYM_TIMES, SYM_SLASH,
	SYM_LPAREN, SYM_RPAREN, SYM_COMMA, SYM_PERIOD, SYM_SEMICOLON,
	SYM_LBRACKET, SYM_RBRACKET,//]
	SYM_NOT,SYM_AND,SYM_OR
};

char csym[NSYM + 1] = {
	' ', '+', '-', '*', '/', '(', ')', ',', '.', ';', '[', ']','!','&','|'
};

#define MAXINS   15
char* mnemonic[MAXINS] = {
	"LIT", "OPR", "LOD", "STO", "CAL", "INT", "JMP", "JPC", "WRT", "RED","NOT","AND","OR","JPD","JPE"
};

#define OPR_NUM 16
char *operation[OPR_NUM] = {
	"RET", "NEG", "ADD", "MIN", "MUL", "DIV", "ODD", "EQU", "NEQ", "LES", "LEQ", "GTR", "GEQ","NOT","AND","OR"
};

typedef struct comtab {
	char name[MAXIDLEN + 1];
	int  kind;
	int  value;
} comtab;

comtab table[TXMAX];

typedef struct mask {
	char  name[MAXIDLEN + 1];
	int   kind;
	short level;
	short address;
} mask;

FILE *infile;

int code_cx1 = 0;
int comment_state = 0;   //comment_state = 1 代表位于块注释内部
int  jx;         /*记录循环的出口地址，为break语句作准备*/
int  jpx;        /*记录过程的出口地址，为exit语句作准备*/



instruction temp_code[CXMAX];
int temp_cx = 1;
int cx_for_1;
int temp_cx_flag = 0; //为1表示指令应该存入temp_code中
int end_addr; //程序最后一句话的地址 即RET的地址
#define MAXEXIT 10
int exit_x = 0;
int exit_cx[MAXEXIT];

int line_num = 0;

// EOF PL0.h
