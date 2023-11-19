/**
 * Group ID - 
 * Member 1 Name - 
 * Member 1 BITS ID - 
 * Member 2 Name - 
 * Member 2 BITS ID - 
 * Member 3 Name - 
 * Member 3 BITS ID - 
*/

// ---------------------------DO NOT MODIFY (Begin)------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

typedef struct Graph {
    int n; // Size of the graph
    int** adj; // Adjacency matrix
    char** station_names; // Array of station names
} Graph;

/**
 * This function has been pre-filled. It reads the input testcase 
 * and creates the Graph structure from it.
*/
Graph* create_graph(char input_file_path[]) {
    FILE* f = fopen(input_file_path, "r");
    Graph* g = (Graph*)malloc(sizeof(Graph));
    // Size of graph
    fscanf(f, "%d", &(g->n));
    // Names of stations
    g->station_names = (char**)malloc(g->n * sizeof(char*));
    for(int i = 0; i < g->n; i++) {
        g->station_names[i] = (char*)malloc(100 * sizeof(char));
        fscanf(f, "%s", g->station_names[i]);
    }
    // Adjacency matrix
    g->adj = (int**)malloc(g->n * sizeof(int*));
    for(int i = 0; i < g->n; i++) {
        g->adj[i] = calloc(g->n, sizeof(int));
    }
    int edges;
    fscanf(f, "%d", &edges);
    for(int i = 0; i < edges; i++) {
        int x, y;
        fscanf(f, "%d%d", &x, &y);
        g->adj[x][y] = 1;
        g->adj[y][x] = 1;
    }
    fclose(f);
    return g;
}
// ---------------------------DO NOT MODIFY (End)------------------------------

/**
 * Q.1 
 * Return the number of junctions.
*/
int find_junctions(Graph* g) {
    int n_nodes = g->n;
    int junctions = 0;

    for (int i = 0; i < n_nodes; i++) {
        int degree = 0;
        for (int j = 0; j < n_nodes; j++) {
            if (g->adj[i][j] == 1) {
                degree++;
            }
        }
        if (degree > 3) {
            junctions++;
        }
    }

    return junctions;
}

/**
 * Q.2
 * Return true if the tour specified in the question is possible for this 
 * graph, else return false.
 * Parameter SAME_STATION - If it is true, then question 2(a) must be solved. 
 * If false, then question 2(b) must be solved.
*/
bool sheldons_tour(Graph* g, bool SAME_STATION) {
    int n_nodes = g->n;

    if (SAME_STATION) {
        for (int i = 0; i < n_nodes; i++) {
            int degree = 0;
            for (int j = 0; j < n_nodes; j++) {
                if (g->adj[i][j] == 1) {
                    degree++;
                }
            }
            if (degree % 2 == 1) {
                return false;
            }
        }
    } else {
        int odd_degree = 0;
        for (int i = 0; i < n_nodes; i++) {
            int degree = 0;
            for (int j = 0; j < n_nodes; j++) {
                if (g->adj[i][j] == 1) {
                    degree++;
                }
            }
            if (degree % 2 == 1) {
                odd_degree++;
            }
        }
        if (odd_degree != 2) {
            return false;
        }
    }

    return true;
}

/**
 * Q.3
 * Return closure, an n * n matrix filled with 0s and 1s as required. 
*/
int** warshall(Graph* g) {
    // Do not modify or delete pre-filled code!
    int** closure = (int**)malloc(g->n * sizeof(int*));
    for(int i = 0; i < g->n; i++) {
        closure[i] = calloc(g->n, sizeof(int));
    }

    // initialising closure matrix to adjacency matrix
    int n_nodes = g->n;
    for(int i = 0; i < n_nodes; i++) {
        for(int j = 0; j < n_nodes; j++) {
            closure[i][j] = g->adj[i][j];
        }
    }

    for (int k = 0; k < n_nodes; k++) {
        for (int i = 0; i < n_nodes; i++) {
            for (int j = 0; j < n_nodes; j++) {
                if (closure[i][k] == 1 && closure[k][j] == 1) {
                    closure[i][j] = 1;
                }
            }
        }
    }

    return closure; // Do not modify
}

