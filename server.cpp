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

    // custom remove trailing null characters
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
    for(int i = 0; i < myStringToSend.length(); i++){
        sData_source_[i] = myStringToSend[i];
        //cout << sData_source_[i];
    }
    //cout << endl;

    cout<<"[LOG] : Transmission Data Size "<<myStringToSend.length()<<" Bytes.\n";
    cout<<"[LOG] : Sending...\n";
    //cout << "THIS IS THE STRING I'M SENDING: " << endl << myStringToSend << endl;
    // cout << "THIS IS THE STRING I'M SENDING (c_str): " << endl << sData_source_ << endl;
    int bytes_sent = send(mySocket , myStringToSend.c_str() , myStringToSend.length() , 0 );
    cout<<"[LOG] : Transmitted Data Size "<<bytes_sent<<" Bytes.\n";
    cout<<"[LOG] : File Transfer Complete.\n";

}

void func(int connfd)
{
    unsigned char buff[MAX];
    int n;
    string rfs = "";
    string fileName = "";
    ifstream inFile;
    ofstream ouFile;

    //for(;;){
        bzero(buff, MAX);

        
        // read the message from client and copy it in buffer
        read(connfd, buff, sizeof(buff));
        //printf("From client: %s\n", buff);

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

        // cout << "RFS: " << rfs << endl;
        // cout << "\"1\": " << "1" << endl;
        // cout << "rfs == 1: " << (rfs=="1") << endl;

        
        bzero(buff, MAX);
        // OPTION 1: UPLOAD
        if((rfs.compare("1") == 0)){
            printf("[LOG] : Option 1 selected...\n");
            buff[0] = '1';
            write(connfd, buff, sizeof(buff));
            bzero(buff, MAX);

            // read in the file name from client
            printf("[LOG] : Waiting for file selection...\n");
            read(connfd, buff, sizeof(buff));

            // Read from the char array
            char ch2;
            ostringstream ss2;
            for(int i = 0; i < MAX; i++){
                ch2 = buff[i];
                ss2 << ch2;
            }   
            fileName = ss2.str();
            cout<<"server received filename"<<fileName;

            bzero(buff, MAX);
            // confirm to client that the name was recieved
            buff[0] = '1';
            write(connfd, buff, sizeof(buff));
            bzero(buff, MAX);

            // recieve the file
            string openFile = ".//data//server//ENC";
            openFile.append(fileName);
            cout<<"filename is now:" <<openFile;

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
            write(connfd, buff, sizeof(buff));
            bzero(buff, MAX);
            // read the requested file name from client
            printf("[LOG] : Requesting file name...\n");
            read(connfd, buff, sizeof(buff));

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
            write(connfd, buff, sizeof(buff));
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

    //}
}

// Driver function
int main()
{
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;
   
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