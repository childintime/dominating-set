#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stack>
#include <vector>
#include <set>

// struktura grafu
typedef struct Graph {
    // pocet vrcholu
    int n;
    
    // matice incidenci
    int** graph;

    // pole vah
    int* weights;
} Graph;

// struktura dominantni mnoziny, offset je pozice v seznamu uzlu, weight celkova vaha mnoziny
typedef struct Dominating_set {
	int offset;
	std::set<int> set;
	int weight;
} Dominating_set;
 
// tisk grafu
void print_graph();

// test jestli mnozina pokryva graf
bool graph_covered(Dominating_set dominating_set);


// zasobnik mnozin k propocitani
std::vector<Dominating_set> set_stack;

// graf ze zadani
Graph graph;
// reseni
Dominating_set solution;

// hlavni funkce, ktera hleda reseni
void dominating_set();

// vypsani mnoziny
void print_ds(Dominating_set dominating_set);


int main(int argc, char *argv[])
{
	if (argc < 3) {
        printf("usage: %s file_with_graph file_with_vert_weights\n", argv[0]);
        return 1;
    }
    
    FILE *graphFile;
    graphFile = fopen(argv[1], "r");

    FILE *weightFile;
    weightFile = fopen(argv[2], "r");

    if (graphFile == NULL)
    {
        printf("Error reading graph file!\n");
        return 1;
    }

    if (weightFile == NULL)
    {
        printf("Error reading weight file!\n");
        return 1;
    }

    char buffer[1024];
    
    fgets(buffer, sizeof(buffer), graphFile);
    int n = 0;
    n = atoi(buffer);

    // graf ze zadani a jeho nacteni ze souboru --------------
    //Graph graph;
    graph.n = n;
    graph.graph = (int**) malloc(n*sizeof(int*));
    graph.weights = (int*) malloc(n*sizeof(int*));

    // budu cist n radek
    for (int i = 0; i < n; i++) {
        
        fgets(buffer, sizeof(buffer), graphFile);

        graph.graph[i] = (int*) malloc(n*sizeof(int));

        // na kazdy bude n cisel
        for (int v = 0; v < n; v++)
        {
            // postupne nacte radek do matice grafu, po znaku, - '0' to srovna.
            graph.graph[i][v] = buffer[v] - '0';
        }
		

		fgets(buffer, sizeof(buffer), weightFile);
		int weight = 0;
        sscanf(buffer, "%d", &weight);
		graph.weights[i] = weight;
    }

    fclose(graphFile);
    fclose(weightFile);
    // ---------------------------------


    // incializace reseni (vsechny uzly)
    solution.offset = 0;
    std::set<int> solution_set;
    for(int i = 0; i < graph.n; i++) {
    	solution_set.insert(i);
    }
    solution.set = solution_set;
    
    
    // vychozi prazdna mnozina
    Dominating_set ds;
    int offset = 0;
    std::set<int> set;
    ds.offset = 0;
    ds.set = set;
    ds.weight = 0;
    
    // jeji pridani na zasobnik 
    set_stack.push_back(ds);
    
    // vypocet minimalni dominantni mnoziny
    dominating_set();

    // reseni
    printf("reseni: \n");
    print_ds(solution);
    
    // tisk grafu
    //print_graph();
}

void dominating_set() {
	// vypocet probiha dokud je neco v zasobniku
	while(set_stack.size() > 0) {
		
		// nacteni vrcholu
		Dominating_set ds = set_stack.back();
		set_stack.pop_back();
		
		// pokud mnozina pokryva cely graf, mame reseni
		if(graph_covered(ds)) {
			//printf("mam reseni, velikost %lu, vaha %d", ds.set.size(), ds.weight);
			
			// pokud je velikost aspon takova jako je nejlepsi pokracuji
			if(ds.set.size() <= solution.set.size()) {
				// pokud je mensi nez zatim nejlepsi mame nove nejlepsi reseni
				if(ds.set.size() < solution.set.size()) {
					//printf(", je nejlepsi\n");
					solution = ds;	
				}
				else {
					// jinak je mnozina stejne velka a reseni prepisu jenom v pripade, ze ma lepsi vahu
					if(ds.weight > solution.weight) {
						// printf(", je nejlepsi\n");
						solution = ds;			
					}
					else {
						// printf(", neni nejlepsi\n");		
					}
				}
				
			}
			else {
				// printf(", neni nejlepsi\n");
			}
		}
		// mnozina nepokryva cely graf
		else {
			// mnozina muze pokryvat jenom pokud pridam hranu, nema cenu hledat reseni, ktere bude mit po pridani vice hran nez soucasne nejlepsi reseni
			if(ds.set.size() < solution.set.size()) {
				//printf("muzu pokracovat\n");
				
				// do zasobniku postupne pridam danou mnozinu rozsirenou o dalsi mozne vrcholy.
				// pomoci offsetu vim, ze tam nebudou kombinace co prozkoumam uz jinde
				for(int i = ds.offset; i < graph.n; i++) {
					Dominating_set new_ds = ds;
					new_ds.offset = i+1;
					new_ds.set.insert(i);
					new_ds.weight = new_ds.weight + graph.weights[i];
					set_stack.push_back(new_ds);
					
					//printf("davam na zasobnik:\n");
					//print_ds(new_ds);
				}
			}
			else {
				//printf("nema cenu pokracovat v:\n");
				//print_ds(ds);			
			}
		}
	}
	//printf("konec zasobniku\n");

}


bool graph_covered(Dominating_set dominating_set) {
	int set_size = dominating_set.set.size();
	std::set<int> covered;
	std::set<int>::iterator it;
	
	// iterace pres mnozinu
	for (it = dominating_set.set.begin(); it != dominating_set.set.end(); ++it) {
		int vert = *it;
		// pridam vrchol z mnoziny do mnoziny pokrytych
		covered.insert(vert);
		// a take pridam vsechny vrcholy kam z nej vede hrana
		for(int i = 0; i < graph.n; i++) {
			if(graph.graph[vert][i] == 1) {
				covered.insert(i);
			}
		}
	}
	
	int covered_size = covered.size();
	//printf("velikost pokryty: %d\n", covered_size);
	
	// pokud je velikost pokryte mnoziny stejna jako pocet vrcholu, tak mnozina pokryva graf
	if (covered_size == graph.n) {
		return true;
	}
	return false;
}

void print_ds(Dominating_set dominating_set) {
	printf("----- dominating set ------\n");
	printf("velikost %lu\n", dominating_set.set.size());
	printf("vaha %d\n", dominating_set.weight
	);
	printf("offset: %d\n", dominating_set.offset);
	printf("vrcholy: ");
	std::set<int>::iterator it;
	for (it = dominating_set.set.begin(); it != dominating_set.set.end(); ++it) {
		int vert = *it;
		printf("%d ", vert);
	}
	printf("\n-------------\n");
}

void print_graph() {
    for (int i = 0; i < graph.n; i++) {
        for (int v = 0; v < graph.n; v++) {
            printf("%d", graph.graph[i][v]);
        }
        printf("\n");
    }
    printf("vahy: \n");
    for (int i = 0; i < graph.n; i++) {
    	printf("%d ", graph.weights[i]);
    }
     printf("\n");

}
