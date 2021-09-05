#include <iostream>
#include <fstream>
#include <string>

#define MAX_KEY_SIZE 18
#define MAX_TEXT_SIZE 1000
#define NO_OF_CHARACTERS 26

class Encryption
{
public:
    int pLength, plainText[MAX_TEXT_SIZE], N, K[MAX_KEY_SIZE][MAX_KEY_SIZE], cipherText[MAX_TEXT_SIZE];

    void getPlainText()
    {
        std::string str;
        std::ifstream fin;
        fin.open("PlainText.txt");
        std::getline(fin, str);
        fin.close();
        if (str.length() == 0)
        {
            std::cout << "Length of plain text must be atleast 1. Please enter a valid plain text.\n";
            exit(0);
        }
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
    }

    void getKey()
    {
        std::string str;
        int r, c;
        bool invalidKey = false;
        std::ifstream fin;
        fin.open("Key.txt");
        fin >> N;
        if (N < 2 || N > 10)
        {
            std::cout << "Key size must be between 2-10. Please enter a valid key size.\n";
            exit(0);
        }
        for (int i = 0; i < N; ++i)
            for (int j = 0; j < N; ++j)
            {
                fin >> K[i][j];
                if (K[i][j] >= 65 && K[i][j] <= 90)
                    K[i][j] -= 65;
                else if (K[i][j] >= 97 && K[i][j] <= 122)
                    K[i][j] -= 97;
            }
        if (determinant(K, N) == 0)
        {
            std::cout << "This key can't be used as it produces a singular matrix. Please enter a new valid key.\n";
            exit(0);
        }
        else if (modInverse(determinant(K, N), NO_OF_CHARACTERS) == -1)
        {
            std::cout << "This key can't be used as Inverse of determinant mod " << NO_OF_CHARACTERS << " doesn't exist. Please enter a new valid key.\n";
            exit(0);
        }
        fin.close();
    }

    void calcCipherText()
    {
        int tempPT[MAX_KEY_SIZE];
        int i, j, k;
        for (i = 0; i <= pLength / N; ++i)
        {
            for (j = 0; j < N; ++j)
            {
                if ((N * i + j) >= pLength)
                    tempPT[j] = 23;
                else
                    tempPT[j] = plainText[N * i + j];
            }
            for (j = 0; j < N; ++j)
            {
                cipherText[N * i + j] = 0;
                for (k = 0; k < N; ++k)
                    cipherText[N * i + j] += K[j][k] * tempPT[k];
                cipherText[N * i + j] %= 26;
            }
        }
        pLength += pLength % N ? N - pLength % N : 0;
    }

    void printCipherText()
    {
        std::ofstream fout;
        fout.open("CipherText.txt");
        std::string str = "";
        for (int i = 0; i < pLength; ++i)
            str += char(cipherText[i] + 65);
        fout << str;
        fout.close();
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
};

int main()
{
    Encryption e;
    e.getPlainText();
    e.getKey();
    e.calcCipherText();
    e.printCipherText();
    return 0;
}
