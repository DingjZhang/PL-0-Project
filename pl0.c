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
		int line_cl = ll; //��line�� "//" ��λ��
		int left_cl = ll;	// "/*"��λ��
		int right_cl = ll; //"*/��λ��"
		int comment_exist = 1; //ֵΪ1����ʾ��ǰ���д���ע��

		while (comment_exist == 1) {
			//��line �� '//'�Ĵ�ƥ��
			for (int i = 1; i < ll - 1; ++i) {
				if (line[i] == '/' && line[i + 1] == '/') {
					line_cl = i;
					break;
				}
			}
			//��line �� '/*'�Ĵ�ƥ��
			for (int i = 1; i < ll - 1; ++i) {
				if (line[i] == '/' && line[i + 1] == '*') {
					left_cl = i;
					break;
				}
			}
			//��line �� '*/'�Ĵ�ƥ��
			for (int i = 1; i < ll - 1; ++i) {
				if (line[i] == '*' && line[i + 1] == '/') {
					right_cl = i;
					break;
				}
			}
			if (left_cl == ll && right_cl == ll && line_cl == ll) {
				comment_exist = 0; //��ǰ���в�����ע��
				if (comment_state == 1) {
					for (int i = 1; i < ll; ++i) {
						line[i] = ' ';
					}
				}
			}
			else { //��ǰ���д���ע��
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
	else if (ch == '&')//�������&
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
	else if (ch == '|')//�������| 
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


//�Լ����
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
//���Ե�ǰ���ʷ����Ƿ�Ϸ���������
//sym ���� s1 ��������˴���Ȼ��ͣ�� getsym() ֱ������һ�� s1 �� s2 �� sym.
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
//�ǼǷ��ű�
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
//���ұ�ʶ���ڷ��ű��е�λ��
int position(char* id) {
	int i;
	strcpy(table[0].name, id);
	i = tx + 1;
	while (strcmp(table[--i].name, id) != 0);
	return i;
} // position

//////////////////////////////////////////////////////////////////////
//�������崦��
void constdeclaration() {
	if (sym == SYM_IDENTIFIER) {
		getsym();
		if (sym == SYM_EQU || sym == SYM_BECOMES) {
			//if (sym == SYM_BECOMES)
			//	error(1); // Found '=' when expecting '=='.
			//�Լ��������о��ϱߵĳ������
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
//�������崦��
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
		//�Լ����������������ʽ
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
//���ӷ�������
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
//���������
void term(symset fsys) {
	int mulop;
	symset set;
	set = uniteset(fsys, createset(SYM_TIMES, SYM_SLASH, SYM_NULL));
	factor(set); //���Ӵ���
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
//���ʽ��������
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
//������������
void condition(symset fsys);//���� 
void condition_1(symset fsys)//������������
{
	int relop;//������ʱ��¼token������ 
	symset set;
	
	if(sym == SYM_NOT)
	{
		getsym(); //��ȡ��һ��token    
		if (sym == SYM_ODD)//�����odd�����(һԪ)   
		{
			getsym(); //��ȡ��һ��token
			expression(fsys); //��odd�ı��ʽ���д������   
			gen(OPR, 0, 6);//����6�Ų���ָ���ż�ж�����   
		}
		else if(sym == SYM_LPAREN)
		{
			getsym(); //��ȡһ��token    
			set = uniteset(createset(SYM_RPAREN, SYM_NULL), fsys);
			condition(set);//�ݹ����expression�ӳ������һ���ӱ��ʽ   
			destroyset(set);
			if (sym == SYM_RPAREN)//�ӱ��ʽ�������Ӧ����������   
			{
				getsym(); //�����ȷ���������ţ���ȡ��һ��token   
			}
			else
			{
				error(22); // Missing ')'.
			}
		}
		else/* �������odd �����(�Ǿ�һ���Ƕ�Ԫ�߼������) */
		{
			set = uniteset(relset, fsys);
			expression(set);//�Ա��ʽ�󲿽��д������   
			destroyset(set);
			if (! inset(sym, relset))// ���token�����߼�������е�һ��   
			{
				
				//error(20);
			}
			else
			{
				relop = sym; //��¼�µ�ǰ���߼������   
				getsym();//��ȡ��һ��token    
				expression(fsys); //�Ա��ʽ�Ҳ����д������    
				switch (relop)//����ղŵ�������������һ��   
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
		gen(OPR, 0,OPR_NOT);//����13�Ų���ָ�not�ж�����
	}
	//���������NOT��ͷ
	else
	{
		if (sym == SYM_ODD)//�����odd�����(һԪ)   
		{
			getsym(); //��ȡ��һ��token
			expression(fsys); //��odd�ı��ʽ���д������   
			gen(OPR, 0, 6);//����6�Ų���ָ���ż�ж�����   
		}
		else if(sym == SYM_LPAREN)
		{
			getsym(); //��ȡһ��token    
			set = uniteset(createset(SYM_RPAREN, SYM_NULL), fsys);
			condition(set);//�ݹ����expression�ӳ������һ���ӱ��ʽ   
			destroyset(set);
			if (sym == SYM_RPAREN)//�ӱ��ʽ�������Ӧ����������   
			{
				getsym(); //�����ȷ���������ţ���ȡ��һ��token   
			}
			else
			{
				error(22); // Missing ')'.
			}
		}
		else/* �������odd �����(�Ǿ�һ���Ƕ�Ԫ�߼������) */
		{
			set = uniteset(relset, fsys);
			expression(set);//�Ա��ʽ�󲿽��д������   
			destroyset(set);
			if (! inset(sym, relset))// ���token�����߼�������е�һ��   
			{
				//error(20);
			}
			else
			{
				relop = sym; //��¼�µ�ǰ���߼������   
				getsym();//��ȡ��һ��token    
				expression(fsys); //�Ա��ʽ�Ҳ����д������    
				switch (relop)//����ղŵ�������������һ��   
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
	condition_1(set);//ÿһ��condition_and��Ӧ����condition_1��ʼ
	while (sym == SYM_AND)//һ��condition_1��Ӧ������AND   
	{ 
		cx_and[i_and] = cx;
		gen(JPE,0,0);//����0��ת 
		getsym();//��ȡ��һ��token    
		condition_1(set);
		//code[cx1_and].a = cx;  code[cx_and[i_and]].a = cx;//L_falselist
		//gen(OPR, 0, OPR_AND);//����ANDָ��14
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
	while (sym == SYM_OR)//һ��condition_and��Ӧ������OR
	{
		cx_or[i_or] = cx;
		gen(JPD,0,0);
		getsym();//��ȡ��һ��token    
		condition_and(set);
		//code[cx1_or].a = cx;  code[cx_or[i_or]].a = cx;//L_truelist
		//gen(OPR, 0, OPR_OR);//����ORָ��15
		i_or++;
	} // while
	destroyset(set);	
}
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//����������
void statement(symset fsys) {
	char ida[MAXIDLEN + 1];
	int q,k=0;
	int i, cx1, cx2, cx3;
	int cx4;
	mask *mk;
	//�Լ��������ʵ��forѭ��
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
		condition(set);//��������
		destroyset(set1);
		destroyset(set);
		if (sym == SYM_THEN) {
			getsym();  
		}
		else {
			error(16); // 'then' expected.  
		}
		cx1 = cx;//���µ�ǰ�������ָ��λ��  
		gen(JPC, 0, 0);//����������תָ��
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
				//������һ��getsym()
				if (line_num > temp_line_num) {//������
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
		if (sym != SYM_ELSE) //���ʽ����������else���   
			code[cx1].a = cx;//��then��0����תλ�øĳɵ�ǰλ�ã����if���Ĵ���   
		else
		{//���ʽ������else���   
			getsym();
			cx2 = cx;//���µ�ǰ�������λ�ã�����else�Ŀ�ʼλ��   
			gen(JMP, 0, 0);  
			//code[cx_and[i_and]].a = cx;//L_falselist
			int m2 = i_and;
			for(i_and = 0;i_and < m2;i_and++)
				code[cx_and[i_and]].a = cx;//L_falselist
			statement(fsys);
			code[cx1].a = cx2 + 1;//��then�����0����תλ�øĳɵ�ǰλ�ã����if���Ĵ���   
			code[cx2].a = cx;//��else��0����תλ�øĳɵ�ǰλ�ã����else���Ĵ���   
		}
	}
	
	
	else if (sym == SYM_BEGIN){// block
		getsym();
		set1 = createset(SYM_SEMICOLON, SYM_END, SYM_NULL);
		set = uniteset(set1, fsys);
		statement(set);//��begin��end֮���������н��з�������
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
		condition(set);//������߼����ʽ���з�������
		destroyset(set1);
		destroyset(set);
		cx2 = cx;//���µ�ǰ�������λ�ã�����while��do�е����Ŀ�ʼλ��
		gen(JPC, 0, 0);// ����������תָ���תλ����ʱ��0
		if (sym == SYM_DO) {//�߼����ʽ��ӦΪdo���
			getsym();
		}
		else {
			error(18); // 'do' expected.
		}
		//code[cx_or[i_or]].a = cx;//L_truelist 
		int m1 = i_or;
		for(i_or = 0;i_or < m1;i_or++)
			code[cx_or[i_or]].a = cx;//L_truelist
					
		statement(fsys);//����do�������
		gen(JMP, 0, cx1);//ѭ����ת��cx1λ�ã����ٴν����߼��ж�
		//code[cx_and[i_and]].a = cx;//L_falselist
		int m2 = i_and;
		for(i_and = 0;i_and < m2;i_and++)
			code[cx_and[i_and]].a = cx;//L_falselist
		code[cx2].a = cx;//�Ѹղ���0����תλ�øĳɵ�ǰλ�ã����while���Ĵ���
		jx = cx;
	}
	//else if (sym == SYM_FOR) {
	//	//�Լ����forѭ��ʵ��
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
	//			//�Լ�����������������
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
	//				//�Լ�����������������
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
	//				//�Լ�����������������
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
	//					//�Լ�����������������
	//					//��������Ӧ�øı����뵽temp_code���
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
	else if (sym == SYM_READ) // ������Ǹ�ֵ��䣬����������read���   
	{
		getsym();//�����һtoken�����������ӦΪ������   
		if (sym != SYM_LPAREN)  //���read������Ĳ���������   
			error(40);
		else  //ѭ���õ�read��������еĲ��������β�����Ӧ�ġ��Ӽ��̶��롱Ŀ�����   
		{
			do
			{
				getsym();   //���һ��token������Ӧ��һ��������                                      
				if (sym == SYM_IDENTIFIER)  //���ȷΪһ����ʶ��   
				{
					if (!(i = position(id))) //����ű��ҵ�������λ�ø�i���Ҳ���ʱi��Ϊ0    
						error(11);
					else
						if (table[i].kind != ID_VARIABLE) //�б������Ƿ�Ϊ��ʶ��   
						{
							error(12);
							i = 0;
						}
						else
						{
							mk = (mask*)&table[i];
							gen(RED, level - mk->level, mk->address);  //����redָ��Ѷ����ֵ����ָ���������ڵĿռ�                                            
						}
				}
				else
					error(4);
				getsym();
			} while (sym == SYM_COMMA);//�������ɴ���ֱ��read���Ĳ������еı���������Ϊֹ   
			if (sym != SYM_RPAREN)
				error(22);
			getsym(); //���read��������������õ���һ��token��һ��Ϊ�ֺŻ�end    
		}
	}
	else if (sym == SYM_WRITE)// ������Ǹ�ֵ��䣬����������write���
	{
		getsym();  //��ȡ��һtoken��ӦΪ������   
		if (sym != SYM_LPAREN)//���write������Ĳ���������   
			error(40);
		else
		{
			do
			{ //���λ�ȡ�����е�ÿһ��ֵ���������    
				getsym();
				set1 = createset(SYM_RPAREN, SYM_COMMA, SYM_NULL);
				set = uniteset(set1, fsys);
				expression(set);
				gen(WRT, 0, 0);//����wrtָ��Ѷ����ֵ����ָ���������ڵĿռ�                                                                    
			} while (sym == SYM_COMMA);
			if (sym != SYM_RPAREN)//�������������   
				error(22);
			getsym();//���������Ҫ��ȡ��һ��token��Ϊ����׼����   
		}
	}
	else if (sym == SYM_FOR)
	{
		getsym();
		if (sym != SYM_IDENTIFIER)
			error(25);
		strcpy(ida, id); //��¼��ǰ������IDENTIFIER   
		set1 = createset(SYM_TO, SYM_DOWNTO, SYM_NULL);
		set = uniteset(set1, fsys);
		statement(set);
		destroyset(set1);
		destroyset(set);
		if ((sym != SYM_TO) && (sym != SYM_DOWNTO))
			error(26);
		q = sym; //��¼�µ�ǰ���������ַ�   
		cx1 = cx;//���µ�ǰ�������λ�ã�����to/downto���Ŀ�ʼλ��   
		gen(JMP, 0, 0);//����һ����תָ���תλ����ʱδ֪��0   
		getsym();
		set1 = createset(SYM_DO, SYM_BY, SYM_NULL);
		set = uniteset(set1, fsys);
		expression(set);
		if ((sym != SYM_DO) && (sym != SYM_BY))
			error(27);
		strcpy(id, ida);//��ida��¼�µ�IDENTIFIER������id   
		i = position(id);
		if (i == 0)
			error(11);
		else
			if (table[i].kind != ID_VARIABLE)//�б������Ƿ�Ϊ����   
			{
				error(12);
				i = 0;
			}
			else
			{
				mk = (mask*)&table[i];
				gen(LOD, level - mk->level, mk->address);//����lodָ��Ѷ����ֵ����ָ���������ڵĿռ�    
				if (sym == SYM_BY)
				{ //��by��ı��ʽ�����ж�   
					getsym();
					//���ǰһ���ַ�Ϊdownto����by���symӦΪSYM_MINUS���������   
					//���ǰһ���ַ�Ϊto����by���symӦΪSYM_PLUS���������   
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
				else //��û��byʱ��ִ��Ĭ�ϴ���ָ��   
				{
					gen(LIT, 0, 1);
					//downtoĬ��ִ��1�Ų���   
					if (q == SYM_DOWNTO)
						gen(OPR, 0, 1);
				}
				gen(OPR, 0, 2);
				mk = (mask*)&table[i];
				gen(STO, level - mk->level, mk->address);//����stoָ��Ѷ����ֵ����ָ���������ڵĿռ�   
				gen(LOD, level - mk->level, mk->address);//����lodָ��Ѷ����ֵ����ָ���������ڵĿռ�   
				if (q == SYM_TO)//toĬ��ִ��11�Ų���   
					gen(OPR, 0, 11);
				else
					gen(OPR, 0, 1);
				cx2 = cx;//���µ�ǰ�������λ��   
				gen(JPC, 0, 0);//����������תָ���תλ����ʱ��0����������������д   
				code[cx1].a = cx;//�Ѹղ���0����תλ�øĳɵ�ǰλ��   
				getsym();
				statement(fsys);
				gen(JMP, 0, cx1 + 1);//ѭ����ת��cx1+1λ�ã����ٴν����߼��ж�   
				code[cx2].a = cx;//�Ѹղ���0����תλ�øĳɵ�ǰλ��
				jx = cx;
			}
	}
	else if (sym == SYM_REPEAT) //�ظ����   
	{
		cx1 = cx;  //���µ�ǰ�������λ�ã�����repeat���Ŀ�ʼλ��   
		getsym();
		set1 = createset(SYM_SEMICOLON, SYM_UNTIL, SYM_NULL);
		set = uniteset(set1, fsys);
		statement(set);
		while (sym == SYM_SEMICOLON || inset(sym, statbegsys))
		{
			if (sym == SYM_SEMICOLON) //�б��Ƿ�Ϊ������    
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
			gen(JPC, 0, cx1);//����������תָ���תλ����cx1   
		}
		else
			error(29);
	}
	else if (sym == SYM_EXIT)
	{                              //exit 
		gen(JMP, 0, jpx);
		getsym();
		//if (sym == SYM_EXPL1) explanation();//��
	}
	else if (sym == SYM_BREAK)
	{                                //break  
		gen(JMP, 0, jx);
		getsym();
		//if (sym == SYM_EXPL1) explanation();//��
	}
	set2 = uniteset(fsys, createset(SYM_BREAK, SYM_ELSE, SYM_BEGIN, SYM_END, SYM_EXIT, SYM_NULL));
	test(set2, phi, 19); // "Incorrect symbol."
} // statement
			
//////////////////////////////////////////////////////////////////////
void block(symset fsys) {//�ֳ����������fsys:���ڳ���ָ��ĵ��ʼ���
	int cx0; // initial code index//��¼���㿪ʼʱ����η���λ��
	mask* mk;
	int block_dx;//���ݶ��ڴ����ָ�룬ָ����һ��������ռ������ݶ��е�ƫ��λ��
	int savedTx;//��¼���㿪ʼʱ���ű�λ��
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
				vardeclaration();//������������
				while (sym == SYM_COMMA) {//��
					getsym();
					vardeclaration();
				}
				if (sym == SYM_SEMICOLON) {//��
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

	code[mk->address].a = cx;//��ǰ�����ɵ���ת������תλ�øĳɵ�ǰλ��
	mk->address = cx;//��ַΪ��ǰ��������ַ
	cx0 = cx;//���µ�ǰ�������λ��
	gen(INT, 0, block_dx);//���ɷ���ռ�ָ�����dx���ռ�
	set1 = createset(SYM_SEMICOLON, SYM_END, SYM_NULL);
	set = uniteset(set1, fsys);
	statement(set);//����ǰ��������������
	destroyset(set1);
	destroyset(set);
	//code[exit_cx].a = cx;//�Լ����exit���Ӧ����ת���ĵĵط�
	if (exit_x != 0) {
		for (int i = 0; i < exit_x; ++i) {
			code[exit_cx[i]].a = cx;
		}
	}
	gen(OPR, 0, OPR_RET); // return//���ɴ��ӳ��򷵻ز���ָ��
	test(fsys, phi, 8); // test for error: Follow the statement is an incorrect symbol.
	listcode(cx0, cx);//�г��������PCODE����
} // block

//////////////////////////////////////////////////////////////////////
int base(int stack[], int currentLevel, int levelDiff) {//ͨ����̬������������Ļ���ַ//levelDiffҪ������������ڲ��뵱ǰ��Ĳ��
	int b = currentLevel;
	while (levelDiff--) {
		b = stack[b];
	}
	return b;//���ҵ���Ҫ�����������ַ����
} // base

//////////////////////////////////////////////////////////////////////
// interprets and executes codes.
void interpret() {//P-code����ִ�г���
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
		i = code[pc++];//ÿ����code ���ж�ȡһ��ָ��
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
			case OPR_NEG:// ���� 1
				stack[top] = -stack[top];
				break;
			case OPR_ADD:// �ӷ� 2
				top--;
				stack[top] += stack[top + 1];
				break;
			case OPR_MIN:// ����3
				top--;
				stack[top] -= stack[top + 1];
				break;
			case OPR_MUL:// �˷�4
				top--;
				stack[top] *= stack[top + 1];
				break;
			case OPR_DIV://����5
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
		case LOD://��������ֵ�ŵ�ջ��
			stack[++top] = stack[base(stack, b, i.l) + i.a];
			break;
		case STO://��ջ����ֵ�͵�������
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
		case INT://�����ڴ�ռ�
			top += i.a;
			break;
		case JMP://��������ת
			pc = i.a;
			break;
		case JPC://������ս
			if (stack[top] == 0)
				pc = i.a;
			top--;
			break;
		case RED://�����redָ��   
				 //��ջ����ֵ����λ�������������lƫ�Ƶ�ַa�ı����ڴ�   
			printf("����:\n");
			scanf("%d", &stack[base(stack, b, i.l) + i.a]);
			break;
		case WRT://�����wrtָ��   
			printf("���:%d\n", stack[top]);
			//fprintf(outfile, "���:%d\n", stack[top]);
			top++;//ջ�����ƣ����ٿռ�   
			break;
		case JPD://JPD������1����ת�����ǻᱣ��ջ���ļ�����
			if (stack[top] != 0)
				pc = i.a;
			top--;
			break;
		case JPE://JPE������0����ת�����ǻᱣ��ջ���ļ����� 
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

	//�Լ���ӳ�ʼ��code[i].var_addr�������������
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
	//�Լ��޸�
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
