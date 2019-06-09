#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define USE_FLOAT

// PAZNJA: ne moze boolean algebra sa floating-om da radi
// #define USE_BOOLEAN

#ifdef USE_FLOAT
typedef float NumType;
#define VAL_FORMAT "%.2f"
#else

typedef int NumType;
#define VAL_FORMAT "%d"
#endif


char error[100];

// stack i liste
typedef struct list_elem {
	NumType value;
	char op;
	struct list_elem *next;
} list_elem;


// STACK
typedef struct stack_t {
	list_elem *top;
} Stack;

// inicijalizuje stek
void stack_init(Stack* stack) {
	stack->top = 0;
}

// da li je stek prazan
int stack_empty(Stack* stack) {
	return stack->top == 0;
}

// dodaje na stek
void stack_push(Stack* stack, NumType value, char op) {
	list_elem* new = malloc(sizeof(list_elem));
	new->value = value;
	new->op = op;
	new->next = stack->top;
	stack->top = new;
}

// vraca gornji element na steku
list_elem* stack_peek(Stack* stack) {
	return stack->top;
}

// skida element sa steka
void stack_pop(Stack* stack) {
	if(stack_empty(stack)) return;
	list_elem* el = stack->top;
	stack->top = stack->top->next;
	free(el);
}

// LIST
typedef struct list_t {
	list_elem *head;
	list_elem *tail;
} List;

// inicijalizacija liste
void list_init(List* list) {
	list->head = list->tail = 0;
}

// dodaje element na listu
void list_append(List* list, NumType value, char op) {
	list_elem* new = malloc(sizeof(list_elem));
	new->value = value;
	new->op = op;
	new->next = 0;
	
	if(list->tail == 0) {
		list->tail = list->head = new;
	} else {
		list->tail->next = new;
		list->tail = new;
	}
}

// obrnuti listu za pravljenje prefiksne verzije iz infiksne
void list_reverse(List* list) {
	list->tail = list->head;
	list_elem *el = list->head;
	list_elem *prev = 0, *next = 0;
	while(el) {
		next = el->next;
		el->next = prev;
		prev = el;
		el = next;
	}
	list->head = prev;
}

#define F_UNARY 0x80

// ispisuje listu
void list_dump(List list) {
	list_elem* el = list.head;
	while(el) {
		if(el->op == 0) {
			printf(VAL_FORMAT " ", el->value);
		} else {
			if(el->op & F_UNARY) {
				printf("u%c ", el->op & ~F_UNARY);
			} else {
				printf("%c ", el->op);
			}
		}
		el = el->next;
	}
	printf("\n");
}

// ispisuje stack preko f-je za stampanje liste
void stack_dump(Stack* stk) {
	List l;
	l.head = stk->top;
	list_reverse(&l);
	list_dump(l);
	list_reverse(&l);
}

// ==============================================

// OPERATORS
typedef struct operator_t {
	char op;
	int priority;
	int unary;
} Operator;



// NORMALNA ALGEBRA
#ifndef USE_BOOLEAN
struct operator_t operators[] =
{
	{'-', 2},
	{'+', 2},
	{'-'|F_UNARY, 5},
	{'+'|F_UNARY, 5},
	{'*', 3},
	{'/', 3},
	{'^', 4},
	{'(', 1},
	{')', 1},
	{0,0}
};


NumType eval_operator(char op, NumType num1, NumType num2) {
	switch((unsigned char)op) {
		case '-':
			return num1-num2;
		case '+':
			return num1+num2;
		case '*':
			return num1*num2;
		case '/':
			if(num2 == 0) {
				sprintf(error, "deljenje sa 0\n");
				return 99999;
			}
			return num1/num2;
		case '^':
			return pow(num1,num2);
			
		// unarni operatori
		case '-'|F_UNARY:
			return num1-num2;
		case '+'|F_UNARY:
			return num1+num2;
	}
}
#endif

