#include <iostream>
#include <fstream>
#include <string>

#define MAX_KEY_SIZE 10
#define MAX_TEXT_SIZE 100000
#define NO_OF_CHARACTERS 26

#define PLAIN_TEXT_FILE_PATH "./Sample/PlainText1.txt"
#define CIPHER_TEXT_FILE_PATH "./Sample/CipherText1.txt"
#define KEY_FILE_PATH "./Sample/Key1.txt"

class Encryption
{
public:
    int pLength, plainText[MAX_TEXT_SIZE], N, K[MAX_KEY_SIZE][MAX_KEY_SIZE], cipherText[MAX_TEXT_SIZE];

    void getPlainText()                 
    {
        //inputs the plain text from the file PLAIN_TEXT_FILE_PATH by ignoring the non-alphabetic characters and stores alphabets as integer in range 0-25
        std::string str;

        //input plain text
        std::ifstream fin;
        fin.open(PLAIN_TEXT_FILE_PATH);
        std::getline(fin, str);
        fin.close();

        //ignoring the non-aplhabetic characters and storing the integer value of alphabetic characters in the range 0-25
        pLength = 0;
        for (int i = 0; i < str.length(); ++i)
        {
            if (int(str[i]) >= 65 && int(str[i]) <= 90)
            {
                plainText[pLength] = int(str[i]) - 65;
                pLength++;
            }
            else if (int(str[i]) >= 97 && int(str[i]) <= 122)
            {
                plainText[pLength] = int(str[i]) - 97;
                pLength++;
            }
        }

        //checking if the plain text is of length atleast 1
        if (pLength == 0)
        {
            std::cout << "Length of plain text must be atleast 1. Please enter a valid plain text.\n";
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

    void calcCipherText()
    {
        //calculating the cipher text by matrix multiplication of key matrix and plain text sub-matrix (of size keySize)
        int i, j, k, tempPT[MAX_KEY_SIZE];

        //plain text is divided into plainTextSize/keySize no. of blocks and for each such block, cipher text is calculated
        for (i = 0; i <= pLength / N; ++i)
        {
            for (j = 0; j < N; ++j)
            {
                //in case plain text size is not a mulitple of keySize, then it is made to be so by appending 23 (i.e. X) appropriate no of times 
                if ((N * i + j) >= pLength)
                    tempPT[j] = 23;

                //else plain text is copied into temporary plain text matrix
                else
                    tempPT[j] = plainText[N * i + j];
            }

            //matrix multiplication of key matrix and temporary plain text matrix to calculate cipher text
            for (j = 0; j < N; ++j)
            {
                cipherText[N * i + j] = 0;
                for (k = 0; k < N; ++k)
                    cipherText[N * i + j] += K[j][k] * tempPT[k];
                cipherText[N * i + j] %= 26;
            }
        }

        //in case plain text size is not a mulitple of keySize, then it is made to be so
        pLength += pLength % N ? N - pLength % N : 0;
    }

    void printCipherText()
    {
        //calculated cipher text is written into the CIPHER_TEXT_FILE_PATH file by transforming so formed cipher text into ASCII valued cipher text 
        std::ofstream fout;
        fout.open(CIPHER_TEXT_FILE_PATH);

        std::string str = "";
        for (int i = 0; i < pLength; ++i)
            str += char(cipherText[i] + 65);

        fout << str;

        fout.close();
    }
};

int main()
{
    Encryption e;
    std::cout << "Loading Plain Text from " << PLAIN_TEXT_FILE_PATH << "...\n";
    e.getPlainText();
    std::cout << "Plain Text loaded successfully\n";
    std::cout << "Loading Key from " << KEY_FILE_PATH << "...\n";
    e.getKey();
    std::cout << "Key loaded successfully\n";
    std::cout << "Calculating Cipher Text...\n";
    e.calcCipherText();
    std::cout << "Cipher Text computed successfully\n";
    std::cout << "Storing Cipher Text in " << CIPHER_TEXT_FILE_PATH << "...\n";
    e.printCipherText();
    std::cout << "Cipher Text stored successfully\n";
    return 0;
}