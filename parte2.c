// ********* BIBLIOTECAS E DEFINIÇÕES *********

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define EPS 1e-12

typedef struct {
	double v1;
	double v2;
} point;

typedef enum { FALSE, TRUE } Bool;

// ********* ARITMÉTICA DE NÚMEROS COMPLEXOS *********

typedef struct {
	double Re;
	double Im;
} Complex;

Complex addComplex(Complex, Complex);  // Função de soma
Complex subComplex(Complex, Complex);  // Função de subtração
Complex multComplex(Complex, Complex); // Função de multiplicação
Complex divComplex(Complex, Complex);  // Função de divisão
Complex powComplex(Complex, int);      // Função de pontencia (expoente inteiro)
Bool equalComplex(Complex, Complex);   // Função de igualdade com base em EPS
void printComplex(Complex);            // Função que imprime um número complexo

const Complex ComplexZero = { 0, 0 };
const Complex ComplexI = { 0, 1 };
const Complex ComplexOne = { 1, 0 };

// ********* FUNÇÕES AVALIADAS *********

// Função 1 (x^6 + 1)

Complex f1(Complex x) { return addComplex(powComplex(x, 6), ComplexOne); }

Complex Df1(Complex x) { return multComplex((Complex){ 6, 0 }, powComplex(x, 5)); }

// Função 2 (x^6 - 1)

Complex f2(Complex x) { return subComplex(powComplex(x, 6), ComplexOne); }

Complex Df2(Complex x) { return multComplex((Complex){ 6, 0 }, powComplex(x, 5)); }

// Função 3 (x^6 + x^1)

Complex f3(Complex x) { return addComplex(powComplex(x, 6), x); }

Complex Df3(Complex x) {
	return addComplex(multComplex((Complex){ 6, 0 }, powComplex(x, 5)), ComplexOne);
}

// Função 4 (x^6 - x^1)

Complex f4(Complex x) { return subComplex(powComplex(x, 6), (Complex){ 2, 1 }); }

Complex Df4(Complex x) {
	return subComplex(multComplex((Complex){ 6, 0 }, powComplex(x, 5)), ComplexOne);
}

// Função 5 (x^3 - 1)

Complex f5(Complex x) { return subComplex(powComplex(x, 3), ComplexOne); }

Complex Df5(Complex x) { return multComplex((Complex){3,0}, powComplex(x, 2)); }

// ********* MÉTODO DE NEWTON *********

void newton_basins(point l, point u, point p, Complex (*evalf)(Complex),
                   Complex (*evalDf)(Complex));

void newton(Complex *x, Complex (*evalf)(Complex), Complex (*evalDf)(Complex),
            Bool *convergs);

// ********* IMPLEMENTAÇÕES *********

int main() {
	int c;
	system("clear");
	printf("Iniciando programa\n");

	printf("CUIDADO! Todos os .dat da pasta serão deletados,\ndigite 1 se quiser "
	       "continuar com o programa: ");

	scanf("%d", &c);

	system("rm *.dat");

	// Intervalo de x, intervalo de y, resolução, função e sua derivada
	newton_basins((point){ -2, 2 }, (point){ -2, 2 }, (point){ 2000, 2000 }, f5,
	              Df5);

	printf("Fim programa\n");

	return 0;
}

