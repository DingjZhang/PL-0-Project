// pl0 compiler source code
//for Visual Studio
#pragma warning(disable:4996) 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "pl0.h"
#include "set.h"

//////////////////////////////////////////////////////////////////////
// print error message.
void error(int n) {
	int i;

	printf("      ");
	for (i = 1; i <= cc - 1; i++)
		printf(" ");
	printf("^\n");
	printf("Error %3d: %s\n", n, err_msg[n]);
	err++;
} // error

//////////////////////////////////////////////////////////////////////
void getch() {
	if (cc == ll) {
		if (feof(infile)) {
			printf("\nPROGRAM INCOMPLETE\n");
			exit(1);
		}
		ll = cc = 0;
		printf("%5d  ", cx);
		while ( (!feof(infile)) && ((ch = getc(infile)) != '\n')) {	// added & modified by alex 01-02-09
			printf("%c", ch);
			line[++ll] = ch;
		} // while
		line_num++;
		printf("\n");
		line[++ll] = ' ';
		int line_cl = ll; //在line中 "//" 的位置
		int left_cl = ll;	// "/*"的位置
		int right_cl = ll; //"*/的位置"
		int comment_exist = 1; //值为1，表示当前行中存在注释

		while (comment_exist == 1) {
			//做line 和 '//'的串匹配
			for (int i = 1; i < ll - 1; ++i) {
				if (line[i] == '/' && line[i + 1] == '/') {
					line_cl = i;
					break;
				}
			}
			//做line 和 '/*'的串匹配
			for (int i = 1; i < ll - 1; ++i) {
				if (line[i] == '/' && line[i + 1] == '*') {
					left_cl = i;
					break;
				}
			}
			//做line 和 '*/'的串匹配
			for (int i = 1; i < ll - 1; ++i) {
				if (line[i] == '*' && line[i + 1] == '/') {
					right_cl = i;
					break;
				}
			}
			if (left_cl == ll && right_cl == ll && line_cl == ll) {
				comment_exist = 0; //当前行中不存在注释
				if (comment_state == 1) {
					for (int i = 1; i < ll; ++i) {
						line[i] = ' ';
					}
				}
			}
			else { //当前行中存在注释
				if (left_cl < ll && right_cl == ll && line_cl == ll) { //     ......../*........	1
					for (int i = left_cl; i < ll; ++i) {
						line[i] = ' ';
					}
					comment_state = 1;
				}
				else if (left_cl == ll && right_cl == ll && line_cl < ll) { //    ........//........
					for (int i = line_cl; i < ll; ++i) {
						line[i] = ' ';
					}
				}
				else if (left_cl == ll && right_cl < ll && line_cl == ll) { //    ........*/........    0
					for (int i = 1; i < right_cl + 2; ++i) {
						line[i] = ' ';
					}
					comment_state = 0;
				}
				else if (left_cl < ll && right_cl < ll && line_cl == ll) {
					if (left_cl < right_cl) {								//    ......../*.........*/........
						for (int i = left_cl; i < right_cl + 2; ++i) {
							line[i] = ' ';
						}
					}
					else {													//    .........*/........./*........	1
						for (int i = 1; i < right_cl + 2; ++i) {
							line[i] = ' ';
						}
						for (int i = left_cl; i < ll; ++i) {
							line[i] = ' ';
						}
						comment_state = 1;
					}
				}
				else if (left_cl < ll && right_cl == ll && line_cl < ll) {	
					if (left_cl < line_cl) {								//    ......../*........//........	1
						for (int i = left_cl; i < ll; ++i) {
							line[i] = ' ';
						}
						comment_state = 1;
					}
					else{													//    ........//........./*........
						for (int i = line_cl; i < ll; ++i) {
								line[i] = ' ';
						}
					}
				}
				else if (left_cl == ll && right_cl < ll && line_cl < ll) {
					if (right_cl < line_cl) {								//    ........*/..........//........	0
						for (int i = 1; i < right_cl + 2; ++i) {
							line[i] = ' ';
						}
						for (int i = line_cl; i < ll; ++i) {
							line[i] = ' ';
						}
						comment_state = 0;
					}
					else {													//    ........//........*/.........
						for (int i = 1; i < right_cl; ++i) {
							line[i] = ' ';
						}
					}
				}
				else if (left_cl < ll && right_cl < ll && line_cl < ll) {
					if (line_cl < left_cl && left_cl < right_cl) {			//    ........//......../*........*/........
						for (int i = line_cl; i < ll; ++i) {				//    ........//........*/......../*........
							line[i] = ' ';
						}
					}
					else if (left_cl < line_cl && line_cl < right_cl) {		//    ......../*........//........*/........
						for (int i = left_cl; i < right_cl + 2; ++i) {
							line[i] = ' ';
						}
					}													
					else if (left_cl < right_cl && right_cl < line_cl) {	//    ......../*........*/........//........
						for (int i = left_cl; i < right_cl + 2; ++i) {
							line[i] = ' ';
						}
						for (int i = line_cl; i < ll; ++i) {
							line[i] = ' ';
						}
					}
					else if (right_cl < left_cl && left_cl < line_cl) {		//    ........*/......../*........//........	1
						for (int i = 1; i < right_cl + 2; ++i) {
							line[i] = ' ';
						}
						for (int i = left_cl; i < ll; ++i) {
							line[i] = ' ';
						}
						comment_state = 1;
					}
					else {													//    ........*/........//......../*........	1
						for (int i = 1; i < right_cl + 2; ++i) {
							line[i] = ' ';
						}
						for (int i = line_cl; i < ll; ++i) {
							line[i] = ' ';
						}
						comment_state = 1;
					}
				}
				left_cl = ll; right_cl = ll; line_cl = ll;
			}
		}	
	}

	ch = line[++cc];
} // getch

