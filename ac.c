#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <omp.h>

void fillMatrix(int n, double * matrix);

void printMatrixByRows(int n, double * matrix);
void printMatrixByRowsInFile(int n, double * matrix, const char* filename);

double * createMatrix(int n);



int main(int argc, char * argv[]) {
	unsigned int mSize = 0, i, k;
	double * a, * b, * c;
    omp_sched_t schedule = 0;
    int chunksize = 0;
    double wtime;


    //check command line arguments
    if (argc == 2 && isdigit(argv[1][0])) {
        mSize = atoi(argv[1]);
    }
    else {
        printf("USAGE\n   %s [SIZE]\n", argv[0]);
        return 0;
    }


    a = (double*)createMatrix(mSize);
	b = (double*)createMatrix(mSize);
	c = (double*)createMatrix(mSize);

	fillMatrix(mSize, a);
	fillMatrix(mSize, b);

//	flops = (double)mSize * (double)mSize * (double)mSize * (double)mSize * 3.0;


    omp_get_schedule(&schedule, &chunksize);
    printf ( "Adjoint convolution benchmark with matrix size %d\n", mSize );
    printf ( "OpenMP schedule: OMP_SCHEDULE=%d\n", (int) schedule );

    wtime = omp_get_wtime ( );

#pragma omp parallel for shared(a,b,c) private(i,k) schedule(runtime)
    for (i = 0; i < mSize*mSize; i++) {
        c[i]= 0;
        for (k = i; k < mSize*mSize; k++) {
            c[i] += 5.5*b[k]*a[k-i];
        }
    }

    wtime = omp_get_wtime ( ) - wtime;
    printf ( "\n" );
    printf ( "  Time = %g seconds.\n", wtime );

	free(a);
	free(b);
	free(c);
}



double * createMatrix(int n) {
	
	double * m = (double*) malloc(n*n*sizeof(double));
	
	return m;
}

void fillMatrix(int n, double * matrix) {
	int i;
	for (i = 0; i < n*n; i++) {
		matrix[i] = (rand()%10) - 5; //between -5 and 4
	}
}


void printMatrixByRows(int n, double * matrix) {
	int i, j;

	printf("{");
	for (i = 0; i < n; i++) {
		printf("[");
		for (j = 0; j < n; j++) {
			printf("%d",(int)matrix[i*n+j]);
			if (j != n-1)
				printf(",");
			else
				printf("]");
		}
		if (i != n-1)
			printf(",\n");
	}
	printf("}\n");
}

void printMatrixByRowsInFile(int n, double *matrix, const char* filename) {
	int i, j;

	FILE *fp = fopen(filename, "w");

	fprintf(fp, "{");
	for (i = 0; i < n; i++) {
		fprintf(fp, "[");
		for (j = 0; j < n; j++) {
			fprintf(fp, "%d",(int)matrix[i*n+j]);
			if (j != n-1)
				fprintf(fp, ",");
			else
				fprintf(fp, "]");
		}
		if (i != n-1)
			fprintf(fp, ",\n");
	}
	fprintf(fp, "}\n");
	fclose(fp);
}
