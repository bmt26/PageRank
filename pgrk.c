#include <errno.h>
#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const double D = 0.85;

typedef struct nod {
	int vertex;
	struct nod *next_node;
} node;

typedef struct {
	int vertices;
	node **adj_lists;
	int *indeg;
	int *outdeg;
} adj_list_rep;

int string_to_int(const char *str, int *result) {
	char *conversion_end_ptr;
	int i;
	int num_chars;
	long num;

	// Try to convert string to long
	num = strtol(str, &conversion_end_ptr, 10);

	// ERROR CHECKING

	// Out of Range for Long
	if (errno == ERANGE) {
		fprintf(stderr, "Error: %s value out of range for int [%d, %d].\n", str, INT_MIN, INT_MAX);
		return -1;
	}

	// Invalid string, not a number
	if (*conversion_end_ptr != '\0') {
		fprintf(stderr, "Error: %s contains non-numeric characters.\n", str);
		return -1;
	}

	// Out of Range for Int
	if (num < INT_MIN || INT_MAX < num) {
		fprintf(stderr, "Error: %s value out of range for int [%d, %d].\n", str, INT_MIN, INT_MAX);
		return -1;
	}

	// END ERROR CHECKING

	// Convert number to int and return 0 success
	*result = (int)num;
	return 0;
}

void print_alr(adj_list_rep *alr) {
	if (alr->vertices < 1) { return; }
	int num_chars = (int)(log10(alr->vertices)) + 1;
	for (int i = 0; i < alr->vertices; i++) {
		printf("%*d | ", num_chars, alr->outdeg[i]);
		printf("%*d | || ", num_chars, alr->indeg[i]);
		printf("%*d", num_chars, i);
		if (alr->adj_lists[i] != NULL) {
			node *w_node = alr->adj_lists[i];
			printf(" | %*d", num_chars, w_node->vertex);
			while (w_node->next_node != NULL) {
				w_node = w_node->next_node;
				printf(" | %*d", num_chars, w_node->vertex);
			}
		}
		printf(" | %*c |\n", num_chars, '/');

	}
}

void insert_alr(adj_list_rep *return_alr, int v_i, int v_j) {
	if (return_alr->adj_lists[v_i] == NULL) {
		return_alr->adj_lists[v_i] = (node *)malloc(sizeof(node));
		return_alr->adj_lists[v_i]->vertex = v_j;
		return_alr->adj_lists[v_i]->next_node = NULL;

	} else {
		node *w_node = return_alr->adj_lists[v_i];
		while (w_node->next_node != NULL) {
			w_node = w_node->next_node;
		}
		w_node->next_node = (node *)malloc(sizeof(node));
		w_node->next_node->vertex = v_j;
		w_node->next_node->next_node = NULL;
	}

	// Increment in and out degree
	return_alr->outdeg[v_i]++;
	return_alr->indeg[v_j]++;
}

int create_alr(FILE *gf_ptr, adj_list_rep **return_alr) {
	int n, big_n;
	int v_i, v_j;

	//// Free return alr if already assigned
	//if (return_alr != NULL) { free(return_alr); }

	// Parse file for first line, number of vertices and edges
	if (fscanf(gf_ptr, "%d %d", &n, &big_n) != 2) {
		fprintf(stderr, "Error: Graph File parse error, on line 1. Line format: int int\n");
		return -2;
	}

	// Malloc 
	*return_alr = (adj_list_rep *)malloc(sizeof(adj_list_rep));
	if (return_alr == NULL) {
		fprintf(stderr, "Error: Memory allocation failed.\n");
		return -2;
	}
	(*return_alr)->outdeg = malloc(n * sizeof(int));
	(*return_alr)->indeg = malloc(n * sizeof(int));	
	(*return_alr)->adj_lists = malloc(n * sizeof(*((*return_alr)->adj_lists)));
	if (	(*return_alr)->adj_lists == NULL ||
		(*return_alr)->outdeg == NULL ||
		(*return_alr)->indeg == NULL ) {
		fprintf(stderr, "Error: Memory allocation failed.\n");
		return -2;
	}
	(*return_alr)->vertices = n;
	memset((*return_alr)->outdeg, 0, n * sizeof(int));
	memset((*return_alr)->indeg, 0, n * sizeof(int));

	// Parse edges
	for (int i = 0; i < big_n; i++) {
		// Parse file for edges v_i->v_j
		if (fscanf(gf_ptr, "%d %d", &v_i, &v_j) != 2) {
			fprintf(stderr, "Error: Graph File parse error, on line %d. Line format: int int\n", i+1);
			return -1;
		}
		insert_alr(*return_alr, v_i, v_j);
	}

	return 0;
}

