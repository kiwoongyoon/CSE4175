#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <queue>
#include <vector>
#include <errno.h>
#define inf int(1e9)

using namespace std;
vector<pair<int, int> > edge[101];
int nodecnt;

int distances[101][101] ; 
int pre[101][101]; 
int start; 
int end ; 
char message[1000] ; 
char *oneline ;
int new_start, new_end, new_cost ;  
void init() ;
void print_message(FILE *fp, FILE *fp2) {
	int st, ed, nextone;
	while (!feof(fp2)) {
		vector<int> temproute;

		// Read source and destination from the file
		if (fscanf(fp2, "%d%d", &st, &ed) == 2) {
			oneline = fgets(message, 1000, fp2);

			// Check if there is a valid path from source to destination
			if (distances[st][ed] != inf) {
				fprintf(fp, "from %d to %d cost %d hops ", st, ed, distances[st][ed]);
				nextone = st;

				// Traverse the path from source to destination
				while (nextone != ed) {
					temproute.push_back(nextone);
					nextone = pre[nextone][ed];
				}

				// Print the intermediate hops in the path
				for (int i = 0; i < (int)temproute.size(); i++) {
					fprintf(fp, "%d ", temproute[i]);
				}

				fprintf(fp, "message");
				fprintf(fp, "%s", oneline);
			} else {
				fprintf(fp, "from %d to %d cost infinite hops unreachable message", st, ed);
				fprintf(fp, "%s", oneline);
			}
		}
	}

	fprintf(fp, "\n");
}

void dijkstra(FILE *fp) {
    init(); // Initialize data structures
    
    for (int i = 0; i < nodecnt; i++) {
        int flag = 0;
        priority_queue<pair<int, int> > pq;
        pq.push(make_pair(0, -i)); // Start with source node i
        
        while (!pq.empty()) {
            int wei = -pq.top().first; // Current weight
            int now = -pq.top().second; // Current node
            pq.pop();
            
            if (wei > distances[i][now]) {
                continue; // Skip if a shorter path already exists
            }
            
            for (int j = 0; j < (int)edge[now].size(); j++) {
                int nextnode = edge[now][j].first; // Next node
                int cost = edge[now][j].second; // Cost of the edge
                
                // Relax the edge if a shorter path is found
                if (distances[i][nextnode] > wei + cost) {
                    distances[i][nextnode] = wei + cost;
                    
                    if (flag == 0) {
                        pre[i][nextnode] = nextnode; // Update the predecessor only for the first iteration
                    } else {
                        pre[i][nextnode] = pre[i][now]; // Update the predecessor
                    }
                    
                    pq.push(make_pair(-distances[i][nextnode], -nextnode)); // Push the updated distance and node to the priority queue
                }
            }
            
            flag += 1;
        }
        
        // Print the calculated distances and predecessors for node i
        for (int j = 0; j < nodecnt; j++) {
            if (distances[i][j] == inf) {
                continue; // Skip if there is no valid path
            }
            
            fprintf(fp, "%d %d %d\n", j, pre[i][j], distances[i][j]);
        }
        
        fprintf(fp, "\n"); // Add a new line after printing distances for node i
    }
    
    return;
}

int main(int argc, char *argv[]) {
    FILE *fp1, *fp2, *fp3, *result;
    fp1 = fopen(argv[1], "r");
    fp2 = fopen(argv[2], "r");
    fp3 = fopen(argv[3], "r");
    result = fopen("output_ls.txt", "w");
    int startnode, endnode, dist;
    char changes[20];

    if (argc != 4) {
        fprintf(stderr, "usage: linkstate topologyfile messagesfile changesfile\n");
        exit(1);
    }

    if (fp1 == NULL || fp2 == NULL || fp3 == NULL) {
        fprintf(stderr, "Error: open input file.\n");
        return 0;
    }

    fscanf(fp1, "%d", &nodecnt);

    // Read the topology file and populate the edge vector
    while (feof(fp1) == 0) {
        fscanf(fp1, "%d %d %d ", &startnode, &endnode, &dist);
        edge[startnode].push_back(make_pair(endnode, dist));
        edge[endnode].push_back(make_pair(startnode, dist));
    }

    dijkstra(result); // Perform Dijkstra's algorithm to compute shortest paths
    print_message(result, fp2); // Print messages using the computed shortest paths
    fclose(fp2);

    // Read the changes file and update the edge weights accordingly
    while (fgets(changes, sizeof(changes), fp3) != NULL) {
        int flag1 = 0, flag2 = 0;
        fp2 = fopen(argv[2], "r");
        sscanf(changes, "%d %d %d", &startnode, &endnode, &dist);
        int real_dist;
        if (dist == -999)
            real_dist = inf;
        else
            real_dist = dist;

        // Update the edge weight in both directions (startnode to endnode and endnode to startnode)
        for (int i = 0; i < (int)edge[startnode].size(); i++) {
            if (edge[startnode][i].first == endnode) {
                edge[startnode][i].second = real_dist;
                flag1 = 1;
            }
        }

        for (int i = 0; i < (int)edge[endnode].size(); i++) {
            if (edge[endnode][i].first == startnode) {
                edge[endnode][i].second = real_dist;
                flag2 = 1;
            }
        }

        // If the edge does not exist, add it with the new weight
        if (!flag1)
            edge[startnode].push_back(make_pair(endnode, real_dist));
        if (!flag2)
            edge[endnode].push_back(make_pair(startnode, real_dist));

        dijkstra(result); // Re-compute shortest paths after the changes
        print_message(result, fp2); // Print messages using the updated shortest paths
        fclose(fp2);
    }

    printf("Complete. Output file written to output_ls.txt.\n");
    fclose(fp1);
    fclose(fp3);
    fclose(result);
    return 0;
}


void init() {
    for (int i = 0; i < nodecnt; i++) {
        for (int j = 0; j < nodecnt; j++) {
            distances[i][j] = inf; // Set initial distance between nodes i and j to infinity
            distances[j][i] = inf; // Set initial distance between nodes j and i to infinity
            pre[i][j] = -1; // Set initial predecessor for node j in the shortest path from i to j as -1 (undefined)
            
            if (i == j) {
                distances[i][j] = 0; // Set the distance from a node to itself as 0 (diagonal elements)
                pre[i][j] = i; // Set the predecessor for node i in the shortest path from i to i as i (itself)
                continue;
            }
        }
    }
}
