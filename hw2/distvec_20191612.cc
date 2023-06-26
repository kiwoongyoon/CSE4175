#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
int nodecnt ; //how many nodes 
int first_start, first_end, first_cost; // inital node1, node2, cost 
int start; //startpoint
int end;// endpoint
char message[1000]; // messages file
char *oneline; // messages file
int new_start, new_end, new_cost; // changes file

void fixtable(int ***) ; 
void addnode(int***) ; 
void showTable(FILE*, int***) ; 
void print_message(FILE *fp, FILE *out, int***) ; 


int main(int argc, char *argv[]) {

	if (argc != 4) {
		// Check if the correct number of command-line arguments is provided
		fprintf(stderr, "usage: distvec topologyfile messagesfile changesfile\n");
		return 0;
	}

	FILE *fp1, *fp2, *fp3, *result;

	// Open the input and output files
	fp1 = fopen(argv[1], "r");
	fp2 = fopen(argv[2], "r");
	fp3 = fopen(argv[3], "r");
	result = fopen("output_dv.txt", "wt");

	// Check if any of the files failed to open
	if (fp1 == NULL || fp2 == NULL || fp3 == NULL) {
		fprintf(stderr, "Error: open input file.\n");
		return 0;
	}

	//int nodecnt;
	fscanf(fp1, "%d", &nodecnt);
	//printf("hihi %d\n",nodecnt) ;
	
	// Allocate memory for the distance table
	int ***disttable;
	disttable = (int ***)malloc(sizeof(int **) * nodecnt);

	// Initialize the distance table
	for (int i = 0; i < nodecnt; i++) {
		disttable[i] = (int **)malloc(sizeof(int *) * nodecnt);
		for (int j = 0; j < nodecnt; j++) {
			disttable[i][j] = (int *)malloc(2 * sizeof(int));
			if (i != j) {
				// Set initial values for non-diagonal entries
				disttable[i][j][0] = -1;
				disttable[i][j][1] = -999;
			} else {
				// Set initial values for diagonal entries
				disttable[i][j][0] = i;
				disttable[i][j][1] = 0;
			}
		//	printf("%d %d %d \n",i,j,disttable[i][j][0]); 
		}
	}

	while (fscanf(fp1, "%d%d%d", &first_start, &first_end, &first_cost) == 3) {
	//printf("%d%d%d\n" , first_start, first_end , first_cost) ; // success !! 
    disttable[first_start][first_end][0] = first_end;
    disttable[first_start][first_end][1] = first_cost;
    disttable[first_end][first_start][0] = first_start;
    disttable[first_end][first_start][1] = first_cost;
	}
	
   fclose(fp1); 


	
	fixtable(disttable); // Exchange table
// 	printf("start now \n"); 
	
//     for(int i = 0; i<nodecnt ; i++){
// 		for(int j =0 ; j<nodecnt ; j++){
// 				printf("%d %d\n" ,disttable[i][j][0], disttable[i][j][1]); ;
			
// 		}
// 	}


	if (result == NULL) {
		fprintf(stderr, "Error: open output file. \n");
		return 0  ; }
	showTable(result, disttable);
	print_message(fp2, result, disttable);

	// 5. Read changes file and update routing table.
	while(1){
		//init_table(disttable); // init routing table
		for(int i=0; i<nodecnt; i++){
			for(int j=0; j<nodecnt; j++){
				if(i==j) {
					disttable[i][j][0] = j;
					disttable[i][j][1] = 0;
				}
				else{
					disttable[i][j][0] = -1; // next node
					disttable[i][j][1] = -999; // distance
				} 
			}
		}
		fp1 = fopen(argv[1], "r");
		fseek(fp1, 0, SEEK_SET) ; 
		fscanf(fp1, "%d", &nodecnt);
	//	printf("%d\n", nodecnt) ; 
			
		while(!feof(fp1)){ // fill routing table
			fscanf(fp1, "%d%d%d", &first_start, &first_end, &first_cost);
			disttable[first_start][first_end][0] = first_end;
			disttable[first_start][first_end][1] = first_cost;
			disttable[first_end][first_start][0] = first_start;
			disttable[first_end][first_start][1] = first_cost;
		} fclose(fp1);

		if(fscanf(fp3, "%d%d%d", &new_start, &new_end, &new_cost)==3){

			addnode(disttable); // change routing table
			fixtable(disttable); // exchange routing table
			showTable(result, disttable); // print changed routing table
		}else{
			break ; 
		}
		

		fp2 = fopen(argv[2], "r");
		
	
		print_message(fp2 , result, disttable) ; 
	}

	printf("Complete. Output file written to output_dv.txt.\n");

	fclose(fp3);
	fclose(result);
	// Free the distance table
    for (int i = 0; i < nodecnt; i++) {
        for (int j = 0; j < nodecnt; j++) {
            free(disttable[i][j]);
        }
        free(disttable[i]);
    }
    free(disttable);

	return 0;
}

