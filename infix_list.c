#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
const char *izrazZaRacun = 0;
#define MAX 100
int sabirak();
char error[100];

char peek () {
	return *izrazZaRacun;
}

char get () {
	return *izrazZaRacun++;
}

int broj() {
	int r = 0;
	while ((peek() >= '0' && peek() <= '9')) {
		r = 10*r + get() - '0';
	}
	
	return r;
}

//sabiranje i oduzimanje
// 5+6+7-5
int izraz() {
	int r = sabirak();
	// provera za greske, nakon svakog poziva cinilaca ili sabiraka
	if(error[0] != 0) return 0;
	while(peek() == ' ') get(); // preskace razmake
	while (peek()=='+' || peek()=='-') {
		if(error[0] != 0) return 0;
		if (get()=='+') {
			r += sabirak();
		} else {
			r -= sabirak();
		}
		while(peek() == ' ') get(); // preskace razmake
	}
	while(peek() == ' ') get(); // preskace razmake
	if(peek() != 0 && peek() != ')') {
		if(isdigit(peek())) {
			sprintf(error, "fali operator");
		} else {
			sprintf(error, "fali operand");
		}
	}
	return r;
}



//zagrada kao prioritet
int cinilac() {
	int r=0;
	// provera za greske, nakon svakog poziva cinilaca ili sabiraka
	if(error[0] != 0) return 0;
	// preskace razmake
	while(peek() == ' ') get(); 
	if (peek() >= '0' && peek() <= '9') // broj
	{
		r = broj();
	}
	else if (peek() == '(') // zagrade
	{ 
		get(); // '('
		r = izraz();
		if(error[0] != 0) return 0;
		if (peek() == ')') {
			get(); // ')'
		} else {
			sprintf(error, "fali zagrada");
		}
	} 
	// 8/-5
	// -5+7
	else if (peek() == '-')
	{
		// unarni operator -
		// npr. -5
		get(); // '-'
		r = -cinilac();
	}
	else if (peek() == '+')
	{
		// unarni operator +
		// npr. +5
		get(); // '+'
		r = cinilac();
	} else {
		// nije 0-9, (, -, +
		sprintf(error, "fali operand");
		return 0;
	}
	
	if (peek() == '^') {
		error[0] = 0;
		get(); // '^'
		r = pow(r, cinilac()); // stepenovanje
	}
	
	return r; // rezultat
}

//mnozenje i deljenje sabiraka
// da bi bilo *i / prioriteti pozivamo funkciju sabiranje umesto fje broj
int sabirak() {
	int r = cinilac();
	if(error[0] != 0) return 0; // provera za greske, nakon svakog poziva cinilaca ili sabiraka
	while(peek() == ' ') get(); // preskace razmake
	// 2*(5+7)/4+8
	while (peek() == '*' || peek()== '/') {
		if(error[0] != 0) return 0;
		if (get() == '*') {
			r *= cinilac();
		} else {
			r /= cinilac();
		}
		while(peek() == ' ') get(); // preskace razmake
	}
	return r;
}

struct Izraz {
	char ispis[MAX];
	int vrednost;
	struct Izraz* next;
};

struct Izraz *izrazi_head = 0, *izrazi_tail = 0;
int izrazi_count = 0;
void push() {
	struct Izraz *el = (struct Izraz *)malloc(sizeof(struct Izraz));
	el->next = 0;
	if(!izrazi_tail) {
		izrazi_head = izrazi_tail = el;
	} else {
		izrazi_tail->next = el;
		izrazi_tail = el;
	}
	izrazi_count++;
}

void main( int argc, char *argv[]) {
	int line=0,izr=0;
	int i,j;
	FILE* f = fopen(argv[1], "r");
	char linija[MAX];

	if(!f) {
		printf("fajl %s ne postoji\n", argv[1]);
		return;
	}
	
	while(fgets(linija, MAX, f)) {
		
		line++;
		// preskakanje linije
		char* c = linija;
		while(isblank(*c++));
		if(*c == 0)
			continue;
		
		error[0]=0;
		linija[strlen(linija)-1] = 0; // \n -> \0
		izrazZaRacun = linija;
		push();
		izrazi_tail->vrednost = izraz();
		// izrazi[izr].vrednost = izraz();
		if(peek() == ')') sprintf(error, "fali zagrada");
		if(error[0] == 0) {
			// = <resenje>
			sprintf(izrazi_tail->ispis, "%s = %d", linija, izrazi_tail->vrednost);
			
		} else {
			// ; <greska>
			sprintf(izrazi_tail->ispis, "%s ; greska na liniji %d : %s", linija, line, error);
		}
	}
	fclose(f);
	printf("fajl %s sadrzi %d izraza\n", argv[1], izr);
	printf("-------------------------------------\n");
	
	// sortiranje
	struct Izraz *a,*b,*t,*ap=0,*bp=0,*an=0,*bn=0;
	for(a=izrazi_head; a; a=a->next) {
		for(bp=a,b=a->next; b; b=b->next) {
			if(a->vrednost > b->vrednost) {
				
				an = a->next;
				bn = b->next;
				if(an == b) {
					b->next = a;
				} else {
					b->next = an;
				}
				
				if(bn == a) {
					a->next = b;
				} else {
					a->next = bn;
				}
				
				if(ap && ap != b) {
					ap->next = b;
				}
					
				if(bp && bp != a) {
					bp->next = a;
				}
				
				
				if(izrazi_tail == a) {
					izrazi_tail = b;
				} 
				else if(izrazi_tail == b) {
					izrazi_tail = a;
				}
				
				if(izrazi_head == a) {
					izrazi_head = b;
				}
				else if(izrazi_head == b) {
					izrazi_head = a;
				}
				
				t=a;
				a=b;
				b=t;
			}
			bp = b;
		}
		ap = a;
	}
	for(a=izrazi_head; a; a=a->next) {
		printf("%s\n", a->ispis);
	}
}
