// ----- NE PREKUCAVATI (osim ako se trazi ispis u odredjenoj notaciji) ------
// ispisuje listu
void list_dump(List list) {
	list_elem* el = list.head;
	while(el) {
		if(el->op.op == 0) {
			printf(VAL_FORMAT " ", el->value);
		} else {
			if(el->op.type & UNARY) {
				printf("u%s ", el->op.op);
			} else {
				printf("%s ", el->op.op);
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
// ------------------------------------------------------------------------------


// NAJVEROVATNIJE NE TREBA PREKUCAVATI
// izracunava direktno prefiksnu notaciju (prekucati ukoliko se bas trazi izvrsavanje prefix notacije, mislim da nece trebati)
NumType eval_prefix(List prefix) {
	Stack num_stack;
	stack_init(&num_stack);
	Stack op_stack;
	stack_init(&op_stack);
	list_elem* el = prefix.head;
	int num = 0;
	while(el) {
		if(el->op.op != 0) {
			stack_push(&op_stack, num+op_operands(el->op), el->op);
		} else {
			num++;
			stack_push(&num_stack, el->value, null_op);
			
			while(!stack_empty(&op_stack) && stack_peek(&op_stack)->value == num) {
				Operator op = stack_peek(&op_stack)->op;
				NumType num1=0, num2=0;
				
				if(op_operands(op) > 0) {
					if(stack_empty(&num_stack)) {
						sprintf(error, "fali operand za operator %s\n", op.op);
						return 0;
					}
					num2 = stack_peek(&num_stack)->value;
					stack_pop(&num_stack);
				}

				// ako nije unarni, uzeti jos jedan broj sa steka
				if(op_operands(op) == 2) {
					if(stack_empty(&num_stack)) {
						sprintf(error, "fali operand za operator %s\n", op.op);
						return 0;
					}
					num1 = stack_peek(&num_stack)->value;
					stack_pop(&num_stack);
					num--;
				}
				
				stack_push(&num_stack, eval_operator(op, num1, num2), null_op);
				stack_pop(&op_stack);
			}
		}
		el = el->next;
	}
	if(!stack_empty(&op_stack)) {
		sprintf(error, "fali operand za operator %s\n", stack_peek(&op_stack)->op.op);
	}
	NumType res = stack_peek(&num_stack)->value;
	stack_pop(&num_stack);
	
	return res;
}

// PREKUCATI UKOLIKO SE KORISTI ISPIS U BAZI KOJA NIJE 10

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
	// OPCIONO: UKOLIKO SE KORISTI DECIMALNI ISPIS
	// decimalni deo (za floating point)
	broj_int = broj_abs;
	broj_abs -= broj_int;
	
	if(broj_abs > 0) {
		if(decimale > 0) {
			out[j++] = '.';
		}
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




// --------------------------------------------------------------
// NORMALNA ALGEBRA (prekucaj ukoliko se koristi normalna algebra)
#ifndef USE_BOOLEAN
Operator operators[] =
{
	{"-", 2, BINARY|UNARY},
	{"+", 2, BINARY|UNARY},
	{"*", 3},
	{"/", 3},
	{"^", 4},
	{"(", 1},
	{")", 1},
	{"sin", 0, FUNC},
	{"cos", 0, FUNC},
	{"sqrt", 0, FUNC},
	{"pi", 0, VARIABLE},
	{0,0}
};

NumType eval_operator(Operator op, NumType num1, NumType num2) {
	switch(op.op[0]) {
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
	}
	
	if(!strcmp(op.op, "sin")) {
		return sin(num2);
	}
	if(!strcmp(op.op, "cos")) {
		return cos(num2);
	}
	if(!strcmp(op.op, "sqrt")) {
		return sqrt(num2);
	}
	if(!strcmp(op.op, "pi")) {
		return 3.14;
	}
}
#endif
// --------------------------------------------------------------



// --------------------------------------------------------------
// VERZIJA SA BULOVOM ALGEBROM (prekucaj ukoliko se koristi bulova algebra)
#ifdef USE_BOOLEAN
Operator operators[] =
{
	{"+", 2},
	{"!", 0, UNARY},
	{"*", 3},
	{"^", 4},
	
	{"(", 1},
	{")", 1},
	{0,0}
};

NumType eval_operator(Operator op, NumType num1, NumType num2) {
	switch(op.op[0]) {
		case '!':
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
// --------------------------------------------------------------