//////////////////////////////////////////////////////////////////////
// gets a symbol from input stream.
void getsym() {
	int i, k;
	char a[MAXIDLEN + 1];

	while (ch == ' ' || ch == '\t') {
		getch();
	}
	if (isalpha(ch)) { // symbol is a reserved word or an identifier.
		k = 0;
		do {
			if (k < MAXIDLEN) {
				a[k++] = ch;
			}
			getch();
		} while (isalpha(ch) || isdigit(ch));
		a[k] = 0;
		strcpy(id, a);
		word[0] = id;
		i = NRW;
		while (strcmp(id, word[i--]));
		if (++i)
			sym = wsym[i]; // symbol is a reserved word
		else {
			sym = SYM_IDENTIFIER;   // symbol is an identifier
		}	
	}
	else if (isdigit(ch))
	{ // symbol is a number.
		k = num = 0;
		sym = SYM_NUMBER;
		do {
			num = num * 10 + ch - '0';
			k++;
			getch();
		} while (isdigit(ch));
		if (k > MAXNUMLEN)
			error(25);     // The number is too great.
	}
	else if (ch == '=') {
		getch();
		if (ch == '=') {
			//sym = SYM_BECOMES; 
			// :=
			sym = SYM_EQU; //==
			getch();
		}
		else {
			sym = SYM_BECOMES;       // =
		}
	}
	else if (ch == '>') {
		getch();
		if (ch == '=') {
			sym = SYM_GEQ;     // >=
			getch();
		}
		else {
			sym = SYM_GTR;     // >
		}
	}
	else if (ch == '<') {
		getch();
		if (ch == '=') {
			sym = SYM_LEQ;     // <=
			getch();
		}
		//else if (ch == '>')
		//{
		//	sym = SYM_NEQ;     // <>
		//	getch();
		//}
		else {
			sym = SYM_LES;     // <
		}
	}
	else if (ch == '!') {
		getch();
		if (ch == '=') {
			sym = SYM_NEQ; //!=
			getch();
		}
		else {
			sym = SYM_NOT;//!not
			//printf("Fatal Error: Unknown character.\n");
			//exit(1);
		}
	}
	else if (ch == '&')//如果读到&
	{
		getch();
		//sym = SYM_AND;     // 
		if (ch == '&') {
			sym = SYM_AND;
			getch();
		}
		else {
			error(29); //"'&' expected."
		}
	}
	else if (ch == '|')//如果读到| 
	{
		//getch();
		//sym = SYM_OR;     // 
		getch();
		//sym = SYM_AND;     // 
		if (ch == '|') {
			sym = SYM_OR;
			getch();
		}
		else {
			error(30); //"'|' expected."
		}
	}
	else { // other tokens
		i = NSYM;
		csym[0] = ch;
		while (csym[i--] != ch);
		if (++i) {
			sym = ssym[i];
			getch();
		}
		else {
			printf("Fatal Error: Unknown character.\n");
			exit(1);
		}
	}
} // getsym

//////////////////////////////////////////////////////////////////////
// generates (assembles) an instruction.
void gen(int x, int y, int z) {
	if (cx > CXMAX) {
		printf("Fatal Error: Program too long.\n");
		exit(1);
	}
	code[cx].f = x;
	code[cx].l = y;
	code[cx++].a = z;
} // gen


//自己添加
void temp_gen(int var_addr, int f, int l, int a) {
	if (temp_cx > CXMAX) {
		printf("Fatal Error: Program too long.\n");
		exit(1);
	}
	temp_code[temp_cx].a = a;
	temp_code[temp_cx].f = f;
	temp_code[temp_cx].l = l;
	temp_code[temp_cx].var_addr = var_addr;
	temp_cx++;
} // temp_gen

//////////////////////////////////////////////////////////////////////
// tests if error occurs and skips all symbols that do not belongs to s1 or s2.
//测试当前单词符号是否合法（错误处理）
//sym 不在 s1 里代表发生了错误；然后不停地 getsym() 直到读到一个 s1 或 s2 的 sym.
void test(symset s1, symset s2, int n) {
	symset s;
	if (!inset(sym, s1)) {
		error(n);
		s = uniteset(s1, s2);
		while (!inset(sym, s)) {
			getsym(); 
		}
		destroyset(s);
	}
} // test

//////////////////////////////////////////////////////////////////////
int dx;  // data allocation index

// enter object(constant, variable or procedre) into table.
//登记符号表
void enter(int kind) {
	//if (position(id)> 0)
	//	error(26);
	mask* mk;
	tx++;
	strcpy(table[tx].name, id);
	table[tx].kind = kind;
	switch (kind) {
	case ID_CONSTANT:
		if (num > MAXADDRESS) {
			error(25); // The number is too great.
			num = 0;
		}
		table[tx].value = num;
		break;
	case ID_VARIABLE:
		mk = (mask*) &table[tx];
		mk->level = level;
		mk->address = dx++;
		break;
	case ID_PROCEDURE:
		mk = (mask*) &table[tx];
		mk->level = level;
		break;
	} // switch
} // enter

//////////////////////////////////////////////////////////////////////
// locates identifier in symbol table.
//查找标识符在符号表中的位置
int position(char* id) {
	int i;
	strcpy(table[0].name, id);
	i = tx + 1;
	while (strcmp(table[--i].name, id) != 0);
	return i;
} // position

//////////////////////////////////////////////////////////////////////
//常量定义处理
void constdeclaration() {
	if (sym == SYM_IDENTIFIER) {
		getsym();
		if (sym == SYM_EQU || sym == SYM_BECOMES) {
			//if (sym == SYM_BECOMES)
			//	error(1); // Found '=' when expecting '=='.
			//自己改正，感觉上边的程序错了
			if (sym == SYM_EQU) {
				error(1); // Found '==' when expecting '='.
			}
			getsym();
			if (sym == SYM_NUMBER) {
				enter(ID_CONSTANT);
				getsym();
			}
			else {
				error(2); // There must be a number to follow '='.
			}
		}
		else {
			error(3); // There must be an '=' to follow the identifier.
		}
	} 
	else {
		error(4); // There must be an identifier to follow 'const', 'var', or 'procedure'.
	}
} // constdeclaration
void dimdeclaration(void)
{
	int dim[5];
	int i = 0, j;
	int address = 1;

	do
	{
		if (i > MAXDIM)
		{
			error(43);
		}
		else
		{
			getsym();
			if (sym == SYM_NUMBER)
			{
				getsym();
				if (sym == SYM_RBRACKET)
				{
					i++;
					dim[i] = num;
					if (num < 1)
					{
						error(44);
					}
					getsym();
				}
				else
				{
					error(42);
				}
			}
			else
			{
				if (sym == SYM_IDENTIFIER)
				{
					if ((j = position(id)) == 0)
					{
						error(11);
					}
					else
					{
						getsym();
						if (sym == SYM_RBRACKET)
						{
							i++;
							dim[i] = table[j].value;
							getsym();
						}
					}
				}
				else if (sym == SYM_RBRACKET)
				{
					error(41);
				}
				else
				{
					error(42);
				}
			}
		}
	} while (sym == SYM_LBRACKET);
	while (i > 0)
	{
		address *= dim[i];
		i--;
	}
	for (i = 1; i <= address - 1; i++)
	{
		sprintf(id, "%s%d", id, i);
		enter(ID_VARIABLE);
	}
}
//////////////////////////////////////////////////////////////////////
//变量定义处理
void vardeclaration(void) {
	if (sym == SYM_IDENTIFIER) {
		enter(ID_VARIABLE);
		getsym();
	}
	else {
		error(4); // There must be an identifier to follow 'const', 'var', or 'procedure'.
	}
	if (sym == SYM_LBRACKET) {
		dimdeclaration();
	}
} // vardeclaration

