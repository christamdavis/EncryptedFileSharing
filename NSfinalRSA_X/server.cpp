#include <iostream>
#include <fstream>
#include <arpa/inet.h>

#include <sstream>
#include <cstdio>
#include <vector>
#include <tgmath.h>

#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // read(), write(), close()

using namespace std;

#define MAX 4096
#define PORT 8080
#define SA struct sockaddr

const int buffSize = MAX;

/* RSA */
double pubKey;
double priKey;
double rsaN;
double pubKeyC;
double rsaNC;

// function to convert decimal to hexadecimal
string decToHexa(int n)
{
    // char array to store hexadecimal number
    char hexaDeciNum[100];
    // counter for hexadecimal number array
    int i = 0;
    while (n != 0) {
        // temporary variable to store remainder
        int temp = 0;
        // storing remainder in temp variable.
        temp = n % 16;
        // check if temp < 10
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
    // printing hexadecimal number array in reverse order
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
    // int p = 13;
    // int q = 11;
    double p = 193939;
    double q = 7901;


    double n = p * q; // calculate n
    double track;
    double phi = (p - 1) * (q - 1); // calculate phi
    double e = 11; // to be public key
    
    //for checking that 1 < e < phi(n) and gcd(e, phi(n)) = 1; i.e., e and phi(n) are coprime.
    while (e < phi)
    {
        track = gcd(e, phi);
        if (track == 1)
            break;
        else
            e++;
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
    string input;
    // Read from the char array
    char ch2;
    ostringstream ss2;
    for(int i = 0; i < MAX; i++){
        ch2 = rsaBuff[i];
        ss2 << ch2;
    }   
    input = ss2.str();
    //cout << "STRINGtoINT string: " << input << endl;
    string hexInput;
    hexInput = ASCIItoHEX(input);

    //cout << "HEX RESULT: " << hexInput << endl;

    // hex to int
    double myint = stol(hexInput,nullptr,16);

    //cout << "MY INT: " << myint << endl;
    
    return myint;
}

unsigned char* intToStringRSA(long rsaint){
    // convert the int to hex string
    stringstream stream;
    stream << hex << rsaint;
    string hexInput(stream.str());

    //cout << "Input to conversion back: " << rsaint << endl;
    //cout << "Input after conversion to hex: " << hexInput << endl;

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

double encryptRSA(unsigned char* rsaBuff){

    //cout << "Message given to encrypt function: ";
    // for(int i = 0; i < mySize; i++){
    //     cout << rsaBuff[i];
    // }
    // cout << endl;

    // character string to double
    double messageDB = stringTointRSA(rsaBuff);

    //cout << "Plain message in number form: " << messageDB << endl;

    // encrypt
    double c = pow(messageDB, pubKeyC);
    //c = fmod(c, rsaN);

    // double back to character string
    // string doubleSTR = to_string(c);
    // unsigned char encMessage = doubleSTR.c_str();

    //cout << "Message leaving encrypt function: " << c << endl;
    return c;
}
unsigned char* decryptRSA(double encMessage){

    ///cout << "Message given to decrypt function: " << encMessage << endl;

    // decrypt message
    double decMessage =  pow(encMessage, priKey);
    //cout << "INTERMEDIATE STEP: " << decMessage << endl;
    decMessage = fmod(decMessage, rsaN);

    //cout << "Decrypted Message still in number form: " << decMessage << endl;
    //cout << "Decrypted Message still in number form: " << (long)round(decMessage) << endl;
    
    // double to character string
    unsigned char* buff2 = intToStringRSA(round(decMessage));

    return buff2;
}

/* MESSAGE SENDING AND RECEIVING */

int sendData(int mySocket, unsigned char (& buff)[MAX], size_t mySize){
    //write(sockfd, buff, sizeof(buff));

    // send how many messages to send
    int sizeCounter = ceil((double)mySize/((double)2));
    double myChar[1];
    myChar[0] = (double)sizeCounter;
    write(mySocket, myChar, (8));
    
    cout << "sizeCounter: " << sizeCounter << endl;

    double encDouble;
    // send the messages
    while(sizeCounter > 0){
        unsigned char s[2];
        s[0] = buff[0];
        s[1] = buff[1];
        encDouble = encryptRSA(s);
        myChar[0] = encDouble;
        write(mySocket, myChar, (8));
        for(int i = 2; i < MAX; i++)
        {
            buff[i-2] = buff[i];
        }
        sizeCounter--;
    }

    return (int)mySize;








    // OLD WAY pre-RSA

    // send how many messages to send
    // int sizeCounter = ceil((double)mySize/((double)4));
    // double myChar[1];
    // myChar[0] = (double)sizeCounter;
    // write(mySocket, myChar, (8));

    // //cout << "sizeCounter: " << sizeCounter << endl;
    // // send the messages
    // while(sizeCounter > 0){
    //     write(mySocket, buff, (4));
    //     for(int i = 4; i < MAX; i++)
    //     {
    //         buff[i-4] = buff[i];
    //     }
    //     sizeCounter--;
    // }
    // //write(mySocket, &buff, (4));
    // return (int)mySize;
}
int receiveData(int mySocket, unsigned char (& buff)[MAX], size_t mySize){
    //read(sockfd, buff, sizeof(buff));

    // read how many messages to receive
    double myChar[1];
    unsigned char interBuffer[2];
    read(mySocket,&myChar,(8));
    int sizeCounter = myChar[0];

    cout << "sizeCounter: " << sizeCounter << endl;

    // read the messages
    int i = 0;
    while(sizeCounter > 0){
        bzero(myChar, sizeof(myChar));
        read(mySocket,myChar,(8));

        *interBuffer = *decryptRSA(myChar[0]);
        
        for(int j = 0; j < 2; j++){
            buff[i+j] = interBuffer[j];
            //cout << "I+J: " << (i+j) << endl;
        }
        i+=2;
        sizeCounter--;
    }
    return (int)(myChar[0] * (2));







    // OLD WAY pre-RSA

    // read how many messages to receive
    // double myChar[1];
    // unsigned char interBuffer[4];
    // read(mySocket,&myChar,(8));

    // // read the messages
    // int sizeCounter = myChar[0];
    // int i = 0;
    // //cout << "sizeCounter: " << sizeCounter << endl;
    // while(sizeCounter > 0){
    //     bzero(interBuffer, sizeof(interBuffer));
    //     read(mySocket,&interBuffer,(4));
    //     for(int j = 0; j < 4; j++){
    //         buff[i+j] = interBuffer[j];
    //         //cout << "I+J: " << (i+j) << endl;
    //     }
    //     i+=4;
    //     sizeCounter--;
    // }
    // return (int)(myChar[0] * (4));
}

void receive_file(int mySocket, ofstream& file){
    unsigned char buffer[buffSize] = {};
    //int valread = read(mySocket , buffer, buffSize);
    int valread = receiveData(mySocket , buffer, buffSize);

    // copy contents of buffer into string
    size_t sData_length;
    sData_length = buffSize;
    string myString;
    myString.resize(buffSize);
    for(int i = 0; i < buffSize; i++){
        myString[i] = buffer[i];
    }

    // custom remove trailing null characters
    int i = buffSize-1;
    while(myString[i-1] == '\0'){
        myString.pop_back();
        i--;
    }
    myString.pop_back();

    cout<<"[LOG] : Data received "<<valread<<" bytes\n";
    cout<<"[LOG] : Saving data to file.\n";
    file<<myString;
    cout<<"[LOG] : File Saved.\n";
}
void transmit_file(int mySocket, ifstream& file){
    
    std::ostringstream ss;
    ss << file.rdbuf();
    std::string myStringToSend = ss.str();

    unsigned char sData_source_[MAX];
    size_t sData_length;

    sData_length = myStringToSend.length();
    // allocate memory for sData_source
    //sData_source_ = malloc(sData_length);
    // copy data to sData_source
    for(int i = 0; i < myStringToSend.length(); i++){
        sData_source_[i] = myStringToSend[i];
    }

    cout<<"[LOG] : Sending...\n";
    // cout << "THIS IS THE STRING I'M SENDING: " << endl << myStringToSend << endl;
    // cout << "THIS IS THE STRING I'M SENDING (c_str): " << endl << sData_source_ << endl;

    //int bytes_sent = send(mySocket , myStringToSend.c_str() , myStringToSend.length() , 0 );
    int bytes_sent = sendData(mySocket , sData_source_ , sData_length);
    
    cout<<"[LOG] : Transmitted Data Size "<<bytes_sent<<" Bytes.\n";
    cout<<"[LOG] : File Transfer Complete.\n";

}

/* PRIMARY PROGRAM FLOW */

void func(int connfd)
{
    unsigned char buff[MAX];
    int n;
    string rfs = "";
    string fileName = "";
    ifstream inFile;
    ofstream ouFile;

    bzero(buff, MAX);

    // EXCHANGE KEYS
    // read the client N
    read(connfd, buff, sizeof(buff));
    string s = "";
    for (int i = 0; i < MAX; i++) {
        s = s + (char)buff[i];
    }
    rsaNC = stod(s);
    bzero(buff, MAX);
    // read the client public key
    read(connfd, buff, sizeof(buff));
    s = "";
    for (int i = 0; i < MAX; i++) {
        s = s + (char)buff[i];
    }
    pubKeyC = stod(s);
    bzero(buff, MAX);

    // send the server N
    s = "";
    s = to_string(rsaN);
    for (int i = 0; i < s.length(); i++) {
        buff[i] = s[i];
    }
    write(connfd, buff, sizeof(buff));
    bzero(buff, MAX);
    // send the server public key
    s = "";
    s = to_string(pubKey);
    for (int i = 0; i < s.length(); i++) {
        buff[i] = s[i];
    }
    write(connfd, buff, sizeof(buff));
    bzero(buff, MAX);

    // cout << "[LOG] : Keys exchanged.\n";
    // cout << "server N: " << rsaN << endl;
    // cout << "server public key: " << pubKey << endl;
    // cout << "server private key: " << priKey << endl;
    // cout << "client N: " << rsaNC << endl;
    // cout << "client public key: " << pubKeyC << endl;

    // read the message from client and copy it in buffer
    //read(connfd, buff, sizeof(buff));
    receiveData(connfd, buff, sizeof(buff));

    // Read from the char array
    char ch;
    ostringstream ss;
    for(int i = 0; i < MAX; i++){
        if(buff[i] == '1' || buff[i] == '2'){
            ch = buff[i];
            ss << ch;
        }
    }   
    rfs = ss.str();
    
    bzero(buff, MAX);
    // OPTION 1: UPLOAD
    if((rfs.compare("1") == 0)){
        printf("[LOG] : Option 1 selected...\n");
        buff[0] = '1';
        //write(connfd, buff, sizeof(buff));
        sendData(connfd, buff, sizeof(buff));
        bzero(buff, MAX);

        // read in the file name from client
        printf("[LOG] : Waiting for file selection...\n");
        //read(connfd, buff, sizeof(buff));
        receiveData(connfd, buff, sizeof(buff));

        // Read from the char array
        char ch2;
        ostringstream ss2;
        for(int i = 0; i < MAX; i++){
            ch2 = buff[i];
            ss2 << ch2;
        }   
        fileName = ss2.str();

        bzero(buff, MAX);
        // confirm to client that the name was recieved
        buff[0] = '1';
        //write(connfd, buff, sizeof(buff));
        sendData(connfd, buff, sizeof(buff));
        bzero(buff, MAX);

        // recieve the file
        string openFile = ".//data//server//";
        openFile.append(fileName);

        ouFile.open(openFile, ios::trunc | ios::binary);
        if(ouFile.is_open()){
            cout<< "[LOG] : File Created.\n";
        }
        else{
            cout<< "[ERROR] : File creation failed, Exititng.\n";
            exit(EXIT_FAILURE);
        }
        receive_file(connfd, ouFile);

        // OPTION 2: DOWNLOAD
    }else if(rfs.compare("2") == 0){
        printf("[LOG] : Option 2 selected...\n");
        buff[0] = '2';
        //write(connfd, buff, sizeof(buff));
        sendData(connfd, buff, sizeof(buff));
        bzero(buff, MAX);
        // read the requested file name from client
        printf("[LOG] : Requesting file name...\n");
        //read(connfd, buff, sizeof(buff));
        receiveData(connfd, buff, sizeof(buff));

        // Read from the char array
        unsigned char ch3;
        ostringstream ss3;

        for(int i = 0; i < MAX; i++){
            ch3 = buff[i];
            ss3 << ch3;
        }   
        fileName = ss3.str();
        bzero(buff, MAX);
        buff[0] = '2';
        //write(connfd, buff, sizeof(buff));
        sendData(connfd, buff, sizeof(buff));
        bzero(buff, MAX);

        string openFile = ".//data//server//";
        openFile.append(fileName);

        // open the file
        inFile.open(openFile, ios::binary);
        if(inFile.is_open()){
            cout<<"[LOG] : File is ready to Transmit.\n";
        }
        else{
            cout<<"[ERROR] : File loading failed, Exititng.\n";
            exit(EXIT_FAILURE);
        }
        // transmit to client
        transmit_file(connfd, inFile);
        
    }else{
        printf("[LOG] : Should not be here\n");
    }
}

/* DRIVER */

int main()
{
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;
    initializeRSA();

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1) {
        printf("[LOG] : socket creation failed...\n");
        exit(0);
    }
    else{
        printf("[LOG] : Socket successfully created..\n");
    }
    bzero(&servaddr, sizeof(servaddr));
   
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
   
    // Binding newly created socket to given IP and verification
    if((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("[ERROR] : socket bind failed...\n");
        exit(0);
    }
    else{
        printf("[LOG] : Socket successfully binded..\n");
    }
    // Now server is ready to listen and verification
    if((listen(sockfd, 5)) != 0) {
        printf("[ERROR] : Listen failed...\n");
        exit(0);
    }
    else{
        printf("[LOG] : Server listening..\n");
    }
    len = sizeof(cli);

    // Accept the data packet from client and verification
    connfd = accept(sockfd, (sockaddr *)&cli, (socklen_t*) &len);
    if(connfd < 0) {
        printf("[ERROR] : server accept failed...\n");
        exit(0);
    }
    else{
        printf("[LOG] : server accept the client...\n");
    }
    
    // Function for chatting between client and server
    func(connfd);
   
    // After chatting close the socket
    close(sockfd);
}