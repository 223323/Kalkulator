all: kalkulator kalkulator_sa_funkcijama kalkulator_sa_stekom

kalkulator: rekurzivno.c
	gcc $^ -o $@ -lm -g

kalkulator_sa_funkcijama: rekurzivno_sa_funkcijama.c
	gcc $^ -o $@ -lm -g

kalkulator_sa_stekom: with_stack.c
	gcc $^ -o $@ -lm -g

clean:
	rm -f kalkulator kalkulator_sa_funkcijama kalkulator_sa_stekom
