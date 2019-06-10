/*
// NE TREBA PREKUCAVATI - SAMO ZA TESTIRANJE
int main() {
	List lst;
	list_init(&lst);

	// lst = tokenize("(-5-75+-25)*(3^-9+5^2)", 1);
	lst = tokenize("(-5-75-25)*(3^-9+5^2)", 1);
	// lst = tokenize("7-8-9", 1);
	// lst = tokenize("7-8-9^(2---+-2)", 1);
	error[0] = 0;
	// lst = tokenize("0b1111+(2+7^(2+1)*2)^2", 1);
	if(strlen(error) > 0) {
		printf("Greska tokenizacije: %s\n", error);
		return 0;
	}
	
	// lst = tokenize("!!0+!0*!1^!0");
	// lst = tokenize("15 2 7 2 1 + ^ 2 * + 2 ^ +", 0);
	// lst = tokenize("+ 15 ^ + 2 * ^ 7 + 2 1 2 2", 0);
	list_dump(lst);
	printf("postfix: %d\n", eval_postfix(infix_to_postfix(lst)));
	return 0;
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
