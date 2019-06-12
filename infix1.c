#include <stdio.h>
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
};

struct Izraz izrazi[100];

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
		izrazi[izr].vrednost = izraz();
		if(peek() == ')') sprintf(error, "fali zagrada");
		if(error[0] == 0) {
			// = <resenje>
			sprintf(izrazi[izr].ispis, "%s = %d", linija, izrazi[izr].vrednost);
			
		} else {
			// ; <greska>
			sprintf(izrazi[izr].ispis, "%s ; greska na liniji %d : %s", linija, line, error);
		}	
		izr++;
	}
	fclose(f);
	printf("fajl %s sadrzi %d izraza\n", argv[1], izr);
	printf("-------------------------------------\n");
	// sortiranje
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
		printf("%s\n",izrazi[i].ispis);
	}
}