/**
 * Q.3
 * Return the number of impossible pairs.
*/
int find_impossible_pairs(Graph* g) {
    int** closure = warshall(g); // Do not modify
    
    int imp_count = 0;

    for (int i = 0; i < g->n; i++) {
        for (int j = i + 1; j < g->n; j++) {
            if (closure[i][j] == 0) {
                imp_count++;
            }
        }
    }

    return imp_count;
}

/**
 * Q.4
 * Return the number of vital train tracks.
*/
int find_vital_train_tracks(Graph* g) {
    int n = g->n;

    int** closure = warshall(g);
    int vital_count = 0;
    
    // check if removing an edge makes the graph disconnected
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (g->adj[i][j] == 1) {
                g->adj[i][j] = 0;
                g->adj[j][i] = 0;

                int** closure_new = warshall(g);

                if (closure_new[i][j] == 0) {
                    vital_count++;
                }

                g->adj[i][j] = 1;
                g->adj[j][i] = 1;
            }
        }
    }
    return vital_count;
}

/**
 * Q.5
 * Return upgrades, an array of size n.
 * upgrades[i] should be 0 if railway station i gets a restaurant
 * upgrades[i] should be 1 if railway station i gets a maintenance depot
 * If it is not possible to upgrade, then return "-1" in the entire array
*/
int* upgrade_railway_stations(Graph* g) {
    int* upgrades = calloc(g->n, sizeof(int)); // Do not modify

    int** adj = g->adj; // Adjacency matrix
    int n = g->n; // Number of stations

    // using DFS to check if bipartite
    for (int i = 0; i < n; i++)
        upgrades[i] = -1;

    for (int i = 0; i < n; i++) {
        if (upgrades[i] == -1) {
            upgrades[i] = 1;

            int stack[n];
            int top = -1;
            stack[++top] = i;

            while (top >= 0) {
                int v = stack[top--];

                for (int u = 0; u < n; u++) {
                    if (adj[v][u] == 1 && upgrades[u] == -1) {
                        upgrades[u] = 1 - upgrades[v];
                        stack[++top] = u;
                    } else if (adj[v][u] && upgrades[u] == upgrades[v]) {
                        // If conflict found, set upgrades to -1 and return
                        for (int j = 0; j < n; j++) {
                            upgrades[j] = -1;
                        }
                        return upgrades;
                    }
                }
            }
        }
    }

    return upgrades; // Do not modify
}

/**
 * Q.6
 * Return distance between X and Y
 * city_x is the index of X, city_y is the index of Y
*/
int distance(Graph* g, int city_x, int city_y) {
    int** dist = (int**)malloc(g->n * sizeof(int*));

    int n_nodes = g->n;
    int INF = 1000000;

    for (int i = 0; i < n_nodes; i++) {
        dist[i] = (int*)malloc(g->n * sizeof(int));
        for (int j = 0; j < n_nodes; j++) {
            if (i == j) {
                dist[i][j] = 0;
            } else {
                if (g->adj[i][j] == 0) {
                    dist[i][j] = INF;
                } else {
                    dist[i][j] = 1;
                }
            }
        }
    }

    for (int k = 0; k < n_nodes; k++) {
        for (int i = 0; i < n_nodes; i++) {
            for (int j = 0; j < n_nodes; j++) {
                if (dist[i][k] + dist[k][j] < dist[i][j])
                    dist[i][j] = dist[i][k] + dist[k][j];
            }
        }
    }

    if (dist[city_x][city_y] != INF) {
        return dist[city_x][city_y];
    } else {
        return -1;
    }
}

/**
 * Q.7
 * Return the index of any one possible railway capital in the network
*/
int railway_capital(Graph* g) {
    int** dist = (int**)malloc(g->n * sizeof(int*));

    int n_nodes = g->n;
    int INF = 1000000;

    for (int i = 0; i < n_nodes; i++) {
        dist[i] = (int*)malloc(g->n * sizeof(int));
        for (int j = 0; j < n_nodes; j++) {
            if (i == j) {
                dist[i][j] = 0;
            } else {
                if (g->adj[i][j] == 0) {
                    dist[i][j] = INF;
                } else {
                    dist[i][j] = 1;
                }
            }
        }
    }

    for (int k = 0; k < n_nodes; k++) {
        for (int i = 0; i < n_nodes; i++) {
            for (int j = 0; j < n_nodes; j++) {
                if (dist[i][k] + dist[k][j] < dist[i][j])
                    dist[i][j] = dist[i][k] + dist[k][j];
            }
        }
    }

    int min = 100000;
    int min_elem = 0;
    for (int i = 0; i < n_nodes; i++) {
        int dist_sum = 0;
        for (int j = 0; j < n_nodes; j++) {
            dist_sum += dist[i][j];
        }
        if (dist_sum < min) {
            min = dist_sum;
            min_elem = i;
        }
    }

    return min_elem;
}

