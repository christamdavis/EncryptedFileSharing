


#include <iostream>
#include <fstream>
#include <arpa/inet.h>

#include <sstream>
#include <cstdio>
#include <vector>
#include <cstdio>
#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
extern void AESDecrypt(unsigned char * encryptedMessage, unsigned char * expandedKey, unsigned char * decryptedMessage);
extern void AESEncrypt(unsigned char * message, unsigned char * expandedKey, unsigned char * encryptedMessage);

#include "structures.h"
using namespace std;

#define MAX 4096
#define PORT 8080
#define SA struct sockaddr

const int buffSize = MAX;

void receive_file(int mySocket, ofstream& file){
    unsigned char buffer[buffSize] = {};
    int valread = read(mySocket , buffer, buffSize);

    // copy contents of buffer into string
    size_t sData_length;
    sData_length = buffSize;
    string myString;
    myString.resize(buffSize);
    for(int i = 0; i < buffSize; i++){
        myString[i] = buffer[i];
        //cout << buffer[i];
    }
    
    // custom remove trailing null characters
    int i = buffSize-1;
    while(myString[i-1] == '\0'){
        myString.pop_back();
        i--;
    }
    myString.pop_back();

    // for(int i = buffSize-1; i > 0; i--){
    //     if(myString[i-1] == '\0'){
    //         myString.pop_back();
    //     }

    // }

    cout<<"[LOG] : Data received "<<valread<<" bytes\n";
    cout<<"[LOG] : Saving data to file.\n";
    file<<myString;
    cout<<"[LOG] : File Saved.\n";



}
void transmit_file(int mySocket, ifstream& file){
    
    std::ostringstream ss;
    ss << file.rdbuf();
    std::string myStringToSend = ss.str();

    char* sData_source_;
    size_t sData_length;

    sData_length = myStringToSend.size();
    // allocate memory for sData_source
    sData_source_ = (char*)malloc(sData_length);
    // copy data to sData_source
    cout<<"out"<<endl;
    for(int i = 0; i < myStringToSend.length(); i++){
        sData_source_[i] = myStringToSend[i];
        cout << sData_source_[i];
    }
    //cout << endl;

    cout<<"[LOG] : Transmission Data Size "<<myStringToSend.length()<<" Bytes.\n";
    cout<<"[LOG] : Sending...\n";
     cout << "THIS IS THE STRING I'M SENDING: " << endl << myStringToSend << endl;
    // cout << "THIS IS THE C STRING I'M SENDING: " << endl << sData_source_ << endl;
    int bytes_sent = 
        send(mySocket , myStringToSend.c_str() , myStringToSend.length() , 0 );
    cout<<"[LOG] : Transmitted Data Size "<<bytes_sent<<" Bytes.\n";
    cout<<"[LOG] : File Transfer Complete.\n";

}

