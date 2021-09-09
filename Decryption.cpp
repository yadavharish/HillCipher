#include <iostream>
#include <fstream>
#include <string>

#define MAX_KEY_SIZE 10
#define MAX_TEXT_SIZE 100000
#define NO_OF_CHARACTERS 26

#define CIPHER_TEXT_FILE_PATH "./Sample/CipherText.txt"
#define PLAIN_TEXT_FILE_PATH "./Sample/PlainText.txt"
#define KEY_FILE_PATH "./Sample/Key.txt"

class Decryption
{
public:
    int cLength, plainText[MAX_TEXT_SIZE], N, K[MAX_KEY_SIZE][MAX_KEY_SIZE], cipherText[MAX_TEXT_SIZE];

    void getCipherText()
    {
        //inputs the cipher text from the file CIPHER_TEXT_FILE_PATH by ignoring the non-alphabetic characters and stores alphabets as integer in range 0-25
        std::string str;
        
        //input cipher text
        std::ifstream fin;
        fin.open(CIPHER_TEXT_FILE_PATH);
        std::getline(fin, str);
        fin.close();

        //ignoring the non-aplhabetic characters and storing the integer value of alphabetic characters in the range 0-25   
        cLength = 0;
        for (int i = 0; i < str.length(); ++i)
        {
            if (int(str[i]) >= 65 && int(str[i]) <= 90)
            {
                cipherText[cLength] = int(str[i]) - 65;
                cLength++;
            }
            else if (int(str[i]) >= 97 && int(str[i]) <= 122)
            {
                cipherText[cLength] = int(str[i]) - 97;
                cLength++;
            }
            else
            {
                std::cout << "Invalid Cipher Text, cipher text can only be alphabetic character.";
                exit(0);
            }
        }

        //checking if the cipher text is of length atleast 1
        if (cLength == 0)
        {
            std::cout << "Length of cipher text must be atleast 1. Please enter a valid cipher text.\n";
            exit(0);
        }
    }

    void getKey()
    {
        //inputs the key from the file KEY_FILE_PATH by taking first integer as key size and the rest as elements of the key
        std::ifstream fin;
        fin.open(KEY_FILE_PATH);

        //input key size
        fin >> N;           

        //checking if the key size is of appropriate size or not
        if (N < 2 || N > MAX_KEY_SIZE)
        {
            std::cout << "Key size must be between 2-" <<MAX_KEY_SIZE << ". Please enter a valid key size.\n";
            exit(0);
        }

        //input elements of the key
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
                fin >> K[i][j];
        

        //calculating determinant of the key matrix and checking whether it is non-singular or not
        int det = determinant(K, N);
        if (det == 0)
        {
            std::cout << "This key can't be used as it produces a singular matrix. Please enter a new valid key.\n";
            exit(0);
        }

        //in case determinant of key matrix is non-singular then it's inverse modulus NO_OF_CHARACTERS must also exist
        else if (modInverse(det, NO_OF_CHARACTERS) == -1)
        {
            std::cout << "This key can't be used as Inverse of determinant mod " << NO_OF_CHARACTERS << " doesn't exist. Please enter a new valid key.\n";
            exit(0);
        }

        fin.close();
    }

    int determinant(int A[MAX_KEY_SIZE][MAX_KEY_SIZE], int size)
    {
        //to calculate the determinant of the argumented matrix
        if (size == 1)
            return A[0][0];

        int i, det = 0, sign = 1, temp[MAX_KEY_SIZE][MAX_KEY_SIZE];

        for (i = 0; i < size; ++i)
        {
            getCofactor(A, temp, 0, i, size);
            det += sign * A[0][i] * determinant(temp, size - 1);
            sign = -sign;
        }
        return det;
    }

    void adjoint(int A[MAX_KEY_SIZE][MAX_KEY_SIZE], int adjA[MAX_KEY_SIZE][MAX_KEY_SIZE], int size)
    {
        //calculates the adjoint of the given matrix
        int i, j, temp[MAX_KEY_SIZE][MAX_KEY_SIZE];
        for (i = 0; i < size; ++i)
            for (j = 0; j < size; ++j)
            {
                getCofactor(A, temp, i, j, size);
                adjA[j][i] = ((i + j) % 2 ? -1 : 1) * determinant(temp, size - 1);
            }
    }