//////////////////////////////////////////////////////////////////////
void listcode(int from, int to) {
	int i;
	printf("\n");
	for (i = from; i < to; i++) {
		//printf("%5d %s\t%d\t%d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
		//自己更改输出的语言形式
		if( (mnemonic[code[i].f] == "OPR")&&(code[i].a <= 12) ){
			printf("%5d %s\n", i, operation[code[i].a]);
		}
		else if (mnemonic[code[i].f] == "LOD" || mnemonic[code[i].f] == "STO" || mnemonic[code[i].f] == "CAL") {
			printf("%5d %s\t%d\t%d\n", i, mnemonic[code[i].f], code[i].l, code[i].a);
		}
		else {
			printf("%5d %s\t \t%d\n", i, mnemonic[code[i].f], code[i].a);
		}
	}
	printf("\n");
} // listcode
int dimassign(void)
{
	int dim[5];
	int i = 0, j;
	int address = 1;

	do
	{
		if (i > MAXDIM)
		{
			error(28);
		}
		else
		{
			getsym();
			if (sym == SYM_NUMBER)
			{
				getsym();
				if (sym == SYM_RBRACKET)
				{
					i++;
					dim[i] = num + 1;
					getsym();
				}
				else
				{
					error(27);
				}
			}
			else
			{
				if (sym == SYM_IDENTIFIER)
				{
					if ((j = position(id)) == 0)
					{
						error(11);
					}
					else
					{
						getsym();
						if (sym == SYM_RBRACKET)
						{
							i++;
							dim[i] = table[j].value + 1;
							getsym();
						}
					}
				}
				else if (sym == SYM_RBRACKET)
				{
					error(41);
				}
				else
				{
					error(42);
				}
			}
		}
	} while (sym == SYM_LBRACKET);
	while (i > 0)
	{
		address *= dim[i];
		i--;
	}
	return address - 1;
}
//////////////////////////////////////////////////////////////////////
//因子分析处理
void factor(symset fsys) {
	void expression(symset fsys);
	int i,k=0;
	symset set;
	char idt[MAXIDLEN + 1];
	test(facbegsys, fsys, 24); // The symbol can not be as the beginning of an expression.

	while (inset(sym, facbegsys)) {
		if (sym == SYM_IDENTIFIER)
		{
			strcpy(idt, id);
			getsym();
			if (sym == SYM_LBRACKET)
			{
				k = dimassign();
			}
			if ((i = position(idt) + k) == 0)
			{
				error(11); // Undeclared identifier.
			}
			else
			{
				switch (table[i].kind)
				{
					mask* mk;
				case ID_CONSTANT:
					gen(LIT, 0, table[i].value);
					break;
				case ID_VARIABLE:
					mk = (mask*)&table[i];
					gen(LOD, level - mk->level, mk->address);
					break;
				case ID_PROCEDURE:
					error(21); // Procedure identifier can not be in an expression.
					break;
				} // switch
			}
		}
		else if (sym == SYM_NUMBER) {
			if (num > MAXADDRESS) {
				error(25); // The number is too great.
				num = 0;
			}
			if (temp_cx_flag == 0) {
				gen(LIT, 0, num);
			}
			else {
				temp_gen(0, LIT, 0, num);
			}
			//gen(LIT, 0, num);
			getsym();
		}
		else if (sym == SYM_LPAREN) {
			getsym();
			set = uniteset(createset(SYM_RPAREN, SYM_NULL), fsys);
			expression(set);
			destroyset(set);
			if (sym == SYM_RPAREN) {
				getsym();
			}
			else {
				error(22); // Missing ')'.
			}
		}
		//test(fsys, createset(SYM_LPAREN, SYM_NULL), 23);
	} // while
} // factor

//////////////////////////////////////////////////////////////////////
//项分析处理
void term(symset fsys) {
	int mulop;
	symset set;
	set = uniteset(fsys, createset(SYM_TIMES, SYM_SLASH, SYM_NULL));
	factor(set); //因子处理
	while (sym == SYM_TIMES || sym == SYM_SLASH) {
		mulop = sym;
		getsym();
		factor(set);
		if (mulop == SYM_TIMES) {
			if (temp_cx_flag == 0) {
				gen(OPR, 0, OPR_MUL);
			}
			else {
				temp_gen(0, OPR, 0, OPR_MUL);
			}
			//gen(OPR, 0, OPR_MUL);
		}
		else {
			if (temp_cx_flag == 0) {
				gen(OPR, 0, OPR_DIV);
			}
			else {
				temp_gen(0, OPR, 0, OPR_DIV);
			}
			//gen(OPR, 0, OPR_DIV);
		}
	} // while
	destroyset(set);
} // term

//////////////////////////////////////////////////////////////////////
//表达式分析处理
void expression(symset fsys){
	int addop;
	symset set;

	set = uniteset(fsys, createset(SYM_PLUS, SYM_MINUS, SYM_NULL));
	if (sym == SYM_PLUS || sym == SYM_MINUS) {
		addop = sym;
		getsym();
		term(set);
		if (addop == SYM_MINUS) {
			if (temp_cx_flag == 0) {
				gen(OPR, 0, OPR_NEG);
			}
			else {
				temp_gen(0, OPR, 0, OPR_NEG);
			}
			//gen(OPR, 0, OPR_NEG);
		}
	}
	else {
		term(set);
	}

	while (sym == SYM_PLUS || sym == SYM_MINUS) {
		addop = sym;
		getsym();
		term(set);
		if (addop == SYM_PLUS) {
			if (temp_cx_flag == 0) {
				gen(OPR, 0, OPR_ADD);
			}
			else {
				temp_gen(0, OPR, 0, OPR_ADD);
			}
			//gen(OPR, 0, OPR_ADD);
		}
		else {
			if (temp_cx_flag == 0) {
				gen(OPR, 0, OPR_MIN);
			}
			else {
				temp_gen(0, OPR, 0, OPR_MIN);
			}
			//gen(OPR, 0, OPR_MIN);
		}
	} // while

	destroyset(set);
} // expression