void func(int sockfd)
{
    unsigned char buff[MAX];
    int n;
    string rfs = "";
    string fileName = "";
    ifstream inFile;
    ofstream ouFile;

    //for(;;){
        // OPTION SELECT
        bzero(buff, sizeof(buff));
        printf(
            "1 : Upload\n"
            "2 : Download\n"
            "Please select an option : ");
        n = 0;
        char temp;
        while ((temp = getchar()) != '\n'){
            if(n == 0){
                buff[0] = temp;
                //buff[1] = '\n';
            }
            n++;
        }

        write(sockfd, buff, sizeof(buff));
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));

        //printf("From server: %s", buff);
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
            //printf("made it to option 1\n");
            bzero(buff, sizeof(buff));

            // get and send the file name to server
            printf("Please enter the name of the file : ");
            getline(cin, fileName);
            string originalFileName1 = fileName;
            cout<<"Filename before encrypt: "<<fileName;


            strncpy((char*) buff, fileName.c_str(), sizeof(buff) - 1);
            write(sockfd, buff, sizeof(buff));
            bzero(buff, MAX);

            // confirm that the server received the file name
            read(sockfd, buff, sizeof(buff));



            //ENCRYPT FILE (AES)
            cout<<"Encrypting File";
            string fileNameSTR = "./data/client/";
            fileNameSTR.append(fileName);
            string msgstr;
            std::ifstream infile1;
            infile1.open(fileNameSTR); //open the input file
            if (infile1.is_open()){
                std::stringstream strStream;
                strStream << infile1.rdbuf(); //read the file
                msgstr = strStream.str(); //str holds the content of the file
                infile1.close();
            } else cout << "Unable to open file\n";
            char * msg = new char[msgstr.size()+1];
            strcpy(msg, msgstr.c_str());
            char *message;
            message = msg;
            int originalLen = strlen((const char *)message);
            int paddedMessageLen = originalLen;
            if ((paddedMessageLen % 16) != 0) {
                paddedMessageLen = (paddedMessageLen / 16 + 1) * 16;
            }

            unsigned char * paddedMessage = new unsigned char[paddedMessageLen];
            for (int i = 0; i < paddedMessageLen; i++) {
                if (i >= originalLen) {
                    paddedMessage[i] = 0;
                }else {paddedMessage[i] = message[i];}
            }
            unsigned char * encryptedMessage = new unsigned char[paddedMessageLen];
            string str;
            ifstream key_infile;
            key_infile.open("keyfile",ios::in | ios::binary);
            if(key_infile.is_open()){
                getline(key_infile,str);
                key_infile.close();
            }else cout<<"Unable to open key file";
            istringstream hex_chars_stream(str);
            unsigned char key[16];
            int i = 0;
            unsigned int c;
            while(hex_chars_stream >> hex >>c){
                key[i] = c;
                i++;
            }
            unsigned char expandedKey[176];
            KeyExpansion(key, expandedKey);
            for(int i=0;i<paddedMessageLen;i+=16){
                AESEncrypt(paddedMessage+i,expandedKey,encryptedMessage+i);
            }
            string ENCfileNameSTR = "./data/client/ENC";
            ENCfileNameSTR.append(fileName);
            ENCfileNameSTR.pop_back();
            ENCfileNameSTR.pop_back();
            ENCfileNameSTR.pop_back();
            ENCfileNameSTR.append("txt");
            ofstream MyFile(ENCfileNameSTR);
            for(int i = 0;i<paddedMessageLen;i++){
                MyFile << encryptedMessage[i];
                //cout<<encryptedMessage[i];
            }
            delete[] paddedMessage;
            delete[]encryptedMessage;
            fileName = ENCfileNameSTR;
            MyFile.close();
            //FILE HAS BEEN ENCRYPTED (AES)

            std::ifstream f("ENCmyFile.txt");