// VERZIJA SA BULOVOM ALGEBROM
#ifdef USE_BOOLEAN
struct operator_t operators[] =
{
	{'+', 2},
	{'!'|F_UNARY, 5},
	{'*', 3},
	{'^', 4},
	
	{'(', 1},
	{')', 1},
	{0,0}
};

NumType eval_operator(char op, NumType num1, NumType num2) {
	switch((unsigned char)op) {
		case '!'|F_UNARY:
			return !num2;
		case '+':
			return num1|num2;
		case '*':
			return num1&num2;
		case '^':
			return num1^num2;
	}
}
#endif

// broj operanda u zavisnosti da li je unarni ili ne
int op_operands(char op) {
	if(op & F_UNARY) {
		return 1;
	} else {
		return 2;
	}
}

// pronaci operator
Operator* op_get(char op) {
	Operator *o = operators;
	while(o->op != 0 && o->op != op) {
		o++;
	}
	if(o->op != 0) {
		o->unary = o->op & F_UNARY;
		return o;
	}
	return 0;
}

// prioritet operatora
int op_priority(char op) {
	Operator *oper = op_get(op);
	if(oper) {
		return oper->priority;
	}
	return -1;
}


////////



List tokenize(char* expr) {
	List res;
	list_init(&res);
	
	int r=0;
	int isunary = 1;
	int brackets = 0;
	
	while(*expr) {
		char c = *expr;
		if(c == ' ' || c == '\t' || c == '\n') {
			expr++;
			continue;
		}
		Operator *oper = op_get(c);
		if (!oper) {
			oper = op_get(c|F_UNARY);
		}
		
		if(oper) { // operator
			char op = oper->op;
			if(isunary && op != '(' && op != ')') {
				// proveriti da li postoji unarni operator
				if(op_get(op|F_UNARY)) {
					op |= F_UNARY;
				} else {
					sprintf(error, "ne postoji unarni operator %c\n", c);
				}
				// printf("is unary \n");
			}
			isunary = 1;
			if(op == '(') {
				brackets++;
			} else if(op == ')') {
				isunary = 0;
				brackets--;
			}
			
			list_append(&res, 0, op);
		} else if(c >= '0' && c <= '9' || c == '.') { // broj
			NumType r = 0;
			int base = 10;
			
			// na osnovu prefiksa 0 ili 0x odrediti bazu
			if (*expr == '0') { // oktalni broj
				base = 8;
				expr++;
				if(*expr == 'x') { // 0x.. heksadecimalni broj
					base = 16;
					expr++;
				} else if(*expr == 'b') { // 0b.. binarni broj
					base = 2;
					expr++;
				} else if(*expr != '.') {
					sprintf(error, "greska broj\n");
					return res;
				}
			}
			
			float zeros = 0;
			while ((c=*expr) && ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || c == '.')) {
				int n = c - '0';
				if(c >= 'a' && c <= 'f') {
					n = c - 'a' + 10;
				}
				if(c == '.') {
					zeros = 1;
					expr++;
					continue;
				}
				
				if(n >= base) {
					// error = GreskaBaza;
					sprintf(error, "greska baza\n");
					return res;
				}
				r = base*r + n;
				zeros *= base;
				expr++;
			}
			expr--;
			
			if(zeros == 0) zeros = 1;
			list_append(&res, r / zeros, 0);
			
			// posle broja ne moze ici unarni operator
			isunary = 0;
		} else {
			sprintf(error, "ne postoji operator %c\n", c);
			return res;
		}
		expr++;
	}
	
	if(brackets > 0) {
		sprintf(error, "fali )\n");
	} else if(brackets < 0) {
		sprintf(error, "fali (\n");
	} else if(isunary) {
		sprintf(error, "fali broj nakon operatora\n");
	}
	return res;
}


