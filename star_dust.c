#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

# define byte1 0xff000000
# define byte2 0x00ff0000
# define byte3 0x0000ff00
# define byte4 0x000000ff

void free_mat1(int n, int **a)//functie de elibarare a memoriei pt int
{
	for (int i = 0; i < n; i++)
		free(a[i]);
	free(a);
}

void free_mat2(int n, bool **a)//functie de eliberare a memoriei pt bool
{
	for (int i = 0; i < n; i++)
		free(a[i]);
	free(a);
}

int complement(char x)//functie care calculeaza complementul fata de 2
{
	x = ~x;
	return (-1) * (x + 1);
}

void task_one(int **a, int *m, int n)
{
	double s = 0, m_a = 0, nr = 0;//calculul sumei de pe contur
	for (int i = 0; i < m[0]; i++) {
		s = s + complement((a[0][i] & byte1) >> (3 * 8));
		s = s + complement((a[0][i] & byte2) >> (2 * 8));
		s = s + complement((a[0][i] & byte3) >> (1 * 8));
		s = s + complement((a[0][i] & byte4) >> (0 * 8));
		nr = nr + 4;
	}
	for (int i = 0; i < m[n - 1]; i++) {
		s = s + complement((a[n - 1][i] & byte1) >> (3 * 8));
		s = s + complement((a[n - 1][i] & byte2) >> (2 * 8));
		s = s + complement((a[n - 1][i] & byte3) >> (1 * 8));
		s = s + complement((a[n - 1][i] & byte4) >> (0 * 8));
		nr = nr + 4;
	}
	for (int i = 1; i < n - 1; i++) {
		s = s + complement((a[i][0] & byte4) >> (0 * 8));
		nr = nr + 1;
	}
	for (int i = 1; i < n - 1; i++) {
		s = s + complement((a[i][m[i] - 1] & byte1) >> (3 * 8));
		nr = nr + 1;
	}

	m_a = 1. * s / nr;//calcul medie aritmetica
	printf("task 1\n%.8f\n", m_a);
}

void task_two(int **a, int *m, int n)
{
	printf("task 2\n");//afisarea matricei dupa modificari
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m[i]; j++)
			printf("%08X ", a[i][j]);
		printf("\n");
	}
}

int replace(int x, int v, int rest, int dim)
{///functie care inlocuieste elemente in functie de tipul de date primit
	unsigned int mask_del;
	if (dim == 4)//inlocuire pe tipul int
		return v;
	if (dim == 1) {//inlocuire pe tipul char
		mask_del = 0xff000000;
		mask_del = ~(mask_del >> (3 - rest) * 8);
		x = x & mask_del;
		x = x | (v << rest * 8);
	}
	if (dim == 2) {//inlocuire pe tipul short
		mask_del = 0xffff0000;
		mask_del = ~(mask_del >> (1 - rest) * 16);
		x = x & mask_del;
		x = x | (v << rest * 16);
	}
	return x;
}

void modify(int dim, int l, int c, int val, int **a, int *m)
{////functie care modifica un element cu o noua valoare primita
	int rest = 0;
	if ((dim * c) > (m[l] * 4)) {
		a[l] = (int *)realloc(a[l], (dim * (c - 1) / 4 + 1) * sizeof(int));
		for (int i = m[l]; i < dim * (c - 1) / 4 + 1; i++)
			a[l][i] = 0;
		m[l] = dim * (c - 1) / 4 + 1;
	}
	int poz = dim * (c - 1) / 4;//calculul pozitiei unui element in matrice
	if (dim == 1)
		rest = (c - 1) % 4;
	if (dim == 2)
		rest = (c - 1) % 2;
	a[l][poz] = replace(a[l][poz], val, rest, dim);
}

void delete(int dim, int l, int c, int **a)
{///functie de stergere(zeroizare) a unui element
	int poz = dim * (c - 1) / 4;
	int rest = 0;
	if (dim == 1)
		rest = (c - 1) % 4;
	if (dim == 2)
		rest = (c - 1) % 2;
	a[l][poz] = replace(a[l][poz], 0, rest, dim);
}

void swap(int dim, int l, int c, int **a)
{////functie de schimbare a ordinii byte-ilor intr-un numar
	int poz = dim * c / 4;
	int rest = 0;
	int x_temp;
	if (dim == 1)
		rest = c % 4;
	if (dim == 2)
		rest = c % 2;
	unsigned int mask_del;
	int x = a[l][poz];
	if (dim == 4)
		mask_del = 0xffffffff;
	if (dim == 1)
		return;
	if (dim == 2) {
		mask_del = 0xffff0000;
		mask_del = ~(mask_del >> (1 - rest) * 16);
		x_temp = x & mask_del;
		mask_del = ~mask_del;
	}
	x = x & mask_del;
	if (dim == 2)
		x = x >> rest * 16;
	int nr_intors = 0, uc;
	for (int i = 0; i < dim; i++) {
	    uc = x & 0x000000ff;
		nr_intors = nr_intors << 8;
		nr_intors = nr_intors | uc;
		x = x >> 8;
	}
	if (dim == 2) {
		nr_intors = nr_intors << rest * 16;
		nr_intors = nr_intors | x_temp;
	}
	a[l][poz] = replace(a[l][poz], nr_intors, rest, 4);
}

