
#include <math.h>
#include <stdlib.h>

double rand_gauss_central_limit(int k) {
	double x = 0;
	int i;
	for (i = 0; i < k; i++)
		x += (double)rand() / RAND_MAX;

	x -= k / 2.0;
	x /= sqrt(k / 12.0);

	return x;
}

double rand_gauss_marsaglia(void) {
	static double V1, V2, S;
	static int phase = 0;
	double X;

	if (phase == 0) {
		do {
			double U1 = (double)rand() / RAND_MAX;
			double U2 = (double)rand() / RAND_MAX;

			V1 = 2 * U1 - 1;
			V2 = 2 * U2 - 1;
			S = V1 * V1 + V2 * V2;
		} while (S >= 1 || S == 0);

		X = V1 * sqrt(-2 * log(S) / S);
	} else
		X = V2 * sqrt(-2 * log(S) / S);

	phase = 1 - phase;

	return X;
}