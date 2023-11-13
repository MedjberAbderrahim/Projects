#include <stdio.h>
#include <unistd.h>

void PrintMatrice(int size, int A[size][size]){
    
    for(int i = 0; i < size; i++){

        for(int j = 0; j < size; j++)   printf("%d  ", A[i][j]);
        printf("\n");
    }
    printf("\n\n");
}

void ScanMatrice(int size, int A[size][size]){
    
    int ExitStatus;
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++)   
            do{
                printf("Matrice[%d][%d] = ", i, j);
                ExitStatus = scanf("%d", &A[i][j]);
                if(!ExitStatus){
                    fprintf(stderr, "ERROR! [scanf() in main()]: No value was assigned.\nExiting...");
                    exit(1);
                }
                else if(ExitStatus < 0){
                    fprintf(stderr, "ERROR! [scanf() in main()]: Read error encountered or end-of-file(EOF) reached before any assignment was made.\nExiting...");
                    exit(1);
                }

                if(A[i][j] != 1 && A[i][j] != 0){
                    printf("Invalide Value, enter again.\n");
                    sleep(2);
                }
            }while(A[i][j] != 1 && A[i][j] != 0);
    }
}


int AliveNeighbours(int size, int A[size][size], int x, int y){
    int Sum = 0;
    
    if(x != 0 && A[x - 1][y]) Sum++;
    if(x != size - 1 && A[x + 1][y]) Sum++;
    if(y != 0 && A[x][y - 1]) Sum++;
    if(y != size - 1 && A[x][y + 1]) Sum++;
    if(x != 0 && y != 0 && A[x -1][y - 1]) Sum++;
    if(x != 0 && y != size - 1 && A[x - 1][y + 1]) Sum++;
    if(x != size - 1 && y != 0 && A[x + 1][y - 1]) Sum++;
    if(x != size - 1 && y != size - 1 && A[x + 1][y + 1]) Sum++;
    
    return Sum;
}

int main()
{
    int size, ExitStatus, Stalemate;
    do{
        printf("Enter the size; 4 < size < 30: ");

        ExitStatus = scanf("%d", &size);
        if(!ExitStatus){
            fprintf(stderr, "ERROR! [scanf() in main()]: No value was assigned.\nExiting...");
            exit(1);
        }
        else if(ExitStatus < 0){
            fprintf(stderr, "ERROR! [scanf() in main()]: Read error encountered or end-of-file(EOF) reached before any assignment was made.\nExiting...");
            exit(1);
        }

    }while( size < 5 || size > 29);

    int Matrice[size][size], swpMatrice[size][size], AdjNeigh;

    printf("Enter the values (only 1 and 0):\n");
    ScanMatrice(size, Matrice);

    printf("\nPrinting results:\n");
    PrintMatrice(size, Matrice);

    printf("\nStarting Game...\n");
    while(1){
        for(int i = 0; i < size; i++){
            for(int j = 0; j < size; j++){
                
                AdjNeigh = AliveNeighbours(size, Matrice, i, j);
                if( Matrice[i][j] && AdjNeigh != 2 && AdjNeigh != 3)    swpMatrice[i][j] = 0;
                else if(!Matrice[i][j] && AdjNeigh == 3)  swpMatrice[i][j] = 1;
                else    swpMatrice[i][j] = Matrice[i][j];
            }
        }
        Stalemate = 0;
        for(int i = 0; i < size; i++)   for(int j = 0; j < size; j++){
            if(Matrice[i][j] != swpMatrice[i][j])   Stalemate = 1;
            Matrice[i][j] = swpMatrice[i][j];
        }

        sleep(1);   
        if(!Stalemate){
            printf("Game reached a stalemate.\nLeaving...\n");
            return 0;
        }

        PrintMatrice(size, Matrice);
    }   
}