void newton_basins(point l, point u, point p, Complex (*evalf)(Complex),
                   Complex (*evalDf)(Complex)) {
	double deltaX, deltaY;
	FILE *out;          // Arquivo de saída
	char fileName[100]; // Nome do arquivo de saída
	double x, y;        // O número a ser considerado é da forma x + yi
	double xInc, yInc;  // Incrementos em cada eixo
	Complex arr[10];    // Array com todas as raízes encontradas até agora
	int arrLen = 0;     // Tamanho do array arr
	Complex x0;         // O ponto a ser considerado
	Complex xN;         // O ponto de convergência
	Bool convergs;      // Se aquele ponto convergiu ou não
	int it;             // Iterador sobre arr
	Bool find;          // Booleano que descobre se a raíz já
	                    // foi encontrada por outro ponto

	convergs = TRUE;

	deltaX = l.v2 - l.v1; // intervalo em x
	deltaY = u.v2 - u.v1; // intervalo em y

	xInc = deltaX / p.v1; // incremento em x
	yInc = deltaY / p.v2; // incremento em y

	for (x = l.v1; x <= l.v2; x += xInc) {
		for (y = u.v1; y <= u.v2; y += yInc) {
			// Para cada ponto do domínio escolhido:

			// O x0 é o ponto atual, xN é o ponto de convergência
			x0.Re = x;
			x0.Im = y;
			xN.Re = x;
			xN.Im = y;

			printComplex(x0);
			printf(": ");

			// Chamamos o método de Newton para ver se converge
			newton(&xN, evalf, evalDf, &convergs);

			// Se não convergiu
			if (!convergs) {
				// Imprime no arquivo dos pontos que não convergem
				out = fopen("not.dat", "a");
				fprintf(out, "%lf %lf\n", x0.Re, x0.Im);
				fclose(out);

				printf("diverge.\n");

			} else { // Se convergiu

				printf("converge para ");
				printComplex(xN); // xN é a raiz encontrada
				printf("\n");

				// Descobre se algum outro ponto já convergiu para essa raiz
				find = FALSE;
				for (it = 0; it < arrLen; it++) {
					if (equalComplex(xN, arr[it])) { // Se já achou a raiz antes

						// Imprime no arquivo da mesma cor que arr[it]
						sprintf(fileName, "%d.dat", it);
						out = fopen(fileName, "a");
						fprintf(out, "%lf %lf\n", x0.Re, x0.Im);
						fclose(out);

						// Marca que achou uma raíz que já foi acha anteriormente
						find = TRUE;
						break; // Podemos parar de procurar
					}
				}

				// Se é a primeira vez que convergimos para essa raiz:
				if (find == FALSE) {
					// Anote a raiz no array
					arr[it].Re = xN.Re;
					arr[it].Im = xN.Im;
					arrLen++; // Aumente o tamanho do array

					// Cria e imprime num arquivo de nova cor
					sprintf(fileName, "%d.dat", it);
					out = fopen(fileName, "a");
					fprintf(out, "%lf %lf\n", x0.Re, x0.Im);
					fclose(out);
				}
			}
		}
	} // Fim for
	  // Lembrando que imprimimos x0, que é o ponto inicial.
	  // xN é a raiz para a qual ele convergiu (se convergiu)
}

void newton(Complex *x, Complex (*evalf)(Complex), Complex (*evalDf)(Complex),
            Bool *convergs) {
	Complex fx;    // O valor de f para o x_i atual
	Complex Dfx;   // O valor de f' para o x_i atual
	int numIt = 0; // Número de iterações
	*convergs = TRUE;

	fx = evalf(*x);
	Dfx = evalDf(*x);

	while (!equalComplex(fx, ComplexZero) && numIt < 50) {
		*x = subComplex(*x, divComplex(fx, Dfx)); // x_{i+1} = x_i - f(x_i) / f'(x_i)
		// Iteramos x e agora atualizamos f(x) e f'(x)
		fx = evalf(*x);
		Dfx = evalDf(*x);

		numIt++; // Contamos uma iteração
	}

	// Se o método terminou por conta do número de iterações, consideramos que não
	// convergiu
	if (numIt == 50) *convergs = FALSE;

	// E é isso, se convergs = TRUE, a raiz está em x, que é retornado por referência
}

// ********* ARITMÉTICA DE NÚMEROS COMPLEXOS - IMPLEMENTAÇÃO *********

Complex addComplex(Complex a, Complex b) {
	Complex c;
	c.Re = a.Re + b.Re;
	c.Im = a.Im + b.Im;
	return c;
}
Complex subComplex(Complex a, Complex b) {
	Complex c;
	c.Re = a.Re - b.Re;
	c.Im = a.Im - b.Im;
	return c;
}
Complex multComplex(Complex a, Complex b) {
	Complex c;
	c.Re = (a.Re * b.Re - a.Im * b.Im);
	c.Im = (a.Re * b.Im + a.Im * b.Re);
	return c;
}
Complex divComplex(Complex a, Complex b) {
	Complex c;
	c.Re = (a.Re * b.Re + a.Im * b.Im) / (b.Re * b.Re + b.Im * b.Im);
	c.Im = (a.Im * b.Re - a.Re * b.Im) / (b.Re * b.Re + b.Im * b.Im);
	return c;
}
Complex powComplex(Complex b, int exp) {
	Complex c;
	c.Re = 1;
	c.Im = 0;

	for (int i = 0; i < exp; i++) { c = multComplex(c, b); }
	return c;
}
Bool equalComplex(Complex a, Complex b) {
	return (fabs(a.Re - b.Re) < EPS && fabs(a.Im - b.Im) < EPS);
}
void printComplex(Complex a) { printf("%.10lf + %.10lfi", a.Im, a.Im); }