// pretvara iz infiksne u postfiksnu ili prefiksnu notaciju u zavisnosti od parametra "to_prefix" koji mora imati vrednost ili 0 ili 1
List infix_to_postpre(List infix, int to_prefix) {
	List res;
	list_init(&res);
	if(to_prefix) {
		list_reverse(&infix);
	}
	Stack op_stack;
	stack_init(&op_stack);
	list_elem* el = infix.head;
	while(el) {
		if(el->op == 0) { // number
			list_append(&res, el->value, 0);
		} else {
			char op = el->op;
			if(op == "()"[to_prefix]) { // (
				stack_push(&op_stack, 0, op); // push '('
			} else if(op == ")("[to_prefix]) { // )
				while (!stack_empty(&op_stack) && stack_peek(&op_stack)->op != "()"[to_prefix]) {
					list_append(&res, 0, stack_peek(&op_stack)->op);
					stack_pop(&op_stack);
				}
				stack_pop(&op_stack); // pop '('
			} else {
				if(op_operands(op) != 1) { // unarni operator ima najveci prioritet i moze se ponavljati vise puta
					while(!stack_empty(&op_stack) && op_priority(stack_peek(&op_stack)->op) >= (op_priority(op) + to_prefix)) {
						list_append(&res, 0, stack_peek(&op_stack)->op);
						stack_pop(&op_stack);
					}
				}
				stack_push(&op_stack, 0, op);
			}
			
		}
		el = el->next;
	}
	while(!stack_empty(&op_stack)) {
		list_append(&res, 0, stack_peek(&op_stack)->op);
		stack_pop(&op_stack);
	}
	if(to_prefix) {
		list_reverse(&res);
	}
	return res;
}

List infix_to_prefix(List infix) {
	return infix_to_postpre(infix, 1);
}
List infix_to_postfix(List infix) {
	return infix_to_postpre(infix, 0);
}


// izracunava postfiksnu notaciju
NumType eval_postfix(List postfix) {
	Stack num_stack;
	stack_init(&num_stack);
	list_elem* el = postfix.head;
	while(el) {
		if(el->op != 0) {
			NumType num1 = 0; // unarni operator ignorise num1
			NumType num2 = stack_peek(&num_stack)->value;
			stack_pop(&num_stack);
			char op = el->op;
			
			// ako nije unarni, uzeti jos jedan broj sa steka
			if(op_operands(op) == 2) {
				num1 = stack_peek(&num_stack)->value;
				stack_pop(&num_stack);
			}
			
			stack_push(&num_stack, eval_operator(op, num1, num2), 0);
		} else {
			stack_push(&num_stack, el->value, 0);
		}
		el = el->next;
	}
	NumType res = stack_peek(&num_stack)->value;
	stack_pop(&num_stack);
	return res;
}

// izracunava direktno prefiksnu notaciju
NumType eval_prefix(List prefix) {
	Stack num_stack;
	stack_init(&num_stack);
	Stack op_stack;
	stack_init(&op_stack);
	list_elem* el = prefix.head;
	int num = 0;
	char last_op = 0;
	while(el) {
		if(el->op != 0) {
			stack_push(&op_stack, num+op_operands(el->op), el->op);
		} else {
			num++;
			stack_push(&num_stack, el->value, 0);
			
			while(!stack_empty(&op_stack) && stack_peek(&op_stack)->value == num) {
				char op = stack_peek(&op_stack)->op;
				NumType num1 = 0;
				NumType num2 = stack_peek(&num_stack)->value;
				stack_pop(&num_stack);
				
				// ako nije unarni operator, uzeti jos jedan broj sa steka
				if(op_operands(op) == 2) {
					num1 = stack_peek(&num_stack)->value;
					stack_pop(&num_stack);
					num--;
				}
				stack_push(&num_stack, eval_operator(op, num1, num2), 0);
				stack_pop(&op_stack);
			}
		}
		el = el->next;
	}
	NumType res = stack_peek(&num_stack)->value;
	stack_pop(&num_stack);
	
	return res;
}