//////////////////////////////////////////////////////////////////////
//条件分析处理
void condition(symset fsys);//声明 
void condition_1(symset fsys)//条件分析处理
{
	int relop;//用于临时记录token的内容 
	symset set;
	
	if(sym == SYM_NOT)
	{
		getsym(); //获取下一个token    
		if (sym == SYM_ODD)//如果是odd运算符(一元)   
		{
			getsym(); //获取下一个token
			expression(fsys); //对odd的表达式进行处理计算   
			gen(OPR, 0, 6);//生成6号操作指令：奇偶判断运算   
		}
		else if(sym == SYM_LPAREN)
		{
			getsym(); //获取一个token    
			set = uniteset(createset(SYM_RPAREN, SYM_NULL), fsys);
			condition(set);//递归调用expression子程序分析一个子表达式   
			destroyset(set);
			if (sym == SYM_RPAREN)//子表达式分析完后，应遇到右括号   
			{
				getsym(); //如果的确遇到右括号，读取下一个token   
			}
			else
			{
				error(22); // Missing ')'.
			}
		}
		else/* 如果不是odd 运算符(那就一定是二元逻辑运算符) */
		{
			set = uniteset(relset, fsys);
			expression(set);//对表达式左部进行处理计算   
			destroyset(set);
			if (! inset(sym, relset))// 如果token不是逻辑运算符中的一个   
			{
				
				//error(20);
			}
			else
			{
				relop = sym; //记录下当前的逻辑运算符   
				getsym();//获取下一个token    
				expression(fsys); //对表达式右部进行处理计算    
				switch (relop)//如果刚才的运算符是下面的一种   
				{
				case SYM_EQU:
					gen(OPR, 0, OPR_EQU);
					break;
				case SYM_NEQ:
					gen(OPR, 0, OPR_NEQ);
					break;
				case SYM_LES:
					gen(OPR, 0, OPR_LES);
					break;
				case SYM_GEQ:
					gen(OPR, 0, OPR_GEQ);
					break;
				case SYM_GTR:
					gen(OPR, 0, OPR_GTR);
					break;
				case SYM_LEQ:
					gen(OPR, 0, OPR_LEQ);
					break;
				} // switch
			} // else
		}// else
		gen(OPR, 0,OPR_NOT);//生成13号操作指令：not判断运算
	}
	//如果不是由NOT开头
	else
	{
		if (sym == SYM_ODD)//如果是odd运算符(一元)   
		{
			getsym(); //获取下一个token
			expression(fsys); //对odd的表达式进行处理计算   
			gen(OPR, 0, 6);//生成6号操作指令：奇偶判断运算   
		}
		else if(sym == SYM_LPAREN)
		{
			getsym(); //获取一个token    
			set = uniteset(createset(SYM_RPAREN, SYM_NULL), fsys);
			condition(set);//递归调用expression子程序分析一个子表达式   
			destroyset(set);
			if (sym == SYM_RPAREN)//子表达式分析完后，应遇到右括号   
			{
				getsym(); //如果的确遇到右括号，读取下一个token   
			}
			else
			{
				error(22); // Missing ')'.
			}
		}
		else/* 如果不是odd 运算符(那就一定是二元逻辑运算符) */
		{
			set = uniteset(relset, fsys);
			expression(set);//对表达式左部进行处理计算   
			destroyset(set);
			if (! inset(sym, relset))// 如果token不是逻辑运算符中的一个   
			{
				//error(20);
			}
			else
			{
				relop = sym; //记录下当前的逻辑运算符   
				getsym();//获取下一个token    
				expression(fsys); //对表达式右部进行处理计算    
				switch (relop)//如果刚才的运算符是下面的一种   
				{
				case SYM_EQU:
					gen(OPR, 0, OPR_EQU);
					break;
				case SYM_NEQ:
					gen(OPR, 0, OPR_NEQ);
					break;
				case SYM_LES:
					gen(OPR, 0, OPR_LES);
					break;
				case SYM_GEQ:
					gen(OPR, 0, OPR_GEQ);
					break;
				case SYM_GTR:
					gen(OPR, 0, OPR_GTR);
					break;
				case SYM_LEQ:
					gen(OPR, 0, OPR_LEQ);
					break;
				} // switch
			} // else
		}// else		
	}

}// condition_1

void condition_and(symset fsys)
{
	symset set;
	//int cx1_and;
	set = uniteset(fsys, createset(SYM_AND, SYM_NULL));
	condition_1(set);//每一个condition_and都应该由condition_1开始
	while (sym == SYM_AND)//一个condition_1后应当遇到AND   
	{ 
		cx_and[i_and] = cx;
		gen(JPE,0,0);//遇到0跳转 
		getsym();//获取下一个token    
		condition_1(set);
		//code[cx1_and].a = cx;  code[cx_and[i_and]].a = cx;//L_falselist
		//gen(OPR, 0, OPR_AND);//生成AND指令14
		i_and++;
	} // while
	destroyset(set);
		
}