    void getCofactor(int A[MAX_KEY_SIZE][MAX_KEY_SIZE], int temp[MAX_KEY_SIZE][MAX_KEY_SIZE], int m, int n, int size)
    {
        //calculates the cofactor of given element of given matrix
        int i, j, r = 0, c = 0;
        
        for (i = 0; i < size; ++i)
            for (j = 0; j < size; ++j)
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

    int mod(int a, int m)
    {
        //returns a modulus m
        int mod = a % m;
        return mod < 0 ? mod + m : mod;
    }

    int modInverse(int a, int n)
    {
        //calculates a^(-1) mod n
        while (a < 0)
            a += n;
        
        for (int i = 1; i < n; ++i)
            if (a * i % n == 1)
                return i;

        return -1;
    }

    void inverse(int A[MAX_KEY_SIZE][MAX_KEY_SIZE], int invA[MAX_KEY_SIZE][MAX_KEY_SIZE], int size)
    {
        //calculates the inverse of the given matrix
        int i, j, adjA[MAX_KEY_SIZE][MAX_KEY_SIZE], det, detInv;
        
        det = determinant(A, size);
        detInv = modInverse(det, NO_OF_CHARACTERS);
        
        adjoint(A, adjA, size);
        
        for (i = 0; i < size; ++i)
            for (j = 0; j < size; ++j)
                invA[i][j] = detInv * adjA[i][j];
        
        matrixMod(invA, size, size);
    }

    void matrixMod(int A[MAX_KEY_SIZE][MAX_KEY_SIZE], int m, int n)
    {
        //computes a modulus NO_OF_CHARACTERS for each element a of given matrix A
        int i, j;
        for (i = 0; i < m; ++i)
            for (j = 0; j < n; ++j)
                A[i][j] = mod(A[i][j], NO_OF_CHARACTERS);
    }

    void matrix_multiplication(int A[MAX_KEY_SIZE][MAX_KEY_SIZE], int B[MAX_KEY_SIZE][MAX_KEY_SIZE], int C[MAX_KEY_SIZE][MAX_KEY_SIZE], int l, int m, int n)
    {
        //computes C = AxB where A, B, and C are matrices
        int i, j, k;

        for (i = 0; i < l; ++i)
            for (j = 0; j < n; ++j)
            {
                C[i][j] = 0;
                for (k = 0; k < m; ++k)
                    C[i][j] += A[i][k] * B[k][j];
            }
    }
    
    void calcPlainText()
    {
        //calculating the cipher text by matrix multiplication of inverse key matrix and cipher text sub-matrix (of size keySize)
        int invK[MAX_KEY_SIZE][MAX_KEY_SIZE], tempCT[MAX_KEY_SIZE], i, j, k;

        //computing inverse of key matrix
        inverse(K, invK, N);

        for (i = 0; i <= cLength / N; ++i)
        {
            for (j = 0; j < N; ++j)
                tempCT[j] = cipherText[N * i + j];

            //matrix multiplication of inverse key matrix and temporary cipher text matrix to calculate plain text
            for (j = 0; j < N; ++j)
            {
                plainText[N * i + j] = 0;
                for (k = 0; k < N; ++k)
                    plainText[N * i + j] += invK[j][k] * tempCT[k];
                plainText[N * i + j] %= 26;
            }
        }

        //in case cipher text size is not a mulitple of keySize, then it is made to be so
        cLength += cLength % N ? N - cLength % N : 0;
    }

    void printPlainText()
    {
        //calculated plain text is written into the PLAIN_TEXT_FILE_PATH file by transforming so formed plain text into ASCII valued plain text 
        std::ofstream fout;
        fout.open(PLAIN_TEXT_FILE_PATH);
        std::string str = "";
        for (int i = 0; i < cLength; ++i)
            str += char(plainText[i] + 65);
        fout << str;
        fout.close();
    }
};

int main()
{
    Decryption d;
    d.getCipherText();
    d.getKey();
    d.calcPlainText();
    d.printPlainText();
    return 0;
}