//
void pgrk(adj_list_rep *alr, int iterations, int initial_value) {
	double *pr_last;
	double *pr_curr;
	double *temp;
	double pr_over_outdeg;
	double rand_visit_prob = (1-D)/alr->vertices;
	node *w_node;
	bool continue_iterating = true;
	int i = 0;
	double init_value;
	bool do_errorate = false;
	bool stop_errorate;
	double errorate;
	double errorate_sum;
	if (alr->vertices < 1) { return; }
	int num_chars = (int)(log10(alr->vertices+1)) + 1;

	// Malloc pr arrays
	pr_last = malloc(alr->vertices * sizeof(double));
	pr_curr = malloc(alr->vertices * sizeof(double));
	if (pr_last == NULL || pr_curr == NULL) {
		fprintf(stderr, "Error: Memory allocation failed.\n");
		return;
	}

	// Calculate initial values
	if (-1 == initial_value)	{ init_value = (double)1 / alr->vertices;}
	else if (-2 == initial_value)	{ init_value = (double)1 / sqrt(alr->vertices); }
	else				{ init_value = (double) initial_value; }

	// Print and assign initial values
	if(alr->vertices<11) { printf("Base    :%*d :", 3, 0); }
	for (int j = 0; j < alr->vertices; j++) {
		pr_curr[j] = init_value;
		pr_last[j] = (double)0;
		if(alr->vertices<11) {
			printf(" P[%*d] = %.7lf", num_chars, j, pr_curr[j]);
		}
	}
	if (alr->vertices<11) { printf("\n"); }

	if (10<alr->vertices) {
		do_errorate=true;
		errorate = pow(10.0, -5.0);
	} else if (iterations < 1) {
		do_errorate=true;
		errorate = pow(10.0, (double)iterations);
	}

	while (continue_iterating) {
		stop_errorate = true;

		// Swap pr's and reset curr
		temp = pr_last;
		pr_last = pr_curr;
		pr_curr = temp;
		memset(pr_curr, 0, alr->vertices * sizeof(double));

		for (int j = 0; j < alr->vertices; j++) {

			if (alr->outdeg[j] == 0) { pr_over_outdeg = (float)0; }
			else { pr_over_outdeg = pr_last[j]/alr->outdeg[j]; }

			// Iterate through ADJ List i;
			if (alr->adj_lists[j] != NULL) {
				w_node = alr->adj_lists[j];
				pr_curr[w_node->vertex] += pr_over_outdeg;
				while (w_node->next_node != NULL) {
					w_node = w_node->next_node;
					pr_curr[w_node->vertex] += pr_over_outdeg;
				}
			}
		}
		if(alr->vertices<11) {
			printf("Iter    :%*d :", 3, i+1);
		}
		// Calculate remainder of pr function (1-d)/n
		for (int j = 0; j < alr->vertices; j++) {
			pr_curr[j] = D * pr_curr[j] + rand_visit_prob;
			stop_errorate = stop_errorate &&
					(fabs(pr_curr[j]-pr_last[j]) <= errorate);
			if(alr->vertices<11) {
				printf(" P[%*d] = %.7lf", num_chars, j, pr_curr[j]);
			}
		}
		if(alr->vertices<11) {
			printf("\n");
		}

		// Print & Evaluate
		i++;
		if (	(!do_errorate && !(0 < iterations && i < iterations)) ||
			(do_errorate && stop_errorate)) {
			break;
		}
	}

	if (10 < alr->vertices) {
		printf("Iter    :%*d\n", 3, i+1);
		for (int j = 0; j < alr->vertices; j++) {
			printf("P[%*d] = %.7lf\n", num_chars, j, pr_curr[j]);
		}
	}
}

int main (int argc, char *argv[]) {
	int iterations;
	int initial_value;
	char *graph_filename;
	FILE *gf_ptr; // Graph File Pointer

	// Adjacency List Representation Variables
	adj_list_rep *alr;

	// Page Rank Variables
	float *pr;

	// Insufficient arguments, Error Message and return -1
        if (argc < 4) {
                fprintf(stderr, "Error: Usage: %s <iterations> <initial_value> <graph_filename>\n", argv[0] );
                return -1;
        }

	// Process integer input
	if ((string_to_int(argv[1], &iterations)) < 0 || (string_to_int(argv[2], &initial_value) < 0)) {
		return -1;
	}

	// Process filename input
	graph_filename = argv[3];
	gf_ptr = fopen(graph_filename, "r");
	if (gf_ptr == NULL) {
		fprintf(stderr, "Error: Could not open the file %s.\n", graph_filename);
		return -1;
	}

	int code = create_alr(gf_ptr, &alr);

	//print_alr(alr);

	pgrk(alr, iterations, initial_value);

	fclose(gf_ptr);
        return 0;
}