void condition(symset fsys)
{
	symset set;
	//int cx1_or; 
	set = uniteset(fsys, createset(SYM_OR, SYM_NULL));
	condition_and(set);
	while (sym == SYM_OR)//一个condition_and后应当遇到OR
	{
		cx_or[i_or] = cx;
		gen(JPD,0,0);
		getsym();//获取下一个token    
		condition_and(set);
		//code[cx1_or].a = cx;  code[cx_or[i_or]].a = cx;//L_truelist
		//gen(OPR, 0, OPR_OR);//生成OR指令15
		i_or++;
	} // while
	destroyset(set);	
}
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//语句分析处理
void statement(symset fsys) {
	char ida[MAXIDLEN + 1];
	int q,k=0;
	int i, cx1, cx2, cx3;
	int cx4;
	mask *mk;
	//自己添加用于实现for循环
	int cx_for_2;
	//, cx_for_3, cx_for_4;
	symset set1, set, set2;

	if (sym == SYM_IDENTIFIER)  
	{ // variable assignment
		char idt[MAXIDLEN + 1];
		strcpy(idt, id);
		int pos;
		pos = position(id);
		getsym();
		if (sym == SYM_BECOMES)
		{
			getsym();
		}
		else
		{
			if (sym == SYM_LBRACKET)
			{
				k = dimassign();
				if (sym == SYM_BECOMES)
				{
					getsym();
				}
				else
				{
					error(13);
				}
			}
			else
			{
				error(13); // ':=' expected.
			}
		}


		if (!(i = position(idt) + k))
		{
			error(11); // Undeclared identifier.
		}
		else if (table[i].kind != ID_VARIABLE)
		{
			error(12); // Illegal assignment.
			i = 0;
		}
		expression(uniteset(fsys, createset(SYM_SEMICOLON, SYM_NULL)));
		mk = (mask*)&table[i];
		if (i) {
			code[cx].var_addr = pos;
			gen(STO, level - mk->level, mk->address);
		}
	}
	else if (sym == SYM_CALL) {
		getsym();
		if (sym != SYM_IDENTIFIER) {
			error(14); // There must be an identifier to follow the 'call'.
		}
		else {
			if (!(i = position(id))) {
				error(11); // Undeclared identifier.
			}
			else if (table[i].kind == ID_PROCEDURE) {
				//mask* mk;
				mk = (mask*)&table[i];
				gen(CAL, level - mk->level, mk->address);
			}
			else {
				error(15); // A constant or variable can not be called. 
			}
			getsym();
		}
		jpx = cx;
	}
	else if (sym == SYM_IF)    { 
		getsym();    
		set1 = createset(SYM_THEN, SYM_DO, SYM_NULL);    
		set = uniteset(set1, fsys);
		condition(set);//条件分析
		destroyset(set1);
		destroyset(set);
		if (sym == SYM_THEN) {
			getsym();  
		}
		else {
			error(16); // 'then' expected.  
		}
		cx1 = cx;//记下当前代码分配指针位置  
		gen(JPC, 0, 0);//生成条件跳转指令
		//code[cx_or[i_or]].a = cx;//L_truelist 
		int m1 = i_or;
		for(i_or = 0;i_or < m1;i_or++)
			code[cx_or[i_or]].a = cx;//L_truelist
			 
		set1 = createset(SYM_ELSE, SYM_NULL);
		set = uniteset(set1, fsys);
		statement(fsys);
		destroyset(set1);
		destroyset(set);
		if (sym == SYM_SEMICOLON) {
			int temp_line_num = line_num;
			int temp_cc = cc;
			getsym();
			if (sym != SYM_ELSE) {
				//撤销上一个getsym()
				if (line_num > temp_line_num) {//换行了
					cc = 1;
				}
				else {
					cc = temp_cc;
				}
				ch = line[cc];
				sym = SYM_SEMICOLON;
			}
		}
		else {
			error(28); //';' semicolon expected.
		}
		if (sym != SYM_ELSE) //表达式后遇到不是else语句   
			code[cx1].a = cx;//把then填0的跳转位置改成当前位置，完成if语句的处理   
		else
		{//表达式后遇到else语句   
			getsym();
			cx2 = cx;//记下当前代码分配位置，这是else的开始位置   
			gen(JMP, 0, 0);  
			//code[cx_and[i_and]].a = cx;//L_falselist
			int m2 = i_and;
			for(i_and = 0;i_and < m2;i_and++)
				code[cx_and[i_and]].a = cx;//L_falselist
			statement(fsys);
			code[cx1].a = cx2 + 1;//把then语句填0的跳转位置改成当前位置，完成if语句的处理   
			code[cx2].a = cx;//把else填0的跳转位置改成当前位置，完成else语句的处理   
		}
	}
	
	
	else if (sym == SYM_BEGIN){// block
		getsym();
		set1 = createset(SYM_SEMICOLON, SYM_END, SYM_NULL);
		set = uniteset(set1, fsys);
		statement(set);//对begin与end之间的语句序列进行分析处理
		while (sym == SYM_SEMICOLON || inset(sym, statbegsys)) {
			if (sym == SYM_SEMICOLON) {
				getsym();
			}
			else {
				error(10);   //"';' expected."
			}
			statement(set);
		} // while
		destroyset(set1);
		destroyset(set);
		if (sym == SYM_END){
			getsym();
		}
		else {
			error(17); // ';' or 'end' expected.
		}
	}
	else if (sym == SYM_WHILE){// while statement
		cx1 = cx;
		getsym();
		set1 = createset(SYM_DO, SYM_NULL);
		set = uniteset(set1, fsys);
		condition(set);//对这个逻辑表达式进行分析计算
		destroyset(set1);
		destroyset(set);
		cx2 = cx;//记下当前代码分配位置，这是while的do中的语句的开始位置
		gen(JPC, 0, 0);// 生成条件跳转指令，跳转位置暂时填0
		if (sym == SYM_DO) {//逻辑表达式后应为do语句
			getsym();
		}
		else {
			error(18); // 'do' expected.
		}
		//code[cx_or[i_or]].a = cx;//L_truelist 
		int m1 = i_or;
		for(i_or = 0;i_or < m1;i_or++)
			code[cx_or[i_or]].a = cx;//L_truelist
					
		statement(fsys);//分析do后的语句块
		gen(JMP, 0, cx1);//循环跳转到cx1位置，即再次进行逻辑判断
		//code[cx_and[i_and]].a = cx;//L_falselist
		int m2 = i_and;
		for(i_and = 0;i_and < m2;i_and++)
			code[cx_and[i_and]].a = cx;//L_falselist
		code[cx2].a = cx;//把刚才填0的跳转位置改成当前位置，完成while语句的处理
		jx = cx;
	}
	//else if (sym == SYM_FOR) {
	//	//自己添加for循环实现
	//	getsym();
	//	if (sym == SYM_LPAREN) {
	//		getsym();
	//		if (sym == SYM_IDENTIFIER) {
	//			mask* mk;
	//			int pos;
	//			if (!(i = position(id))) {
	//				error(11); // Undeclared identifier.
	//			}
	//			else if (table[i].kind != ID_VARIABLE) {
	//				error(12); // Illegal assignment.
	//				i = 0;
	//			}
	//			//自己添加用于输出变量名
	//			pos = i;
	//			getsym();
	//			if (sym == SYM_BECOMES) {
	//				getsym();
	//			}
	//			else {
	//				error(13); // '=' expected.
	//			}
	//			expression(uniteset(fsys, createset(SYM_SEMICOLON, SYM_NULL)));
	//			mk = (mask*)&table[i];
	//			if (i) {
	//				//自己添加用于输出变量名
	//				code[cx].var_addr = pos;
	//				gen(STO, level - mk->level, mk->address);
	//			}
	//		}
	//		if (sym == SYM_SEMICOLON) {
	//			getsym();
	//			set1 = createset(SYM_SEMICOLON, SYM_NULL);
	//			set = uniteset(set1, fsys);
	//			//cx_for_1 = cx;
	//			condition(set);
	//			destroyset(set1);
	//			destroyset(set);
	//			cx_for_2 = cx;
	//			gen(JPC, 0, 0);
	//		}
	//		if (sym == SYM_SEMICOLON) {
	//			getsym();
	//			if (sym == SYM_IDENTIFIER) {
	//				mask* mk;
	//				int pos;
	//				if (!(i = position(id))) {
	//					error(11); // Undeclared identifier.
	//				}
	//				else if (table[i].kind != ID_VARIABLE) {
	//					error(12); // Illegal assignment.
	//					i = 0;
	//				}
	//				//自己添加用于输出变量名
	//				pos = i;
	//				getsym();
	//				if (sym == SYM_BECOMES) {
	//					getsym();
	//				}
	//				else {
	//					error(13); // '=' expected.
	//				}
	//				temp_cx_flag = 1;
	//				expression(uniteset(fsys, createset(SYM_SEMICOLON, SYM_NULL)));
	//				temp_cx_flag = 0;
	//				mk = (mask*)&table[i];
	//				if (i) {
	//					//自己添加用于输出变量名
	//					//以下两句应该改变输入到temp_code里边
	//					temp_gen(pos, STO, level - mk->level, mk->address);
	//					//gen(STO, level - mk->level, mk->address);
	//				}
	//			}
	//			//getsym();
	//			if (sym == SYM_RPAREN) {
	//				getsym();
	//			}
	//			else {
	//				error(22); // "Missing ')'."
	//			}
	//			statement(fsys);
	//			for (int j = 1; j < temp_cx; ++j, ++cx) {
	//				code[cx].a = temp_code[j].a;
	//				code[cx].f = temp_code[j].f;
	//				code[cx].l = temp_code[j].l;
	//				code[cx].var_addr = temp_code[j].var_addr;
	//			}
	//			temp_cx = 1;
	//			gen(JMP, 0, cx_for_1);
	//			code[cx_for_2].a = cx;
	//		}

	//	}
	//	else {
	//		error(27); // '(' expected.
	//	}
	//}
	/*else if (sym == SYM_EXITALL) {
		exit_cx[exit_x] = cx;
		exit_x++;
		gen(JMP, 0, 0);
		getsym();
	}*/
	else if (sym == SYM_READ) // 如果不是赋值语句，而是遇到了read语句   
	{
		getsym();//获得下一token，正常情况下应为左括号   
		if (sym != SYM_LPAREN)  //如果read语句后跟的不是左括号   
			error(40);
		else  //循环得到read语句括号中的参数表，依次产生相应的“从键盘读入”目标代码   
		{
			do
			{
				getsym();   //获得一个token，正常应是一个变量名                                      
				if (sym == SYM_IDENTIFIER)  //如果确为一个标识符   
				{
					if (!(i = position(id))) //查符号表，找到它所在位置给i，找不到时i会为0    
						error(11);
					else
						if (table[i].kind != ID_VARIABLE) //判别类型是否为标识符   
						{
							error(12);
							i = 0;
						}
						else
						{
							mk = (mask*)&table[i];
							gen(RED, level - mk->level, mk->address);  //生成red指令，把读入的值存入指定变量所在的空间                                            
						}
				}
				else
					error(4);
				getsym();
			} while (sym == SYM_COMMA);//不断生成代码直到read语句的参数表中的变量遍历完为止   
			if (sym != SYM_RPAREN)
				error(22);
			getsym(); //如果read语句正常结束，得到下一个token，一般为分号或end    
		}
	}
	else if (sym == SYM_WRITE)// 如果不是赋值语句，而是遇到了write语句
	{
		getsym();  //获取下一token，应为左括号   
		if (sym != SYM_LPAREN)//如果write语句后跟的不是左括号   
			error(40);
		else
		{
			do
			{ //依次获取括号中的每一个值，进行输出    
				getsym();
				set1 = createset(SYM_RPAREN, SYM_COMMA, SYM_NULL);
				set = uniteset(set1, fsys);
				expression(set);
				gen(WRT, 0, 0);//生成wrt指令，把读入的值存入指定变量所在的空间                                                                    
			} while (sym == SYM_COMMA);
			if (sym != SYM_RPAREN)//如果不是右括号   
				error(22);
			getsym();//正常情况下要获取下一个token，为后面准备好   
		}
	}
	else if (sym == SYM_FOR)
	{
		getsym();
		if (sym != SYM_IDENTIFIER)
			error(25);
		strcpy(ida, id); //记录当前读到的IDENTIFIER   
		set1 = createset(SYM_TO, SYM_DOWNTO, SYM_NULL);
		set = uniteset(set1, fsys);
		statement(set);
		destroyset(set1);
		destroyset(set);
		if ((sym != SYM_TO) && (sym != SYM_DOWNTO))
			error(26);
		q = sym; //记录下当前所读到的字符   
		cx1 = cx;//记下当前代码分配位置，这是to/downto语句的开始位置   
		gen(JMP, 0, 0);//产生一行跳转指令，跳转位置暂时未知填0   
		getsym();
		set1 = createset(SYM_DO, SYM_BY, SYM_NULL);
		set = uniteset(set1, fsys);
		expression(set);
		if ((sym != SYM_DO) && (sym != SYM_BY))
			error(27);
		strcpy(id, ida);//将ida记录下的IDENTIFIER拷贝给id   
		i = position(id);
		if (i == 0)
			error(11);
		else
			if (table[i].kind != ID_VARIABLE)//判别类型是否为变量   
			{
				error(12);
				i = 0;
			}
			else
			{
				mk = (mask*)&table[i];
				gen(LOD, level - mk->level, mk->address);//生成lod指令，把读入的值存入指定变量所在的空间    
				if (sym == SYM_BY)
				{ //对by后的表达式进行判断   
					getsym();
					//如果前一个字符为downto，则by后的sym应为SYM_MINUS，否则出错   
					//如果前一个字符为to，则by后的sym应为SYM_PLUS，否则出错   
					if ((sym == SYM_MINUS) || (sym == SYM_PLUS))
					{
						set1 = createset(SYM_DO, SYM_NULL);
						set = uniteset(set1, fsys);
						expression(set);
						if (sym != SYM_DO)
							error(28);
					}
					else
					{
						if (q == SYM_DOWNTO)
							error(35);
						else
							if (q == SYM_TO)
								error(36);
					}
				}
				else //当没有by时，执行默认处理指令   
				{
					gen(LIT, 0, 1);
					//downto默认执行1号操作   
					if (q == SYM_DOWNTO)
						gen(OPR, 0, 1);
				}
				gen(OPR, 0, 2);
				mk = (mask*)&table[i];
				gen(STO, level - mk->level, mk->address);//生成sto指令，把读入的值存入指定变量所在的空间   
				gen(LOD, level - mk->level, mk->address);//生成lod指令，把读入的值存入指定变量所在的空间   
				if (q == SYM_TO)//to默认执行11号操作   
					gen(OPR, 0, 11);
				else
					gen(OPR, 0, 1);
				cx2 = cx;//记下当前代码分配位置   
				gen(JPC, 0, 0);//生成条件跳转指令，跳转位置暂时填0，分析完语句后再填写   
				code[cx1].a = cx;//把刚才填0的跳转位置改成当前位置   
				getsym();
				statement(fsys);
				gen(JMP, 0, cx1 + 1);//循环跳转到cx1+1位置，即再次进行逻辑判断   
				code[cx2].a = cx;//把刚才填0的跳转位置改成当前位置
				jx = cx;
			}
	}
	else if (sym == SYM_REPEAT) //重复语句   
	{
		cx1 = cx;  //记下当前代码分配位置，这是repeat语句的开始位置   
		getsym();
		set1 = createset(SYM_SEMICOLON, SYM_UNTIL, SYM_NULL);
		set = uniteset(set1, fsys);
		statement(set);
		while (sym == SYM_SEMICOLON || inset(sym, statbegsys))
		{
			if (sym == SYM_SEMICOLON) //判别是否为‘；’    
				getsym();
			else
				error(10);
			statement(set);
		}
		destroyset(set1);
		destroyset(set);
		if (sym == SYM_UNTIL)
		{
			getsym();
			condition(fsys);
			gen(JPC, 0, cx1);//生成条件跳转指令，跳转位置填cx1   
		}
		else
			error(29);
	}
	else if (sym == SYM_EXIT)
	{                              //exit 
		gen(JMP, 0, jpx);
		getsym();
		//if (sym == SYM_EXPL1) explanation();//′
	}
	else if (sym == SYM_BREAK)
	{                                //break  
		gen(JMP, 0, jx);
		getsym();
		//if (sym == SYM_EXPL1) explanation();//′
	}
	set2 = uniteset(fsys, createset(SYM_BREAK, SYM_ELSE, SYM_BEGIN, SYM_END, SYM_EXIT, SYM_NULL));
	test(set2, phi, 19); // "Incorrect symbol."
} // statement
			