int verify(int **a, int *m, int pos, int rest, int l, int n, bool **a_viz)
{///functie recursiva care determina dimensiunea maxima a unei zone nule
	int dim = 1;
	unsigned int mask_del;
	int x, y;
	int rest_temp = rest + 1, pos_temp = pos;
	if (rest_temp == 4) {
		pos_temp++;
		rest_temp = 0;
	}
	if (m[l] > pos_temp) {//verificare element nul in dreapta
		x = a[l][pos_temp];
		mask_del = 0xff000000;
		mask_del = mask_del >> ((3 - rest_temp) * 8);
		x = x & mask_del;
		if (x == 0 && a_viz[l][pos_temp * 4 + rest_temp] == 0) {
			a_viz[l][pos_temp * 4 + rest_temp] = true;
			dim = dim + verify(a, m, pos_temp, rest_temp, l, n, a_viz);
		}
	}
	if ((l + 1 < n) && m[l + 1] > pos) {//verificare element nul in jos
		y = a[l + 1][pos];
		mask_del = 0xff000000;
		mask_del = mask_del >> ((3 - rest) * 8);
		y = y & mask_del;
		if (y == 0 && a_viz[l + 1][pos * 4 + rest] == 0) {
			a_viz[l + 1][pos * 4 + rest] = true;
			dim = dim + verify(a, m, pos, rest, l + 1, n, a_viz);
		}
	}
	if ((l - 1 > -1) && m[l - 1] > pos) {//verificare element nul in sus
		y = a[l - 1][pos];
		mask_del = 0xff000000;
		mask_del = mask_del >> ((3 - rest) * 8);
		y = y & mask_del;
		if (y == 0 && a_viz[l - 1][pos * 4 + rest] == 0) {
			a_viz[l - 1][pos * 4 + rest] = true;
			dim = dim + verify(a, m, pos, rest, l - 1, n, a_viz);
		}
	}
	rest_temp = rest - 1;
	pos_temp = pos;
	if (rest_temp == -1) {
		pos_temp--;
		rest_temp = 3;
	}
	if (pos_temp >= 0) {//verificare element nul in stanga
		x = a[l][pos_temp];
		mask_del = 0xff000000;
		mask_del = mask_del >> ((3 - rest_temp) * 8);
		x = x & mask_del;
		if (x == 0 && a_viz[l][pos_temp * 4 + rest_temp] == 0) {
			a_viz[l][pos_temp * 4 + rest_temp] = true;
			dim = dim + verify(a, m, pos_temp, rest_temp, l, n, a_viz);
		}
	}
	return dim;
}

void task_tree(int **a, int *m, int n, bool **a_viz)
{
	int dim_max = 0, i_max, j_max;
	for (int i = 0; i < n; i++)//parcurgere matrice pe bytes
		for (int pos = 0; pos < m[i]; pos++)
			for (int rest = 0; rest < 4; rest++) {
				int c;
				unsigned int mask = 0xff000000;
				mask = mask >> ((3 - rest) * 8);
				c = a[i][pos] & mask;
				if (c == 0 && a_viz[i][pos * 4 + rest] == 0) {
					a_viz[i][pos * 4 + rest] = true;
					int dim = verify(a, m, pos, rest, i, n, a_viz);
					if (dim > dim_max) {//gasire dimensiune maxima
						i_max = i;
						j_max = pos * 4 + rest;
						dim_max = dim;
					}
				}
			}
	printf("task 3\n");
	printf("%d %d %d\n", i_max, j_max, dim_max);
}

int main(void)
{
	int n, **a, *m, nr_reguli, l, c, val;
	scanf("%d", &n);
	a = malloc(n * sizeof(int *));
	if (!a)
		return -1;
	m = malloc(n * sizeof(int));
	if (!m)
		return -1;
	for (int i = 0; i < n; i++) {
		scanf("%d", &m[i]);
		a[i] = malloc(m[i] * sizeof(int));
		for (int j = 0; j < m[i]; j++)
			scanf("%x", &a[i][j]);
	}
	if (!a)
		return -1;
	task_one(a, m, n);
	char rule, type;
	scanf("%d", &nr_reguli);
	while (nr_reguli > 0) {//citire reguli de modificare a matricei
		scanf(" %c", &rule);
		if (rule == 'M') {
			scanf(" %c %d %d %x", &type, &l, &c, &val);
			if (type == 'I')
				modify(4, l, c, val, a, m);
			else if (type == 'S')
				modify(2, l, c, val, a, m);
			else
				modify(1, l, c, val, a, m);
		} else if (rule == 'S') {
			scanf(" %c %d %d", &type, &l, &c);
			if (type == 'I')
				swap(4, l, c, a);
			else if (type == 'S')
				swap(2, l, c, a);
			else
				swap(1, l, c, a);
			} else if (rule == 'D') {
				scanf(" %c %d %d", &type, &l, &c);
				if (type == 'I')
					delete(4, l, c, a);
				else if (type == 'S')
					delete(2, l, c, a);
				else
					delete(1, l, c, a);
				}
		nr_reguli--;
	}
	task_two(a, m, n);
	bool **a_viz;
	a_viz = (bool **)malloc(n * sizeof(bool *));
	if (!a_viz)
		exit(-1);
	for (int i = 0; i < n; i++)
		a_viz[i] = (bool *)calloc(4 * m[i], sizeof(bool));
	if (!a_viz)
		exit(-1);
	task_tree(a, m, n, a_viz);
	free(m);
	free_mat1(n, a);
	free_mat2(n, a_viz);
	return 0;
}
