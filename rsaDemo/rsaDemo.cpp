#include <iostream>
#include <fstream>
#include <arpa/inet.h>

#include <sstream>

#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // read(), write(), close()

#include <math.h>

using namespace std;

#define MAX 4096

double pubKey;
double priKey;
double rsaN;
string message;

// function to convert decimal to hexadecimal
string decToHexa(int n)
{
    char hexaDeciNum[100];
    
    int i = 0;
    while (n != 0) {
        // remainder
        int temp = 0;
        temp = n % 16;
        if (temp < 10) {
            hexaDeciNum[i] = temp + 48;
            i++;
        }
        else {
            hexaDeciNum[i] = temp + 55;
            i++;
        }
        n = n / 16;
    }
    string ans = "";
    // result is in reverse order
    for (int j = i - 1; j >= 0; j--)
        ans += hexaDeciNum[j];
    return ans;
}

// Function to convert ASCII to HEX
string ASCIItoHEX(string ascii)
{
    string hex = "";
    for (int i = 0; i < ascii.length(); i++) {
        char ch = ascii[i];
        int tmp = (int)ch;
        string part = decToHexa(tmp);
        hex += part;
    }
    return hex;
}

// find gcd
int gcd(int a, int b)
{
    int t;
    while (1)
    {
        t = a % b;
        if (t == 0){
            return b;
        }
        a = b;
        b = t;
    }
}

void initializeRSA(){
    // 2 random prime numbers
    double p = 199933;
    double q = 7907;

    double n = p * q; // calculate n
    double track;
    double phi = (p - 1) * (q - 1); // calculate phi
    double e = 7; // to be public key
    
    //for checking that 1 < e < phi(n) and gcd(e, phi(n)) = 1; i.e., e and phi(n) are coprime.
    while (e < phi)
    {
        track = gcd(e, phi);
        if(track == 1){
            break;
        }
        else{
            e++;
        }
    }
    
    //choosing d such that it satisfies d*e = 1 mod phi
    double d1 = 1 / e;
    double d = fmod(d1, phi);

    rsaN = n; // rsa N
    pubKey = e; // rsa public key
    priKey = d; // rsa private key

    return;
}

double stringTointRSA(unsigned char* rsaBuff){
    // Read from the char array
    string input;
    char ch2;
    ostringstream ss2;
    for(int i = 0; i < MAX; i++){
        ch2 = rsaBuff[i];
        ss2 << ch2;
    }   
    input = ss2.str();
    
    // convert hex string to its numerical representation
    // char array to hex string
    string hexInput;
    hexInput = ASCIItoHEX(input);
    // hex string to int
    double myint = stol(hexInput,nullptr,16);
    return myint;
}

unsigned char* intToStringRSA(long rsaint){
    // convert the int to hex string
    stringstream stream;
    stream << hex << rsaint;
    string hexInput(stream.str());

    // convert integer to its string representation
    int len = hexInput.length();
    string newString;
    for(int i=0; i < len; i+=2)
    {
        string byte = hexInput.substr(i,2);
        char chr = (char) (int)strtol(byte.c_str(), NULL, 16);
        newString.push_back(chr);
    }
    static unsigned char myBuff[MAX];
    strncpy((char*) myBuff, newString.c_str(), sizeof(myBuff));

    //cout << "Resulting string: " << newString << endl;

    return myBuff;
}

double encryptRSA(unsigned char* rsaBuff, size_t mySize){

    // character string to double
    double messageDB = stringTointRSA(rsaBuff);

    // encrypt
    double c = pow(messageDB, pubKey);
    
    return c;
}
unsigned char* decryptRSA(double encMessage){

    // decrypt message
    double decMessage =  pow(encMessage, priKey);
    decMessage = fmod(decMessage, rsaN);

    // double to character string
    unsigned char* buff2 = intToStringRSA(round(decMessage));

    return buff2;
}


int main()
{
    unsigned char buff[MAX];
    initializeRSA();
    message = "Net";

    // output available data
    cout << endl << "INPUT\n";
    cout << "N: " << rsaN << endl;
    cout << "pubKey: " << pubKey << endl;
    cout << "priKey: " << priKey << endl;
    cout << endl;
    cout << "Original message: " << message << endl;

    // encrypt the message
    strncpy((char*) buff, message.c_str(), sizeof(buff) - 1);
    double encMessage = encryptRSA(buff, sizeof(buff) - 1);

    cout << "Encrypted Message: " << encMessage << endl;

    // decrypt the message
    unsigned char* decMessage = decryptRSA(encMessage);
    // Read from the char array
    char ch2;
    ostringstream ss2;
    for(int i = 0; i < MAX; i++){
        ch2 = decMessage[i];
        ss2 << ch2;
        //cout << ch2;
    }   
    string message2 = ss2.str();

    cout << "Decrypted Message: " << message2 << endl;

    cout << endl;
    return 0;
}