//////////////////////////////////////////////////////////////////////
void block(symset fsys) {//分程序分析处理，fsys:用于出错恢复的单词集合
	int cx0; // initial code index//记录本层开始时代码段分配位置
	mask* mk;
	int block_dx;//数据段内存分配指针，指向下一个被分配空间在数据段中的偏移位置
	int savedTx;//记录本层开始时符号表位置
	symset set1, set;

	dx = 3;
	block_dx = dx;
	mk = (mask*) &table[tx];
	mk->address = cx;
	gen(JMP, 0, 0);
	if (level > MAXLEVEL) {
		error(32); // There are too many levels.
	}
	do {
		if (sym == SYM_CONST) { // constant declarations
			getsym();
			do {
				constdeclaration();
				while (sym == SYM_COMMA) {
					getsym();
					constdeclaration();
				}
				if (sym == SYM_SEMICOLON) {
					getsym();
				}
				else {
					error(5); // Missing ',' or ';'.
				}
			} while (sym == SYM_IDENTIFIER);
		} // if

		if (sym == SYM_VAR) { // variable declarations
			getsym();
			do {
				vardeclaration();//变量声明处理
				while (sym == SYM_COMMA) {//，
					getsym();
					vardeclaration();
				}
				if (sym == SYM_SEMICOLON) {//；
					getsym();
				}
				else {
					error(5); // Missing ',' or ';'.
				}
			} while (sym == SYM_IDENTIFIER);
             //block = dx;
		} // if

		/*if (sym == SYM_ARR) {
			getsym();
			if (sym == SYM_LP) {

			}
		}*/

		while (sym == SYM_PROCEDURE) { // procedure declarations
			getsym();
			if (sym == SYM_IDENTIFIER) {
				enter(ID_PROCEDURE);
				getsym();
			}
			else {
				error(4); // There must be an identifier to follow 'const', 'var', or 'procedure'.
			}
			if (sym == SYM_SEMICOLON) {
				getsym();
			}
			else {
				error(5); // Missing ',' or ';'.
			}
			level++;
			savedTx = tx;
			set1 = createset(SYM_SEMICOLON, SYM_NULL);
			set = uniteset(set1, fsys);
			block(set);
			destroyset(set1);
			destroyset(set);
			tx = savedTx;
			level--;

			if (sym == SYM_SEMICOLON) {
				getsym();
				set1 = createset(SYM_IDENTIFIER, SYM_PROCEDURE, SYM_NULL);
				set = uniteset(statbegsys, set1);
				test(set, fsys, 6);
				destroyset(set1);
				destroyset(set);
			}
			else {
				error(5); // Missing ',' or ';'.
			}
		} // while
		set1 = createset(SYM_IDENTIFIER, SYM_NULL);
		set = uniteset(statbegsys, set1);
		test(set, declbegsys, 7); // "Statement expected."
		destroyset(set1);
		destroyset(set);
	} while (inset(sym, declbegsys));

	code[mk->address].a = cx;//把前面生成的跳转语句的跳转位置改成当前位置
	mk->address = cx;//地址为当前代码分配地址
	cx0 = cx;//记下当前代码分配位置
	gen(INT, 0, block_dx);//生成分配空间指令，分配dx个空间
	set1 = createset(SYM_SEMICOLON, SYM_END, SYM_NULL);
	set = uniteset(set1, fsys);
	statement(set);//处理当前遇到的语句或语句块
	destroyset(set1);
	destroyset(set);
	//code[exit_cx].a = cx;//自己添加exit语句应该跳转到的的地方
	if (exit_x != 0) {
		for (int i = 0; i < exit_x; ++i) {
			code[exit_cx[i]].a = cx;
		}
	}
	gen(OPR, 0, OPR_RET); // return//生成从子程序返回操作指令
	test(fsys, phi, 8); // test for error: Follow the statement is an incorrect symbol.
	listcode(cx0, cx);//列出本层的类PCODE代码
} // block