void fixtable(int ***gra) {
    // for(int i = 0; i<nodecnt ; i++){
 
    int endflag ; 

        while (1) {
        //printf("start here in func\n") ; 
         endflag = 0;
        // Iterate over all nodes i
        for (int i = 0; i < nodecnt; i++) {
            // Iterate over all nodes j
           // printf("%d is the number\n", i )  ; 
            for (int j = 0; j < nodecnt; j++) {
                // Check if j is the next hop for node i and the cost is not zero
                if (j == gra[i][j][0] && gra[i][j][1]!=0) {
                    // Iterate over all nodes k
                   // printf("ready to fix %d%d\n", i,j) ; 
                    for (int k = 0; k < nodecnt; k++) {
                        // Skip if the cost from j to k is negative
                        if (gra[j][k][1] < 0) {
                            continue;
                        }
                        // Update the distance from i to k if it's not set or the new path is shorter
                        if (gra[i][k][1] == -999 || gra[i][j][1] + gra[j][k][1] < gra[i][k][1]) {
							if(gra[i][j][1]+gra[j][k][1]<0){
								continue; 
							}
							int howfar = gra[i][j][1] + gra[j][k][1];
                            gra[i][k][0] = j;
                            gra[i][k][1] = howfar;
                            endflag = 1;
                        }
                    }
                }
            }
        }
        if(endflag ==0){
            break ;
        }
    }
}


void addnode(int ***disttable) {
    // Update the distance from new_start to new_end
    disttable[new_start][new_end][0] = new_end;
    disttable[new_start][new_end][1] = new_cost;

    // Update the distance from new_end to new_start
    disttable[new_end][new_start][0] = new_start;
    disttable[new_end][new_start][1] = new_cost;
}

void showTable(FILE *out, int ***disttable) {
    int i, j;
    for (i = 0; i < nodecnt; i++) {
        for (j = 0; j < nodecnt; j++) {
            // Check if a valid distance exists between nodes x and y
			if(disttable[i][j][1] ==-999){
			//	printf("no line \n") ; 
			}
            if (disttable[i][j][1] != -999) {
			//	printf("it has link\n") ; 
                // Print the distance information to the output file
                fprintf(out, "%d %d %d\n", j, disttable[i][j][0], disttable[i][j][1]);
            }
        }
        // Print a newline character to separate the distances for each node
        fprintf(out, "\n");
    }
}


void print_message(FILE *fp, FILE *out, int ***disttable){
    fseek(fp, 0, SEEK_SET) ; 
	while(!feof(fp)){
		if(fscanf(fp, "%d%d", &start, &end)==2){
		oneline = fgets(message, 1000, fp);
		//fprintf(stdout, "%s\n",oneline) ; 
		if(oneline ) {
			if(disttable[start][end][1] != -999){
				fprintf(out, "from %d to %d cost %d hops ", start, end, disttable[start][end][1]);
				while(start!=end){
					fprintf(out, "%d ", start);
					start = disttable[start][end][0];
				} fprintf(out, "message%s", oneline);
			}else {
				fprintf(out, "from %d to %d cost infinite hops unreachable message%s", start, end, oneline);
			}
		}
	  }
	} 
	fprintf(out, "\n"); 
	fclose(fp);
}