cout<<"FILE HERE"<<endl;
    if (f.is_open())
        std::cout << f.rdbuf();

            // Read from the char array
            char ch2;
            ostringstream ss2;
            for(int i = 0; i < MAX; i++){
                if(buff[i] == '1' || buff[i] == '2'){
                    ch2 = buff[i];
                    ss2 << ch2;
                }
            }   
            rfs = ss.str();
            
            bzero(buff, MAX);
            if(rfs.compare("1") == 0){
                string openFile = "";
                openFile.append(fileName);
                //cout<<"file name" << openFile;


                // open the file
                inFile.open(openFile, ios::binary);
                if(inFile.is_open()){
                    cout<< "[LOG] : File is ready to Transmit.\n";
                }
                else{
                    cout<< "[ERROR] : File loading failed, Exititng.\n";
                    exit(EXIT_FAILURE);
                }
                // transmit to server
                transmit_file(sockfd, inFile);

                int n = ENCfileNameSTR.length();
                char ENCfileArr[n+1];
                strcpy(ENCfileArr, ENCfileNameSTR.c_str());
                int r = fileName.length();
                char NfileArr[r+1];
                strcpy(NfileArr, fileName.c_str());
                int one = remove(ENCfileArr); //deletes original file
                int result = remove("myFile.txt"); //deletes ENC file
            }

        // OPTION 2: DOWNLOAD
        }else if(rfs.compare("2") == 0){
            bzero(buff, sizeof(buff));
            // get the name of the file from user input
            printf("Please enter the name of the file : ");
            getline(cin, fileName);

            string originalFileName = fileName;
            string tempFileName = "ENC";
            tempFileName.append(fileName);
            fileName = tempFileName;

            //cin.getline (fileName, MAX);
            strncpy((char*) buff, fileName.c_str(), sizeof(buff) - 1);
            buff[sizeof(buff) - 1] = 0;
            // send file name to server
            write(sockfd, buff, sizeof(buff));
            bzero(buff, sizeof(buff));
            n = 0;
            // read in response
            read(sockfd, buff, sizeof(buff));

            // read in from buffer
            char ch3;
            ostringstream ss3;
            for(int i = 0; i < MAX; i++){
                if(buff[i] == '1' || buff[i] == '2'){
                    ch3 = buff[i];
                    ss3 << ch3;
                }
            }   
            rfs = ss.str();

            if (rfs.compare("2") == 0) {
                string openFile = ".//data//client//";
                openFile.append(tempFileName);

                ouFile.open(openFile, ios::trunc | ios::binary);
                if(ouFile.is_open()){
                    cout<<"[LOG] : File Created.\n";
                }
                else{
                    cout<<"[ERROR] : File creation failed, Exititng.\n";
                    exit(EXIT_FAILURE);
                }
                receive_file(sockfd, ouFile);
                ouFile.close();


                //BEGIN DECRYPT
                string fileNameSTR = "./data/client/ENC";
                fileNameSTR.append(originalFileName);
                // Read from the text file
                string msgstr; 
                char ch;
                ifstream f(fileNameSTR);
                ostringstream ss;

                while (f.get(ch)) {
                //Output the text from the file
                ss << ch;
                }   
                msgstr = ss.str();
                int n = msgstr.length();
                unsigned char * encryptedMessage = new unsigned char[n]; 

                const char* encrypted = msgstr.c_str();


                int m;
                for (m =0; m< n; m++){
                    encryptedMessage[m] = encrypted[m];	
                }
                

                //cout << "THIS IS THE STRING I'M DECRYPTING: " << endl << encryptedMessage << endl;
                
                // Read in the key
                string keystr;
                ifstream keyfile;
                keyfile.open("keyfile", ios::in | ios::binary);

                if (keyfile.is_open())
                {
                    getline(keyfile, keystr); // The first line of file should be the key
                    keyfile.close();
                }else cout << "Unable to open key file";

                istringstream hex_chars_stream(keystr);
                unsigned char key[16];
                int i = 0;
                unsigned int c;
                while (hex_chars_stream >> hex >> c)
                {
                    key[i] = c;
                    i++;
                }

                unsigned char expandedKey[176];

                KeyExpansion(key, expandedKey);

                int messageLen = m; //strlen((const char *)encryptedMessage);
                
                unsigned char * decryptedMessage = new unsigned char[messageLen];

                for (int i = 0; i < messageLen; i += 16) {
                    AESDecrypt(encryptedMessage + i, expandedKey, decryptedMessage + i);
                }

                

                fileNameSTR = "./data/client/";
                string tempName = originalFileName;
                //tempName.erase(0,3);
                fileNameSTR.append(tempName);
                //fileNameSTR.erase(fileNameSTR.size() - 3);
                //fileNameSTR.append("txt");
                cout << "Decrypted message stored in: " << fileNameSTR << endl;

                char ch2;
                ofstream dm;
                dm.open(fileNameSTR, ios::trunc);
                for (int i = 0; i < messageLen; i++) {
                    if(decryptedMessage[i] != '\0'){
                        ch2 = decryptedMessage[i];
                        dm << ch2;
                    }
                }

                dm.close();



                
            }
        }else {
            printf("Should not be here\n");
        }

        
        
    //}
}
 
int main()
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
 
    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));
 
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);
 
    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr))
        != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");
 
    // function for chat
    func(sockfd);
 
    // close the socket
    close(sockfd);
}