//////////////////////////////////////////////////////////////////////
int base(int stack[], int currentLevel, int levelDiff) {//通过静态链求出数据区的基地址//levelDiff要求的数据区所在层与当前层的层差
	int b = currentLevel;
	while (levelDiff--) {
		b = stack[b];
	}
	return b;//把找到的要求的数据区基址返回
} // base

//////////////////////////////////////////////////////////////////////
// interprets and executes codes.
void interpret() {//P-code解释执行程序
	int pc;        // program counter
	int stack[STACKSIZE];
	int top;       // top of stack
	int b;         // program, base, and top-stack register
	instruction i; // instruction register

	printf("Begin executing PL/0 program.\n");

	pc = 0;
	b = 1;
	top = 3;
	stack[1] = stack[2] = stack[3] = 0;
	do {
		i = code[pc++];//每次在code 表中读取一条指令
		switch (i.f) {
		case LIT:
			stack[++top] = i.a;//t=t+1; s[t]=i.a;
			break;
		case OPR:
			switch (i.a) {	// operator
			case OPR_RET://0
				top = b - 1;
				pc = stack[top + 3];
				b = stack[top + 2];
				break;
			case OPR_NEG:// 负号 1
				stack[top] = -stack[top];
				break;
			case OPR_ADD:// 加法 2
				top--;
				stack[top] += stack[top + 1];
				break;
			case OPR_MIN:// 减法3
				top--;
				stack[top] -= stack[top + 1];
				break;
			case OPR_MUL:// 乘法4
				top--;
				stack[top] *= stack[top + 1];
				break;
			case OPR_DIV://除法5
				top--;
				if (stack[top + 1] == 0) {
					fprintf(stderr, "Runtime Error: Divided by zero.\n");
					fprintf(stderr, "Program terminated.\n");
					continue;
				}
				stack[top] /= stack[top + 1];
				break;
			case OPR_ODD://6
				stack[top] %= 2;
				break;
			case OPR_EQU:
				top--;
				stack[top] = stack[top] == stack[top + 1];
				break;
			case OPR_NEQ:
				top--;
				stack[top] = stack[top] != stack[top + 1];
				break;
			case OPR_LES:
				top--;
				stack[top] = stack[top] < stack[top + 1];
				break;
			case OPR_GEQ:
				top--;
				stack[top] = stack[top] >= stack[top + 1];
				break;
			case OPR_GTR:
				top--;
				stack[top] = stack[top] > stack[top + 1];
				break;
			case OPR_LEQ://12
				top--;
				stack[top] = (stack[top] <= stack[top + 1]);
				break;
			case OPR_NOT://13
				if(stack[top] != 0) stack[top] = 0;
				else stack[top] = 1;
				break;
			case OPR_AND://14
				top--;
				if((stack[top] != 0) && (stack[top+1] != 0)) 
					stack[top] = 1;
				else stack[top] = 0;
				break;
			case OPR_OR://15
				top--;
				if((stack[top] == 0) && (stack[top+1] == 0)) 
					stack[top] = 0;
				else stack[top] = 1;
				break;
			} // switch
			break;
		case LOD://将变量的值放到栈顶
			stack[++top] = stack[base(stack, b, i.l) + i.a];
			break;
		case STO://将栈顶的值送到变量中
			stack[base(stack, b, i.l) + i.a] = stack[top];
			if (i.var_addr != 0) {
				printf(" %s = ", table[i.var_addr].name);
			}
			printf(" %d\n", stack[top]);
			top--;
			break;
		case CAL:
			stack[top + 1] = base(stack, b, i.l);
			// generate new block mark
			stack[top + 2] = b;
			stack[top + 3] = pc;
			b = top + 1;
			pc = i.a;
			break;
		case INT://分配内存空间
			top += i.a;
			break;
		case JMP://无条件跳转
			pc = i.a;
			break;
		case JPC://条件挑战
			if (stack[top] == 0)
				pc = i.a;
			top--;
			break;
		case RED://如果是red指令   
				 //把栈顶的值存入位置在数据区层差l偏移地址a的变量内存   
			printf("输入:\n");
			scanf("%d", &stack[base(stack, b, i.l) + i.a]);
			break;
		case WRT://如果是wrt指令   
			printf("输出:%d\n", stack[top]);
			//fprintf(outfile, "输出:%d\n", stack[top]);
			top++;//栈顶上移，开辟空间   
			break;
		case JPD://JPD是遇到1就跳转，但是会保留栈顶的计算结果
			if (stack[top] != 0)
				pc = i.a;
			top--;
			break;
		case JPE://JPE是遇到0就跳转，但是会保留栈顶的计算结果 
			if (stack[top] == 0)
				pc = i.a;
			top--;
			break;
		} // switch
	} while (pc);

	printf("End executing PL/0 program.\n");
} // interpret


