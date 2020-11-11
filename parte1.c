#include <math.h>
#include <stdio.h>

#define EPS 1e-12

// A função f do enunciado, que buscaremos as três raízes
double f(double x) { return pow(M_E, x) - 2 * x * x; }

// Uma função g que vamos usar para o método do ponto fixo
// g(x) = x - f(x) / (e^x - 4x)
double g(double x) { return x - f(x) / (pow(M_E, x) - 4 * x); }

// Método do ponto fixo
double fixo(double x0) {
	double x_i = x0;
	double fx_ant;
	double fx_i = f(x_i);

	while (fabs(fx_i) > EPS) {
		printf("f(%lf) = %lf\n", x_i, fx_i);
		x_i = g(x_i);
		fx_ant = fx_i;
		fx_i = f(x_i);

		// Se o valor da função aumentou
		if (fabs(fx_ant) < fabs(fx_i)) { break; }
	}
	printf("Final: f(%lf) = %lf\n", x_i, fx_i);
	return x_i;
}

int main() {
	double x0;
	double root;
	printf("Informe um x_0 para começar: ");
	scanf("%lf", &x0);

	root = fixo(x0);
	printf("\n\nConvergiu para %lf\n", root);

	return 0;
}