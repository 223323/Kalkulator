#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

char error[100];

typedef int Num;
struct stack_el {
	Num num;
	char op;
	struct stack_el* top;
};

struct stack_el* stack = 0;
int stack_level = 0;


void push(Num num) {
	struct stack_el *el = malloc(sizeof(struct stack_el));
	el->num = num;
	el->top = stack;
	stack = el;
	stack_level++;
}

Num top() {
	return stack->num;
}
Num pop() {
	if(stack) {
		Num num = stack->num;
		stack = stack->top;
		stack_level--;
		return num;
	}
	return 0;
}

#define MAX 100
struct Izraz {
	char ispis[MAX];
	Num vrednost;
};
struct Izraz izrazi[MAX];

int is_number(char c) {
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || c == '.';
}
int is_operator(char c) {
	return strchr("+-*/^()", (int)c & 0x7f) != 0;
}

int is_empty(char *s) {
	while(isblank(*s++));
	return *s == '\0';
}
int is_unary(char c) {
	return (c & 0x80) == 0x80;
}

Num broj(char** p_expr) {
	Num r = 0;
	int i=0, base = 10;
	float zeros = 0;
	char c;
	char* expr = *p_expr;

	if (expr[i] == '0') {
		base = 8;
		i++;
		if(expr[i] == 'x') {
			base = 16;
			i++;
		}
	}
	
	for (; is_number(c = expr[i]); i++) {
		int n = c - '0';
		if(c >= 'a' && c <= 'f') {
			n = c - 'a' + 10;
		}
		if(c == '.') {
			zeros = 1;
			continue;
		}
		if(n >= base) {
			sprintf(error, "fali operand za operator %c\n", c);
			return 0;
		}
		r = base*r + n;
		zeros *= base;
	}
	*p_expr += i-1;
	return r / fmax(1, zeros);
}

typedef struct operator_t {
	char* op;
	int priority;
	int type;
} Operator;

int has_unary[] = {1,1,0,0,0,0,0};
char ops[] = "-+*/^()";
int priorities[] = { 2,2,3,3,4,1,1 };

int op_operands(char op) {
	return op & 0x80 ? 1 : 2;
}

int op_priority(char op) {
	return (op & 0x80) ? 5 : priorities[strchr(ops,op)-ops];
}

void infix_to_postfix(char* infix, char* sol) {
	int i;
	char c;
	int unary = 1;
	int zag = 0;
	for(i=0; c=*infix; infix++) {
		if(is_number(c)) { // number
			sol[i++] = c;
			unary = 0;
		} else if(is_operator(c)) {
			if(c == '(') {
				push(c);
				unary = 1;
				zag++;
			} else if(c == ')') {
				zag--;
				while (stack && top() != '(') {
					sol[i++] = pop();
				}
				pop();
				unary = 0;
			} else {
				if(unary) {
					c |= 0x80;
				}
				if(op_operands(c) != 1) {
					while(stack && op_priority(top()) >= op_priority(c)) {
						sol[i++] = pop();
					}
				}
				sol[i++] = ' ';
				push(c);
				unary = 1;
			}
		} else if(!isblank(c)) {
			sprintf(error, "nepoznati operator %c\n", c);
			while(stack)pop();
			return;
		}
	}
	if(zag != 0) {
		sprintf(error, "fali zagrada\n", c);
		while(stack)pop();
		return;
	}
	while(stack) {
		sol[i++] = pop();
	}
	sol[i] = 0;
}

Num postfix(char *s) {
	while(stack)pop();
	for(;*s; s++) {
		char c = *s;
		if(is_operator(c)) {
			if(is_unary(c) && stack_level >= 1) {
				c &= ~0x80;
				Num n1 = pop();
				if(c == '+')
					push(n1);
				else if(c == '-')
					push(-n1);
			}
			else 
			if(!is_unary(c) && stack_level >= 2) {
				Num n2 = pop();
				Num n1 = pop();
				if(c == '+')
					push(n1+n2);
				else if(c == '-')
					push(n1-n2);
				else if(c == '*')
					push(n1*n2);
				else if(c == '/')
					push(n1/n2);
				else if(c == '^')
					push(pow(n1,n2));
			} else {
				sprintf(error, "fali operand za operator %c\n", c);
				while(stack)pop();
				return 0;
			}
		} else if(is_number(c)) {
			push(broj(&s));
		}
	}
	Num r = pop();
	if(stack) {
		while(stack)pop();
	}
	return r;
}



void main( int argc, char *argv[]) {
	FILE* f = fopen(argv[1], "r");
	char linija[MAX];
	if(!f) {
		printf("fajl %s ne postoji\n", argv[1]);
		return;
	}
	
	int line=0,izr=0;
	while(fgets(linija, MAX, f)) {
		line++;
		if(is_empty(linija)) {
			continue;
		}
		error[0]=0;
		linija[strlen(linija)-1] = 0; // \n -> \0

		char tmp[100];
		infix_to_postfix(linija, tmp);
		
		if(error[0] == 0) {
			izrazi[izr].vrednost = postfix(tmp);
		}
		
		if(error[0] == 0) {
			sprintf(izrazi[izr].ispis, "%s = %d\n", linija, izrazi[izr].vrednost);
		} else {
			sprintf(izrazi[izr].ispis, "%s ; greska na liniji %d : %s", linija, line, error);
		}
		izr++;
	}
	fclose(f);
	
	// sortiranje resenja
	int i,j;
	for(i=0; i < izr; i++) {
		for(j=i; j < izr; j++) {
			if(izrazi[i].vrednost > izrazi[j].vrednost) {
				struct Izraz tmp = izrazi[i];
				izrazi[i] = izrazi[j];
				izrazi[j] = tmp;
			}
		}
	}
	
	printf("Fajl %s sadrzi %d izraza\n", argv[1], izr);
	printf("----------------\n");
	for(i=0; i < izr; i++) {
		if(1) { // dodaj ovde uslov ukoliko se trazi neparno resenje i sl.
			printf("%s", (izrazi[i].ispis));
		}
	}
}
