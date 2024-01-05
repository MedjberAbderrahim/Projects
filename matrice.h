#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

typedef struct matrice{
    float **mat;
    int x, y;
}matrice, *pMatrice;

pMatrice matrice_create(int x, int y){
    pMatrice M = (pMatrice)malloc(sizeof(matrice));
    if(M == NULL)                   goto allocation_error;

    M->mat = (float**)malloc(x * sizeof(float*));
    if(M->mat == NULL)              goto allocation_error;

    for(int i = 0; i < x; i++){
        M->mat[i] = (float*)malloc(y * sizeof(float));
        if(M->mat[i] == NULL)       goto allocation_error;
    }
    M->x = x;           M->y = y;
    return M;

    allocation_error:
        fprintf(stderr, "ERROR! [malloc in matrice_create]: couldn't allocate memory.\n");
        exit(EXIT_FAILURE);
}

void matrice_print(pMatrice M, int precision){
    if(precision < 0){
        fprintf(stderr, "Negative precision parameter.\n");
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < M->x; i++){
        for(int j = 0; j < M->y; j++)
            printf("%.*f\t", precision, M->mat[i][j]);
        printf("\n");
    }
}

pMatrice matrice_add(pMatrice MA, pMatrice MB){
    if(MA->x != MB->x || MA->y != MB->y){
        fprintf(stderr, "Matrices' sizes are not compatible for matriciel addition.\n");
        exit(EXIT_FAILURE);
    }
    
    pMatrice MC = matrice_create(MA->x, MA->y);
    for(int i = 0; i < MA->x; i++)
        for(int j = 0; j < MA->y; j++)
            MC->mat[i][j] = MA->mat[i][j] + MB->mat[i][j];

    return MC;
}

void matrice_free(pMatrice M){
    for(int i = 0; i < M->x; i++)
        free(M->mat[i]);
    free(M->mat);           free(M);
}

pMatrice matrice_mulMat(pMatrice MA, pMatrice MB){
    if(MA->y != MB->x){
        fprintf(stderr, "Matrices' sizes are not compatible for matriciel multiplication.\n");
        exit(EXIT_FAILURE);
    }

    pMatrice MC = matrice_create(MA->x, MB->y);
    for(int i = 0; i < MC->x; i++)
        for(int j = 0; j < MC->y; j++){
            MC->mat[i][j] = 0;
            for(int k = 0; k < MA->y; k++)
                MC->mat[i][j] += MA->mat[i][k] * MB->mat[k][j];
        }
    return MC;
}

void matrice_mulScal(pMatrice M, float scalaire){
    for(int i = 0; i < M->x; i++)
        for(int j = 0; j < M->y; j++)
            M->mat[i][j] *= scalaire;
}

pMatrice matrice_transpose(pMatrice M){
    if(M->x != M->y){
        fprintf(stderr, "Matrix's size is not compatible for transpose.\n");
        exit(EXIT_FAILURE);
    }
    pMatrice transpose = matrice_create(M->x, M->y);

    for(int i = 0; i < M->x; i++)
        for(int j = 0; j < M->y; j++)
            transpose->mat[i][j] = M->mat[j][i];
    
    return transpose;
}

float matrice_trace(pMatrice M){
    float trace = 0;
    int min = (M->x > M->y) ? M->y : M->x;

    for(int i = 0; i < min; i++)
        trace += M->mat[i][i];
    return trace;
}

bool matrice_isUpperTriangular(pMatrice M){
    if(M->x != M->y)                        return false;
    for(int i = 1; i < M->x; i++)
        for(int j = 0; j < i; j++)
            if(M->mat[i][j] != 0)           return false;
    return true;
}

bool matrice_isLowerTriangular(pMatrice M){
    if(M->x != M->y)                        return false;
    for(int i = 0; i < M->x; i++)
        for(int j = i + 1; j < M->y; j++)
            if(M->mat[i][j] != 0)           return false;
    return true;
}

bool matrice_isTriangular(pMatrice M){
    if(M->x != M->y)            return false;
    if(matrice_isUpperTriangular(M) || matrice_isLowerTriangular(M))       return true;
    return false;
}

bool matrice_isIdentity(pMatrice M){
    for(int i = 0; i < M->x; i++)
        for(int j = 0; j < M->y; j++)
            if(M->mat[i][j] != (i == j))    return false;
    return true;
}

bool matrice_isNull(pMatrice M){
    for(int i = 0; i < M->x; i++)
        for(int j = 0; j < M->y; j++)
            if(M->mat[i][j] != 0)           return false;
    return true;
}

bool matrice_isDiagonal(pMatrice M){
    if(M->x != M->y)                        return false;
    for(int i = 0; i < M->x; i++)
        for(int j = 0; j < M->y; j++)
            if(i != j && M->mat[i][j] != 0)                 return false;
    return true;
}

bool matrice_isScalar(pMatrice M){
    if(M->x != M->y)                        return false;
    for(int i = 0; i < M->x; i++)
        for(int j = 0; j < M->y; j++)
            if(M->mat[i][j] != (i == j) * M->mat[0][0])     return false;
    return true;
}

pMatrice _matrice_reduce(pMatrice M, int row, int column){
    pMatrice M1 = matrice_create(M->x - 1, M->y - 1);
    int x1 = 0, y1 = 0;

    for(int i = 0; i < M->x; i++){
        if(i == row)                continue;
        for(int j = 0; j < M->y; j++){
            if(j == column)         continue;
            M1->mat[x1][y1] = M->mat[i][j];
            y1++;
        }
        x1++;           y1 = 0;
    }
    return M1;
}

float matrice_determinant(pMatrice M){
    if(M->x != M->y){
        fprintf(stderr, "Matrix is not squared, can't calculate determinant.\n");
        exit(EXIT_FAILURE);
    }

    if(M->x == 2)   return ( (M->mat[0][0] * M->mat[1][1]) - (M->mat[0][1] * M->mat[1][0]) );
    
    float det = 0;          int tmpCoeff = 1;
    pMatrice tmpMatrice;
    for(int j = 0; j < M->x; j++){
        if(M->mat[0][j] == 0)       continue;
        tmpMatrice = _matrice_reduce(M, 0, j);
        det += pow(-1, j % 2) * M->mat[0][j] * matrice_determinant(tmpMatrice);
        matrice_free(tmpMatrice);
    }

    return det;
}

pMatrice matrice_cofacteur(pMatrice M){
    if(M->x != M->y){
        fprintf(stderr, "Matrix's size does not allow for cofacteur.\n");
        exit(EXIT_FAILURE);
    }

    pMatrice tmpMatrice, result = matrice_create(M->x, M->y);

    for(int i = 0; i < M->x; i++)
        for(int j = 0; j < M->y; j++){
            tmpMatrice = _matrice_reduce(M, i, j);
            result->mat[i][j] = pow(-1, (i + j) % 2) * matrice_determinant(tmpMatrice);
            matrice_free(tmpMatrice);
        }
    
    return result; 
}

pMatrice matrice_inverse(pMatrice M){
    if(M->x != M->y){
        fprintf(stderr, "Matrix's size does not allow for inverse.\n");
        exit(EXIT_FAILURE);
    }

    float det = matrice_determinant(M);
    if(det == 0){
        fprintf(stderr, "Matrix is uninversible ( det(M) = 0 ).\n");
        exit(EXIT_FAILURE);
    }

    pMatrice cofacteur = matrice_cofacteur(M);
    pMatrice inverse = matrice_transpose(cofacteur);
    matrice_mulScal(inverse, 1 / det);
    matrice_free(cofacteur);

    return inverse;
}