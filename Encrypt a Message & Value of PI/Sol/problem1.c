#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>

int main(int argc, char* argv[])
{
    int rank, np, key;

    char* task, * encrypted = NULL, * message = NULL;
    int size, remender, remain, bigSize, task_siz;
    int i = 0, j = 0;

    /* Start up MPI */
    MPI_Init(&argc, &argv);

    /* Find out process rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    /* Find out number of process */
    MPI_Comm_size(MPI_COMM_WORLD, &np);


    if (rank == 0) {

        printf("\nKEY/SHIFT: ");
        scanf("%d", &key);

        FILE* fptr;
        // Opening file in reading mode
        fptr = fopen("Plaintext.txt", "r");

        if (fptr == NULL) {
            printf("Error opening file.");
            return 1;
        }

        fseek(fptr, 0, SEEK_END);
        size = ftell(fptr);
        rewind(fptr);

        task_siz = size / np;
        remender = size % np;

        if (remender == 0)
            remain = 0;
        else remain = np - remender;
        bigSize = size + remain;

        message = (char*)malloc(bigSize * sizeof(int));
        encrypted = (char*)malloc(bigSize * sizeof(int));

        fread(message, sizeof(char), size, fptr);
        message[size] = '\0';
        fclose(fptr);

        if (remender > 0) {
            task_siz++;
            j = bigSize;

            while (remain > 0) {
                message[j] = ' ';
                j--;
                remain--;
            }
        }
    }

    MPI_Bcast(&key, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&bigSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&task_siz, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&remender, 1, MPI_INT, 0, MPI_COMM_WORLD);


    task = (char*)malloc(task_siz * sizeof(int));

    MPI_Scatter(message, task_siz, MPI_CHAR, task, task_siz, MPI_CHAR, 0, MPI_COMM_WORLD);

    for (i = 0; i < task_siz; i++) {


        if (task[i] >= 'A' && task[i] <= 'Z')
            task[i] = (task[i] - 'A' + key) % 26 + 'A';

        else if (task[i] >= 'a' && task[i] <= 'z')
            task[i] = (task[i] - 'a' + key) % 26 + 'a';

        else task[i] = task[i];
    }

    MPI_Gather(task, task_siz, MPI_CHAR, encrypted, task_siz, MPI_CHAR, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("\n\n");
        printf("Plaintext: %s", message);
        printf("\n\n");
        printf("Ciphertext: ");
        for (i = 0; i < size; i++) {
            printf("%c", encrypted[i]);
        }
        printf("\n\n");

        FILE* ptr;
        ptr = fopen("Ciphertext.txt", "w"); // "w" defines "writing mode"
        for (i = 0; i < size; i++) {
            /* write to file using fputc() function */
            fputc(encrypted[i], ptr);
        }
        fclose(ptr);
    }


    MPI_Finalize();
    return 0;
}
