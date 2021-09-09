#include <iostream>
#include <string>
#include <fstream>
#include <math.h>

#define MAX_TEXT_SIZE 100000
#define MAX_KEY_SIZE 10
#define NO_OF_CHARACTERS 26
#define IC_ENGLISH 0.0686

#define CIPHER_TEXT_FILE_PATH "./Sample/CipherText1.txt"
#define PLAIN_TEXT_FILE_PATH "./Sample/PlainText1_Analysis.txt"
#define ANALYZED_KEY_FILE_PATH "./Sample/AnalyzedKey1.txt"
#define ANALYZED_PLAIN_TEXT_FILE_PATH "./Sample/AnalyzedPlainText1.txt"

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

void matrixMod(int A[MAX_KEY_SIZE][MAX_KEY_SIZE], int m, int n)
{
    //computes a modulus NO_OF_CHARACTERS for each element a of given matrix A
    int i, j;
    for (i = 0; i < m; ++i)
        for (j = 0; j < n; ++j)
            A[i][j] = mod(A[i][j], NO_OF_CHARACTERS);
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

bool createTextMatrix(int text[MAX_TEXT_SIZE], int textSize, int textMatrix[MAX_KEY_SIZE][MAX_KEY_SIZE], int count, int size)
{
    //takes text and count as input and creates a matrix of argumented size by skiping starting count*size characters

    //checking whether on skipping starting count*size alphabets, the counter overflows or not
    if (count * size + size > textSize)
        return false;

    int i, j, k = 0;
    for (i = 0; i < size; ++i)
        for (j = 0; j < size; ++j)
        {
            textMatrix[j][i] = text[count * size + k];
            k++;
        }
    return true;
}

double clacIndexOfCoincidence(std::string str)
{
    //calculates IC of argumented string
    float freqSum = 0;
    int i, l = 0, charCount[NO_OF_CHARACTERS] = {0};

    //counting no of times each alphabet occurs in the given string
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

    for (i = 0; i < NO_OF_CHARACTERS; ++i)
        freqSum += charCount[i] * (charCount[i] - 1);
    return (freqSum / (l * (l - 1)));
}

void inputText(std::string fileName, std::string *str)
{
    //inputs the text from the argumented file
    std::ifstream fin;
    fin.open(fileName);
    std::getline(fin, *str);
    fin.close();
}

int transformPlainText(std::string plainText, int plainTextTransformed[MAX_TEXT_SIZE])
{
    //ignores non-aplhabetic characters and stores the aplhabetic characters integer value in the range 0-25 calculates the length of the plain text
    int pLength = 0;
    for (int i = 0; i < plainText.length(); ++i)
    {
        if (int(plainText[i]) >= 65 && int(plainText[i]) <= 90)
        {
            plainTextTransformed[pLength] = int(plainText[i]) - 65;
            pLength++;
        }
        else if (int(plainText[i]) >= 97 && int(plainText[i]) <= 122)
        {
            plainTextTransformed[pLength] = int(plainText[i]) - 97;
            pLength++;
        }
    }
    return pLength;
}

int transformCipherText(std::string cipherText, int cipherTextTransformed[MAX_TEXT_SIZE])
{
    //ignores non-aplhabetic characters and stores the aplhabetic characters integer value in the range 0-25 calculates the length of the plain text
    int cLength = 0;
    for (int i = 0; i < cipherText.length(); ++i)
    {
        if (int(cipherText[i]) >= 65 && int(cipherText[i]) <= 90)
        {
            cipherTextTransformed[cLength] = int(cipherText[i]) - 65;
            cLength++;
        }
        else if (int(cipherText[i]) >= 97 && int(cipherText[i]) <= 122)
        {
            cipherTextTransformed[cLength] = int(cipherText[i]) - 97;
            cLength++;
        }
        else
        {
            std::cout << "Invalid Cipher Text entered. Only aplhabetic characters allowed in the cipher text." << std::endl;
            exit(0);
        }
    }
    return cLength;
}

std::string transformToASCII(int arr[MAX_TEXT_SIZE], int size)
{
    std::string str = "";
    for (int i = 0; i < size; ++i)
        str += char(arr[i] + 65);
    return str;
}

void printKey(int size, int key[MAX_KEY_SIZE][MAX_KEY_SIZE])
{
    std::ofstream fout;
    fout.open(ANALYZED_KEY_FILE_PATH);
    fout << size << " ";
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            fout << key[i][j] << " ";
    fout.close();
}

void printPlainText(std::string text)
{
    std::ofstream fout;
    fout.open(ANALYZED_PLAIN_TEXT_FILE_PATH);
    fout << text;
    fout.close();
}

int main()
{
    int i, j, k, count, det, detModInv, pLength, cLength, minICDiffKeySize, minIC;
    int keyMatrix[MAX_KEY_SIZE][MAX_KEY_SIZE], inversePlainTextMatrix[MAX_KEY_SIZE][MAX_KEY_SIZE], plainTextTransformed[MAX_TEXT_SIZE], cipherTextTransformed[MAX_TEXT_SIZE], plainTextMatrix[MAX_KEY_SIZE][MAX_KEY_SIZE], cipherTextMatrix[MAX_KEY_SIZE][MAX_KEY_SIZE], tempCT[MAX_KEY_SIZE], calcPlainText[MAX_TEXT_SIZE], inverseKeyMatrix[MAX_KEY_SIZE][MAX_KEY_SIZE], minICDiffKeyMatrix[MAX_KEY_SIZE][MAX_KEY_SIZE];
    bool inversiblePlainTextMatrixExist;
    std::string minICDiffPlainText, calcPlainTextStr;
    double ic, minICDiff = 1.0;

    //inputting plainText and cipherText
    std::string plainText, cipherText;
    std::cout << "Loading Partial Plain Text from " << PLAIN_TEXT_FILE_PATH << "...\n";
    inputText(PLAIN_TEXT_FILE_PATH, &plainText);
    std::cout << "Partial Plain Text loaded successfully\n";
    std::cout << "Loading Cipher Text from " << CIPHER_TEXT_FILE_PATH << "...\n";
    inputText(CIPHER_TEXT_FILE_PATH, &cipherText);
    std::cout << "Cipher Text loaded successfully\n";

    //transforming plain text and cipher text to the desired format
    std::cout << "Transforming Partial Plain Text...\n";
    pLength = transformPlainText(plainText, plainTextTransformed);
    std::cout << "Partial Plain Text transformed successfully\n";
    std::cout << "Transforming Cipher Text...\n";
    cLength = transformCipherText(cipherText, cipherTextTransformed);
    std::cout << "Cipher Text transformed successfully\n";

    //computing key for each possible key size and computing corresponding plain text and IC
    for (int keySize = 2; keySize <= MAX_KEY_SIZE; keySize++)
    {
        std::cout << "Performing analysis for key size " << keySize << "...\n";
        count = 0;
        inversiblePlainTextMatrixExist = true;
        std::cout << "Creating partial plain text matrix of size " << keySize << "x" << keySize << "...\n";
        do
        {
            //creating a plain text matrix such that mod inverse of determinant exist
            inversiblePlainTextMatrixExist = createTextMatrix(plainTextTransformed, pLength, plainTextMatrix, count, keySize);

            //incase no such matrix was formed, then that keySize is skipped
            if (!inversePlainTextMatrix)
            {
                break;
                std::cout << "Failed to perform analysis for key size " << keySize << "\n";
            }

            //determinant and inverse modulus NO_OF_CHARACTERS of  determinant is evaluated for so formed plain text matrix
            det = determinant(plainTextMatrix, keySize);
            detModInv = modInverse(det, NO_OF_CHARACTERS);

            //if inverse modulus of determinant exists, then keyMatrix is evaluated and correspondingly plaintext for cipher text and IC is evaluated
            if (detModInv != -1)
            {
                std::cout << "Partial plain text matrix of size " << keySize << "x" << keySize << " created successfully\n";
                //cipher text matrix corresponding to so formed plain text is created
                std::cout << "Creating cipher text matrix of size " << keySize << "x" << keySize << "...\n";
                createTextMatrix(cipherTextTransformed, cLength, cipherTextMatrix, count, keySize);
                std::cout << "Cipher text matrix of size " << keySize << "x" << keySize << " created successfully\n";

                std::cout << "Computing key matrix of size " << keySize << "x" << keySize << "...\n";
                //plain text matrix is inversed
                inverse(plainTextMatrix, inversePlainTextMatrix, keySize);

                //key matrix is evaluated by matrix multiplication of cipher text matrix and inversed plain text matrix
                matrix_multiplication(cipherTextMatrix, inversePlainTextMatrix, keyMatrix, keySize, keySize, keySize);

                //modulus NO_OF_CHARACTERS of each element of key matrix is evaluated
                matrixMod(keyMatrix, keySize, keySize);
                std::cout << "Key matrix of size " << keySize << "x" << keySize << "computed successfully\n";

                std::cout << "Computing plain text w.r.t. key of keysize " << keySize << "...\n";
                //plain text is calculated using matrix mulitplication of inverse of key matrix formed and given cipher text
                inverse(keyMatrix, inverseKeyMatrix, keySize);

                for (i = 0; i <= cLength / keySize; ++i)
                {
                    for (j = 0; j < keySize; ++j)
                        tempCT[j] = cipherTextTransformed[keySize * i + j];

                    for (j = 0; j < keySize; ++j)
                    {
                        calcPlainText[keySize * i + j] = 0;
                        for (k = 0; k < keySize; ++k)
                            calcPlainText[keySize * i + j] += inverseKeyMatrix[j][k] * tempCT[k];
                        calcPlainText[keySize * i + j] %= 26;
                    }
                }

                //for calculated plain text in numerical form, alphabetical string is created
                calcPlainTextStr = transformToASCII(calcPlainText, cLength);
                std::cout << "Plain text w.r.t. key of keysize " << keySize << "successfully computed\n";

                //IC value for the evaluated plain text is calculated
                std::cout << "Computing IC for plaintext created w.r.t. key of keysize " << keySize << "...\n";
                ic = clacIndexOfCoincidence(calcPlainTextStr);
                std::cout << "IC for plaintext created w.r.t. key of keysize " << keySize << " = " << ic << "\n";

                //calculated IC is checked, whether it lies closest to IC of English, if yes, then that keySize and corresponding attributes are stored
                if (abs(ic - IC_ENGLISH) < minICDiff)
                {
                    std::cout << "This IC lies closest to IC(English) yet\n";
                    minIC = ic;
                    minICDiff = abs(ic - IC_ENGLISH);
                    for (i = 0; i < keySize; ++i)
                        for (j = 0; j < keySize; ++j)
                            minICDiffKeyMatrix[i][j] = keyMatrix[i][j];
                    minICDiffKeySize = keySize;
                    minICDiffPlainText = calcPlainTextStr;
                }
                else
                {
                }
            }
            else
                count++;
        } while (detModInv == -1);
    }

    //[keySize, key], plainText corresponding to the IC lying closest to the IC(English) is stored in the KEY_FILE_PATH and PLAIN_TEXT_PATH respectively
    std::cout << "IC corresponding plaintext lying closest to IC(English) = " << minIC << "\n";
    std::cout << "Key size corresponding plaintext lying closest to IC(English) = " << minICDiffKeySize << "\n";
    std::cout << "Key matrix corresponding plaintext lying closest to IC(English):\n";
    for (i = 0; i < minICDiffKeySize; ++i)
    {
        for (j = 0; j < minICDiffKeySize; ++j)
            std::cout << minICDiffKeyMatrix[i][j] << " ";
        std::cout << std::endl;
    }
    std::cout << "Storing analyzed key size and analyzed key matrix in " << ANALYZED_KEY_FILE_PATH << "...\n";
    printKey(minICDiffKeySize, minICDiffKeyMatrix);
    std::cout << "Analyzed key size and analyzed key matrix stored successfully\n";
    std::cout << "Storing Analyzed plain text in " << ANALYZED_KEY_FILE_PATH << "...\n";
    printPlainText(minICDiffPlainText);
    std::cout << "Analyzed plain text stored successfully\n";

    return 0;
}