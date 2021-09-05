#include <iostream>
#include <string>

#define MAX_TEXT_SIZE 1000
#define MAX_KEY_SIZE 10
#define NO_OF_CHARACTERS 26

int mod(int a, int m)
{
    int mod = a % m;
    return mod < 0 ? mod + m : mod;
}

int modInverse(int a, int n)
{
    while (a < 0)
        a += n;
    int i;
    for (i = 1; i < n; ++i)
    {
        if (a * i % n == 1)
            return i;
    }
    if (i == n)
        return -1;
}

void getCofactor(int A[MAX_KEY_SIZE][MAX_KEY_SIZE], int temp[MAX_KEY_SIZE][MAX_KEY_SIZE], int m, int n, int size)
{
    int i, j, r = 0, c = 0;
    for (i = 0; i < size; ++i)
        for (j = 0; j < size; ++j)
        {
            if (i != m && j != n)
            {
                temp[r][c++] = A[i][j];
                if (c == size - 1)
                {
                    r++;
                    c = 0;
                }
            }
        }
}

int determinant(int A[MAX_KEY_SIZE][MAX_KEY_SIZE], int size)
{
    if (size == 1)
    {
        return A[0][0];
    }
    int i, det = 0;
    int temp[MAX_KEY_SIZE][MAX_KEY_SIZE];
    int sign = 1;
    for (i = 0; i < size; ++i)
    {
        getCofactor(A, temp, 0, i, size);
        det += sign * A[0][i] * determinant(temp, size - 1);
        sign = -sign;
    }
    return det;
}

void matrix_multiplication(int A[MAX_KEY_SIZE][MAX_KEY_SIZE], int B[MAX_KEY_SIZE][MAX_KEY_SIZE], int C[MAX_KEY_SIZE][MAX_KEY_SIZE], int l, int m, int n)
{
    int i, j, k;
    for (i = 0; i < l; ++i)
        for (j = 0; j < n; ++j)
        {
            C[i][j] = 0;
            for (k = 0; k < m; ++k)
                C[i][j] += A[i][k] * B[k][j];
        }
}

void adjoint(int A[MAX_KEY_SIZE][MAX_KEY_SIZE], int adjA[MAX_KEY_SIZE][MAX_KEY_SIZE], int size)
{
    int i, j;
    int temp[MAX_KEY_SIZE][MAX_KEY_SIZE];
    for (i = 0; i < size; ++i)
        for (j = 0; j < size; ++j)
        {
            getCofactor(A, temp, i, j, size);
            adjA[j][i] = ((i + j) % 2 ? -1 : 1) * determinant(temp, size - 1);
        }
}

void matrixMod(int A[MAX_KEY_SIZE][MAX_KEY_SIZE], int m, int n)
{
    int i, j;
    for (i = 0; i < m; ++i)
        for (j = 0; j < n; ++j)
            A[i][j] = mod(A[i][j], NO_OF_CHARACTERS);
}

void print(int A[MAX_KEY_SIZE][MAX_KEY_SIZE], int m, int n)
{
    int i, j;
    for (i = 0; i < m; ++i)
    {
        for (j = 0; j < n; ++j)
            std::cout << A[i][j] << " ";
        std::cout << std::endl;
    }
}

void inverse(int A[MAX_KEY_SIZE][MAX_KEY_SIZE], int invA[MAX_KEY_SIZE][MAX_KEY_SIZE], int size)
{
    int i, j;
    int adjA[MAX_KEY_SIZE][MAX_KEY_SIZE];
    int det = determinant(A, size);
    int detInv = modInverse(det, NO_OF_CHARACTERS);
    adjoint(A, adjA, size);
    for (i = 0; i < size; ++i)
        for (j = 0; j < size; ++j)
            invA[i][j] = detInv * adjA[i][j];
    matrixMod(invA, size, size);
}

void createTextMatrix(int text[MAX_TEXT_SIZE], int textMatrix[MAX_KEY_SIZE][MAX_KEY_SIZE], int size)
{
    int i, j, k = 0;
    for (i = 0; i < size; ++i)
        for (j = 0; j < size; ++j)
        {
            textMatrix[j][i] = text[k];
            k++;
        }
}

double clacIndexOfCoincidence(std::string str)
{
    float IC;
    float charCount[NO_OF_CHARACTERS] = {0};
    int i;
    int l = 0;
    for (i = 0; i < str.length(); ++i)
    {
        if (int(str[i]) >= 65 && int(str[i]) <= 90)
        {
            charCount[int(str[i]) - 65]++;
            l++;
        }
        else if (int(str[i]) >= 97 && int(str[i]) <= 122)
        {
            charCount[int(str[i]) - 97]++;
            l++;
        }
    }
    float freqSum = 0;
    for (i = 0; i < NO_OF_CHARACTERS; ++i)
        freqSum += charCount[i] * (charCount[i] - 1);
    IC = (freqSum / (l * (l - 1)));
    return IC;
}

int main()
{
    int i;
    bool invalidInput = 0;
    int plainTextTransformed[MAX_TEXT_SIZE], cipherTextTransformed[MAX_TEXT_SIZE], plainTextMatrix[MAX_KEY_SIZE][MAX_KEY_SIZE], cipherTextMatrix[MAX_KEY_SIZE][MAX_KEY_SIZE];
    std::string plainText, cipherText;
    std::getline(std::cin, plainText);
    std::getline(std::cin, cipherText);
    for (i = 0; i < plainText.length(); ++i)
    {
        if (int(plainText[i]) >= 97 && int(plainText[i]) <= 122)
            plainTextTransformed[i] = int(plainText[i]) - 97;
        else if (int(plainText[i]) >= 65 && int(plainText[i]) <= 90)
            plainTextTransformed[i] = int(plainText[i]) - 65;
        else
        {
            invalidInput = true;
            break;
        }

        if (int(cipherText[i]) >= 97 && int(cipherText[i]) <= 122)
            cipherTextTransformed[i] = int(cipherText[i]) - 97;
        else if (int(cipherText[i]) >= 65 && int(cipherText[i]) <= 90)
            cipherTextTransformed[i] = int(cipherText[i]) - 65;
        else
        {
            invalidInput = true;
            break;
        }
    }
    if (!invalidInput)
    {
        createTextMatrix(cipherTextTransformed, cipherTextMatrix, 2);
        createTextMatrix(plainTextTransformed, plainTextMatrix, 2);
    }
    else
    {
        std::cout << "Invalid Input";
    }
    int inversePlainTextMatrix[MAX_KEY_SIZE][MAX_KEY_SIZE];
    inverse(plainTextMatrix, inversePlainTextMatrix, 2);
    int det = determinant(inversePlainTextMatrix, 2);
    if (det != 0)
    {
        if (modInverse(det, NO_OF_CHARACTERS) != -1)
        {
            int keyMatrix[MAX_KEY_SIZE][MAX_KEY_SIZE];
            matrix_multiplication(cipherTextMatrix, inversePlainTextMatrix, keyMatrix, 2, 2, 2);
            matrixMod(keyMatrix, 2, 2);
            print(keyMatrix, 2, 2);
        }
        else
            std::cout << "Mod Inverse dont exist";
    }
    else
        std::cout << "Singlar PTM";
    return 0;
}