/**
 * Helper function for Q.8
*/
bool maharaja_express_tour(Graph* g, int source, int current_city, int previous_city, int* visited) {
    visited[current_city] = 1;
    
    // Iterate through adjacent cities
    for (int i = 0; i < g->n; ++i) {
        if (g->adj[current_city][i]) {
            // If the adjacent city is the source city it's a valid tour
            if (i == source && previous_city != source) {
                return true;
            }
            
            // If the adjacent city is not the previous city and not visited, explore further
            if (i != previous_city && visited[i] == 0) {
                if (maharaja_express_tour(g, source, i, current_city, visited)) {
                    return true;
                }
            }
        }
    }
    
    // Unmark the current city before returning
    visited[current_city] = 0;
    return false;
}

/**
 * Q.8 
 * Return true if it is possible to have a Maharaja Express tour 
 * starting from source city, else return false. 
*/
bool maharaja_express(Graph* g, int source) {
    int* visited = (int*)malloc(g->n * sizeof(int)); 
    for(int i = 0; i < g->n; i++) {
        visited[i] = 0;
    }

    int n_nodes = g->n;

    int** closure = warshall(g);
    
    // if source is part of a cycle, then it is possible. to check for cycle we look at vital edges
    for (int i = 0; i < n_nodes; i++) {
        if (g->adj[source][i] == 1) {
            g->adj[source][i] = 0;
            g->adj[i][source] = 0;

            bool vital = false;

            int** closure_new = warshall(g);

            if (closure_new[source][i] == 0) {
                vital = true;
            }

            g->adj[source][i] = 1;
            g->adj[i][source] = 1;

            if (!vital) {
                return true;
            }
        }
    }

    return false;
}

int main() {
    char input_file_path[100] = "testcase_3.txt"; // Can be modified
    Graph* g = create_graph(input_file_path); // Do not modify
    
    // Code goes here
    printf("The number of junctions: %d\n", find_junctions(g));
    printf("Sheldon's tour (same city) is possible: %d\n", sheldons_tour(g, true));
    printf("Sheldon's tour (diff city) is possible: %d\n", sheldons_tour(g, false));
    printf("Number of impossible pairs: %d\n", find_impossible_pairs(g));
    printf("Number of vital train tracks: %d\n", find_vital_train_tracks(g));
    int* upgrades = upgrade_railway_stations(g);
    printf("Upgrades: ");
    for(int i = 0; i < g->n; i++) {
        printf("%d ", upgrades[i]);
    }
    printf("\n");
    printf("Distance between 4 and 4: %d\n", distance(g, 4-1, 4-1));
    printf("Distance between 4 and 6: %d\n", distance(g, 4-1, 6-1));
    printf("Distance between 1 and 5: %d\n", distance(g, 1-1, 5-1));
    printf("Distance between 2 and 5: %d\n", distance(g, 2-1, 5-1));

    printf("Railway capital: %d\n", railway_capital(g));

    printf("Maharaja Express tour (1): %d\n", maharaja_express(g, 1-1));
    printf("Maharaja Express tour (2): %d\n", maharaja_express(g, 2-1));
    printf("Maharaja Express tour (3): %d\n", maharaja_express(g, 3-1));
    printf("Maharaja Express tour (4): %d\n", maharaja_express(g, 4-1));
    printf("Maharaja Express tour (5): %d\n", maharaja_express(g, 5-1));
    printf("Maharaja Express tour (6): %d\n", maharaja_express(g, 6-1));
    printf("Maharaja Express tour (7): %d\n", maharaja_express(g, 7-1));
    printf("Maharaja Express tour (8): %d\n", maharaja_express(g, 8-1));

    return 0;
}