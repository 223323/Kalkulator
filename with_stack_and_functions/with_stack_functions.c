#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


#define USE_FLOAT

// PAZNJA: ne moze boolean algebra sa decimalnim brojevima (USE_FLOAT) da radi
// #define USE_BOOLEAN

#ifdef USE_FLOAT

typedef float NumType;
#define VAL_FORMAT "%.2f"

#else

typedef int NumType;
#define VAL_FORMAT "%d"

#endif


char error[100];


#define BINARY 1
#define UNARY 2
#define FUNC 4
#define VARIABLE 8

typedef struct operator_t {
	char* op;
	int priority;
	int type;
} Operator;

// stack i liste
typedef struct list_elem {
	NumType value;
	Operator op;
	struct list_elem *next;
} list_elem;

// STACK
typedef struct stack_t {
	list_elem *top;
} Stack;

Operator operators[];
NumType eval_operator(Operator op, NumType num1, NumType num2);
Operator null_op = {0,0,0};

// inicijalizuje stek
void stack_init(Stack* stack) {
	stack->top = 0;
}

// da li je stek prazan
int stack_empty(Stack* stack) {
	return stack->top == 0;
}

// dodaje na stek
void stack_push(Stack* stack, NumType value, Operator op) {
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
void list_append(List* list, NumType value, Operator op) {
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

// UVEK PREKUCATI
// ==============================================================
// broj operanda u zavisnosti da li je unarni ili ne
int op_operands(Operator op) {
	if(op.type & (UNARY|FUNC)) {
		return 1;
	} else {
		if(op.type == VARIABLE) {
			return 0;
		} else {
			return 2;
		}
	}
}

// pronaci operator
Operator* op_get(char* op) {
	Operator *o = operators;
	while(o->op != 0 && strstr(op, o->op) != op) {
		o++;
	}
	if(o->op != 0) {
		return o;
	}
	return 0;
}

// prioritet operatora
int op_priority(Operator op) {
	return op.priority + (op.type & (UNARY|FUNC|VARIABLE) ? 5 : 0);
}


#include "addons.h"

List tokenize(char* expr, int is_infix) {
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
		Operator *oper = op_get(expr);
		
		if(oper) { // operator
			Operator op = *oper;
			
			if(is_infix) {
				if(oper->type == VARIABLE) {
					isunary = 0;
				} else {
					if(isunary && c != '(' && c != ')') {
						// proveriti da li postoji unarni operator
						if(oper->type & (UNARY|FUNC)) {
							op.type = UNARY;
						} else {
							sprintf(error, "ne postoji unarni operator %c\n", c);
						}
					} else {
						op.type &= ~UNARY;
					}
					isunary = 1;
				}
			}

			if(c == '(') {
				brackets++;
			} else if(c == ')') {
				isunary = 0;
				brackets--;
			}
			
			list_append(&res, 0, op);
			expr += strlen(oper->op);
			continue;
		} else if(c >= '0' && c <= '9' || c == '.') { // broj
			NumType r = 0;
			int base = 10;
			
			// --------------
			// PREKUCATI SAMO AKO SE TRAZI prepoznavanje baze na osnovu prefiksa broja 0.., 0x.., 0b..
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
				}
			}
			// ---------------
			
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
					sprintf(error, "greska baza\n");
					return res;
				}
				r = base*r + n;
				zeros *= base;
				expr++;
			}
			expr--;
			list_append(&res, r / fmax(1, zeros), null_op);
			
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
List infix_to_postprefix(List infix, int to_prefix) {
	List res;
	list_init(&res);
	if(to_prefix) {
		list_reverse(&infix);
	}
	Stack op_stack;
	stack_init(&op_stack);
	list_elem* el = infix.head;
	while(el) {
		if(el->op.op == 0) { // number
			list_append(&res, el->value, null_op);
		} else {
			Operator op = el->op;
			if(op.op[0] == "()"[to_prefix]) { // (
				stack_push(&op_stack, 0, op); // push '('
			} else if(op.op[0] == ")("[to_prefix]) { // )
				while (!stack_empty(&op_stack) && stack_peek(&op_stack)->op.op[0] != "()"[to_prefix]) {
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
	return infix_to_postprefix(infix, 1);
}
List infix_to_postfix(List infix) {
	return infix_to_postprefix(infix, 0);
}
// ==============================================================

NumType eval_postfix(List postfix) {
	Stack num_stack;
	stack_init(&num_stack);
	list_elem* el = postfix.head;
	while(el) {
		if(el->op.op != 0) {
			NumType num1=0, num2=0;
			Operator op = el->op;
			// printf("op %s %d\n", op.op, op_operands(op));
			if(op_operands(op) > 0) {
				num2 = stack_peek(&num_stack)->value;
				stack_pop(&num_stack);
			}

			// ako nije unarni, uzeti jos jedan broj sa steka
			if(op_operands(op) == 2) {
				num1 = stack_peek(&num_stack)->value;
				stack_pop(&num_stack);
			}
			
			stack_push(&num_stack, eval_operator(op, num1, num2), null_op);
		} else {
			stack_push(&num_stack, el->value, null_op);
		}
		el = el->next;
	}
	NumType res = stack_peek(&num_stack)->value;
	stack_pop(&num_stack);
	return res;
}


#define MAX 100

struct Izraz {
	char ispis[MAX];
	NumType vrednost;
};

struct Izraz izrazi[100];

void main( int argc, char *argv[]) {
	
	if(argc < 2) {
		printf("pravilan unos\n");
		printf("\tprogram <ime_fajla>\n");
		return;
	}
	
	FILE* f = fopen(argv[1], "r");
	// ukoliko se koristi ispis u fajl
	// FILE* f2 = fopen("izrazi2.txt", "w"); 
	
	char linija[MAX];
	if(!f) {
		printf("fajl %s ne postoji\n", argv[1]);
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
		linija[strlen(linija)-1] = 0; // \n -> \0
		// printf("calculating %s\n", linija);
		
		// UKOLIKO U IZRAZI.TXT NIJE INFIX FORMAT, POSTAVITI OVO NA 0 I ISPOD PO POTREBI eval_postfix ZAMENITI SA eval_prefix KOJU TREBA UBACITI U addons.h
		int infix_input = 1;
		
		List tokens = tokenize(linija, infix_input);

		// format:
		// <izraz> = <resenje>
		strcpy(izrazi[izr].ispis, linija);
		
		if(error[0] == 0) {
			if(infix_input) {
				tokens = infix_to_postfix(tokens);
			}
			
			NumType rez = eval_postfix(tokens); // UKOLIKO SE TRAZI PREFIKSNA FORMA ZAMENITI sa eval_prefix
			izrazi[izr].vrednost = rez;
			
			if(error[0] == 0) {
				// = <resenje>
				char tmp[100];
				
				// TODO: promeniti ovde bazu za ispis u oktalnim i sl.
				// broj_u_string(rez, tmp, 10, 3);
				sprintf(tmp, VAL_FORMAT, rez); // UKOLIKO se ne koristi broj_u_string
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
			// ukoliko se koristi ispis u fajl
			// fputs(izrazi[i].ispis, f2);
			printf("%s", (izrazi[i].ispis));
		}
	}
	// ukoliko se koristi ispis u fajl
	// fclose(f2);
}
