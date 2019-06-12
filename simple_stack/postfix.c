#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

char error[100];

typedef int Num;
struct stack_el {
	Num num;
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
	return strchr("+-*/^()", (int)c) != 0;
}

int is_empty(char *s) {
	while(isblank(*s++));
	return *s == '\0';
}

Num broj(char** p_expr) {
	Num r = 0;
	int i=0, base = 10;
	float zeros = 0;
	char c;
	char* expr = *p_expr;
	// na osnovu prefiksa 0 ili 0x odrediti bazu
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

Num postfix(char *s) {
	for(;*s; s++) {
		char c = *s;
		if(is_operator(c)) {
			if(stack_level >= 2) {
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
				return 0;
			}
		} else if(is_number(c)) {
			push(broj(&s));
		} else if(!isspace(c)) {
			sprintf(error, "nepoznati operator %c\n", c);
			return 0;
		}
	}
	return pop();
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
		// preskakanje linije
		if(is_empty(linija)) {
			continue;
		}
		
		error[0]=0;
		linija[strlen(linija)-1] = 0; // \n -> \0

		strcpy(izrazi[izr].ispis, linija);
		izrazi[izr].vrednost = postfix(linija);
		
		if(error[0] == 0) {
			char tmp[100];
			sprintf(tmp, "%d", izrazi[izr].vrednost);
			sprintf(linija, " = %s\n", tmp);
		} else {
			// ; <greska>
			sprintf(linija, " ; greska na liniji %d : %s", line, error);
		}
		strcat(izrazi[izr].ispis, linija);
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