//////////////////////////////////////////////////////////////////////
void main() {
	FILE *table_fp;
	FILE *hbin;
	char s[80];
	int i; 
	symset set, set1, set2;

	//自己添加初始化code[i].var_addr用于输出变量名
	for (int i = 0; i < CXMAX; ++i) {
		code[i].var_addr = 0;
	}

	printf("Please input source file name: "); // get file name to be compiled
	scanf("%s", s);
	if ((infile = fopen(s, "r")) == NULL) {
		printf("File %s can't be opened.\n", s);
		exit(1);
	}
	phi = createset(SYM_NULL);
	relset = createset(SYM_EQU, SYM_NEQ, SYM_LES, SYM_LEQ, SYM_GTR, SYM_GEQ, SYM_NULL);
	
	// create begin symbol sets
	declbegsys = createset(SYM_CONST, SYM_VAR, SYM_PROCEDURE, SYM_NULL);
	//statbegsys = createset(SYM_BEGIN, SYM_CALL, SYM_IF, SYM_WHILE, SYM_NULL);
	//自己修改
	statbegsys = createset(SYM_IDENTIFIER, SYM_BEGIN, SYM_CALL, SYM_IF, SYM_WHILE, SYM_NULL);
	facbegsys = createset(SYM_IDENTIFIER, SYM_NUMBER, SYM_LPAREN, SYM_NULL);

	err = cc = cx = ll = 0; // initialize global variables
	ch = ' ';
	kk = MAXIDLEN;

	getsym();

	set1 = createset(SYM_PERIOD, SYM_NULL);
	set2 = uniteset(declbegsys, statbegsys);
	set = uniteset(set1, set2);
	block(set);
	destroyset(set1);
	destroyset(set2);
	destroyset(set);
	destroyset(phi);
	destroyset(relset);
	destroyset(declbegsys);
	destroyset(statbegsys);
	destroyset(facbegsys);

	if (sym != SYM_PERIOD) {
		error(9); // '.' expected.
	}	

	if (err == 0) {
		hbin = fopen(BIN_PATH, "w");
		fwrite(&cx, sizeof(int), 1, hbin);
		for (i = 0; i < cx; i++) {
			fwrite(&code[i], sizeof(instruction), 1, hbin);
		}	
		for (i = 1; i <= tx; ++i) {
			fwrite(&table[i], sizeof(comtab), 1, hbin);
		}
		fclose(hbin);
	}
	else {
		printf("There are %d error(s) in PL/0 program.\n", err);
	}
	if (err == 0) {
		interpret();
	}
	
		
	listcode(0, cx);
	getchar();

} // main

//////////////////////////////////////////////////////////////////////
// eof pl0.c