/*
int main() {
	List lst;
	list_init(&lst);

	// lst = tokenize("(-5-75+-25)*(3^-9+5^2)");
	// lst = tokenize("(-5-75-25)*(3^-9+5^2)");
	// lst = tokenize("7-8-9");
	// lst = tokenize("7-8-9^(2---+-2)");
	error[0] = 0;
	lst = tokenize("0b1111+(2+7^(2+1)*2)^2");
	if(strlen(error) > 0) {
		printf("Greska tokenizacija: %s\n", error);
		return 0;
	}
	
	// lst = tokenize("!!0+!0*!1^!0");
	list_dump(lst);
	List postfix = infix_to_postfix(lst);
	List prefix = infix_to_prefix(lst);
	
	printf("postfix: ");
	list_dump(postfix);
	printf("prefix: ");
	list_dump(prefix);
	
	printf("postfix solution " VAL_FORMAT "\n", eval_postfix(postfix));
	printf("prefix solution " VAL_FORMAT "\n", eval_prefix(prefix));
}
*/


// konverzija 
// baza = 8; // za oktalni
// baza = 16; // za heksa
// baza = 10; // za dekadni
void broj_u_string(NumType broj, char* out, int baza, int decimale) {
	int i=0,j=0;
	char tmp[100];
	tmp[0]=0;
	long int broj_int = fabs(broj);
	NumType broj_abs = fabs(broj);
	
	// celobrojni deo
	while(broj_int > 0) {
		int n = broj_int % baza;
		tmp[i++] = n + ((n >= 10) ? -10+'a' : '0');
		broj_int = broj_int / baza;
	}
	
	j=0;
	if (broj < 0) {
		out[j++] = '-';
	}
	
	if(broj_abs < 1) {
		out[j++] = '0';
	}
	
	int k;
	for(k=0; k < i; k++) {
		out[j++] = tmp[i-k-1];
	}
	
	//--------------------------------
	// decimalni deo (za floating point)
	broj_int = broj_abs;
	broj_abs -= broj_int;
	
	if(broj_abs > 0) {
		out[j++] = '.';
		while(decimale-- > 0) {
			broj_abs *= (NumType)baza;
			int n = broj_abs;
			out[j++] = n + ((n >= 10) ? -10+'a' : '0');
			broj_abs -= (long int)broj_abs;
		}
	}
	//---------------------------------
	
	out[j] = 0;
}

#define MAX 100

struct Izraz {
	char ispis[MAX];
	NumType vrednost;
};

struct Izraz izrazi[100];

void main( int argc, char *argv[]) {
	FILE* f = fopen("izrazi.txt", "r");
	FILE* f2 = fopen("izrazi2.txt", "w");
	
	char linija[MAX];
	if(!f) {
		printf("fajl izrazi.txt ne postoji\n");
		return;
	}
	int line=0,izr=0;
	while(fgets(linija, MAX, f)) {
		line++;
		
		// preskakanje linije
		if(linija[0] == '\n') {
			continue;
		}
		
		
		
		error[0]=0;
		// izrazZaRacun = linija;
		
		linija[strlen(linija)-1] = 0; // \n -> \0
		
		// printf("calculating %s\n", linija);
		
		List tokens = tokenize(linija);
		// if(strlen(error) > 0) {
			// printf("Greska tokenizacija: %s\n", error);
			// continue;
		// }
		
		// format:
		// <izraz> = <resenje>
		strcpy(izrazi[izr].ispis, linija);
		
		if(error[0] == 0) {
			NumType rez = eval_postfix(infix_to_postfix(tokens));
			izrazi[izr].vrednost = rez;
			
			if(error[0] == 0) {
				// = <resenje>
				char tmp[100];
				
				broj_u_string(rez, tmp, 10, 3);
				// printf("%f %s\n", rez, tmp);
				sprintf(linija, " = %s\n", tmp);
			}
		}
		if(error[0] != 0) {
			// ; <greska>
			sprintf(linija, " ; greska na liniji %d : %s", line, error);
		}
		strcat(izrazi[izr].ispis, linija);
		izr++;
	}
	fclose(f);
	
	// sortiranje
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
	
	for(i=0; i < izr; i++) {
		fputs(izrazi[i].ispis, f2);
		printf("%s", (izrazi[i].ispis));
	}
	
	fclose(f2);
}
