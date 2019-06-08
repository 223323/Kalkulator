#include <stdio.h>
#include <math.h>
#include <string.h>
const char *izrazZaRacun = 0;
#define MAX 100
typedef float Tip;
Tip sabirak();

enum Errors {
	NemaGreske,
	OcekujemBroj,
	FaliZagrada,
	FaliOperator,
	DupliOperator,
	GreskaBaza,
	NepoznataFunkcija
};

char* error_translation[] = {
	"NoError",
	"OcekujemBroj",
	"FaliZagrada",
	"FaliOperator",
	"DupliOperator",
	"GreskaBaza (Baza)",
	"NepoznataFunkcija"
};

int error=0;

char peek () {
	return *izrazZaRacun;
}

char get () {
	return *izrazZaRacun++;
}

Tip broj() {
	Tip r = 0;
	int base = 10;
	
	// na osnovu prefiksa 0 ili 0x odrediti bazu
	if (peek() == '0') {
		base = 8;
		get();
		if(peek() == 'x') {
			base = 16;
			get();
		}
	}
	
	float zeros = 0;
	while ((peek() >= '0' && peek() <= '9') || (peek() >= 'a' && peek() <= 'f') || peek() == '.') {
		int n = peek() - '0';
		if(peek() >= 'a' && peek() <= 'f') {
			n = peek() - 'a' + 10;
		}
		if(peek() == '.') {
			zeros = 1;
			get();
			continue;
		}
		get();
		if(n >= base) {
			error = GreskaBaza;
		}
		r = base*r + n;
		zeros *= base;
	}
	return r / fmax(1, zeros);
}

//sabiranje i oduzimanje
// 5+6+7-5
Tip izraz() {
	Tip r = sabirak();
	
	// provera za greske, nakon svakog poziva cinilaca ili sabiraka
	if(error != 0) {
		return 0;
	}
	
	// preskace razmake
	while(peek() == ' ') get();
	
	while (peek()=='+' || peek()=='-')
	{
		if (get()=='+') {
			r += sabirak();
		} else {
			r -= sabirak();
		}
		
		// preskace razmake
		while(peek() == ' ') get();
	}
	return r;
}



//zagrada kao prioritet
Tip cinilac() {
	Tip r=0;
	
	// provera za greske, nakon svakog poziva cinilaca ili sabiraka
	if(error != 0) {
		return 0;
	}

	// preskace razmake
	while(peek() == ' ') get(); 
	
	if (peek() >= '0' && peek() <= '9') { // broj
		r = broj();
	}
	else if (peek() == '(') // zagrade
	{
		get(); // '('
		r = izraz();
		if (error != 0) {
			return 0;
		}
		if (peek() == ')') {
			get(); // ')'
		} else {
			error = FaliZagrada;
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
		char c = peek();
		if (c=='*' || c =='/' || c == '+' || c == '-' || c == '^') {
			error = DupliOperator;
		} else {
			error = OcekujemBroj;
		}
	}
	
	if (peek() == '^') {
		error=0;
		get(); // '^'
		r = pow(r, cinilac()); // stepenovanje
	}
	
	return r; // rezultat
}



//mnozenje i deljenje sabiraka
// da bi bilo *i / prioriteti pozivamo funkciju sabiranje umesto fje broj
Tip sabirak() {
	Tip r = cinilac();
	
	// provera za greske, nakon svakog poziva cinilaca ili sabiraka
	if(error != 0) {
		return 0;
	}
	
	// preskace razmake
	while(peek() == ' ') get();
	
	// 2*(5+7)/4+8
	while (peek() == '*' || peek()== '/') {
		if (get() == '*') {
			r *= cinilac();
		} else {
			r /= cinilac();
		}
		
		// preskace razmake
		while(peek() == ' ') get();
	}
	return r;
}


// konverzija 
// baza = 8; // za oktalni
// baza = 16; // za heksa
// baza = 10; // za dekadni
void broj_u_string(Tip broj, char* out, int baza, int decimale) {
	int i=0,j=0;
	char tmp[100];
	tmp[0]=0;
	long int broj_int = fabs(broj);
	Tip broj_abs = fabs(broj);
	
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
			broj_abs *= (Tip)baza;
			int n = broj_abs;
			out[j++] = n + ((n >= 10) ? -10+'a' : '0');
			broj_abs -= (long int)broj_abs;
		}
	}
	//---------------------------------
	
	out[j] = 0;
}

struct Izraz {
	char ispis[MAX];
	Tip vrednost;
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
		
		error=0;
		izrazZaRacun = linija;
		
		linija[strlen(linija)-1] = 0; // \n -> \0
		
		// format:
		// <izraz> = <resenje>
		strcpy(izrazi[izr].ispis, linija);
		Tip rez = izraz();
		izrazi[izr].vrednost = rez;
		if(error == 0) {
			// = <resenje>
			char tmp[100];
			
			broj_u_string(rez, tmp, 10, 3);
			// printf("%f %s\n", rez, tmp);
			sprintf(linija, " = %s\n", tmp);
			
		} else {
			// ; <greska>
			sprintf(linija, " ; greska na liniji %d : %s\n", line, error_translation[error]);
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
	}
	
	fclose(f2);
}

