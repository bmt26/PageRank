# Page-Rank

Compilation Instructions
Compiler Version: GNU C17 (GCC) version 11.5.0 20240719 (Red Hat 11.5.0-5) (x86_64-redhat-linux)

## How To Run

### Placeholders (Do not include in-equality symbols <>)

- <exe_filename>	| Desired exectuable filename
- <iterations>		| Positive integer -> number of iterations, 0 -> errorate of $(10**-5),
--			|	Negative integer -> errorate of $(10**<iterations>)
- <initial_value>	| Initial Vector Values (0 -> 0, 1 -> 1, -1 -> $(1/N), -2 $(1/(N**0.5)) 
- <graph_filename>	| Prexisiting filename containing a directed graph represented through
			|	an adjacency list representation

### pgrk.c | PageRank

#### Compilation
	> gcc pgrk.c -o <exe_filename> -lm

#### Execution
	> ./<exe_filename> <iterations> <initial_value> <graph_filename>

## Notes

- -lm option used in pgrk.c used to access math.h library, used in several computations
