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

void receive_file(int mySocket, fstream& file){
    unsigned char buffer[buffSize] = {};
    int valread = read(mySocket , buffer, buffSize);
    
    cout<<"[LOG] : Data received "<<valread<<" bytes\n";

    cout << "THIS IS THE STRING I'M RECIEVING: " << endl << buffer << endl;

    cout<<"[LOG] : Saving data to file.\n";

    file<<buffer;
    cout<<"[LOG] : File Saved.\n";
}
void transmit_file(int mySocket, fstream& file){
    string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    cout<<"[LOG] : Transmission Data Size "<<contents.length()<<" Bytes.\n";

    cout<<"[LOG] : Sending...\n";

    int bytes_sent = send(mySocket , contents.c_str() , contents.length() , 0 );
    cout<<"[LOG] : Transmitted Data Size "<<bytes_sent<<" Bytes.\n";

    cout<<"[LOG] : File Transfer Complete.\n";
}

// Function designed for chat between client and server.
void func(int connfd)
{
    fstream file;
    unsigned char buff[MAX];
    int n;
    string rfs = "";
    string fileName = "";

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

            bzero(buff, MAX);
            // confirm to client that the name was recieved
            buff[0] = '1';
            write(connfd, buff, sizeof(buff));
            bzero(buff, MAX);

            // recieve the file
            string openFile = ".//data//server//";
            openFile.append(fileName);

            file.open(openFile, ios::out | ios::trunc | ios::binary);
            if(file.is_open()){
            cout<<"[LOG] : File Created.\n";
            }
            else{
            cout<<"[ERROR] : File creation failed, Exititng.\n";
            exit(EXIT_FAILURE);
            }
            receive_file(connfd, file);

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
            file.open(openFile, ios::in | ios::binary);
            if(file.is_open()){
                cout<<"[LOG] : File is ready to Transmit.\n";
            }
            else{
                cout<<"[ERROR] : File loading failed, Exititng.\n";
                exit(EXIT_FAILURE);
            }
            // transmit to client
            transmit_file(connfd, file);

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