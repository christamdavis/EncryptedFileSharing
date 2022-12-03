#include <iostream>
#include <fstream>
#include <arpa/inet.h>

#include <sstream>
#include <cstdio>

#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()

using namespace std;

#define MAX 4096
#define PORT 8080
#define SA struct sockaddr
fstream file;

const int buffSize = MAX;


void receive_file(int mySocket, fstream& file){
    unsigned char buffer[buffSize] = {};
    int valread = read(mySocket , buffer, buffSize);

    cout<<"[LOG] : Data received "<<valread<<" bytes\n";
    cout<<"[LOG] : Saving data to file.\n";

    file<<buffer;
    cout<<"[LOG] : File Saved.\n";
}
void transmit_file(int mySocket, fstream& file){
    string contents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    cout<<"[LOG] : Transmission Data Size "<<contents.length()<<" Bytes.\n";

    cout<<"[LOG] : Sending...\n";

    cout << "THIS IS THE STRING I'M SENDING: " << endl << contents.c_str() << endl;
    
    int bytes_sent = send(mySocket , contents.c_str() , contents.length() , 0 );
    cout<<"[LOG] : Transmitted Data Size "<<bytes_sent<<" Bytes.\n";

    cout<<"[LOG] : File Transfer Complete.\n";
}

void func(int sockfd)
{
    unsigned char buff[MAX];
    int n;
    string rfs = "";
    string fileName = "";

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

        //cout << "RFS: " << rfs;
        //cout << "rfs == 1? " << (rfs.compare("1")==0) << endl;

        bzero(buff, MAX);
        // OPTION 1: UPLOAD
        if((rfs.compare("1") == 0)){
            //printf("made it to option 1\n");
            bzero(buff, sizeof(buff));

            // get and send the file name to server
            printf("Please enter the name of the file : ");
            getline(cin, fileName);
            strncpy((char*) buff, fileName.c_str(), sizeof(buff) - 1);
            write(sockfd, buff, sizeof(buff));
            bzero(buff, MAX);

            // confirm that the server received the file name
            read(sockfd, buff, sizeof(buff));

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
            
            //cout << "RFS: " << rfs;
            //cout << "rfs == 1? " << (rfs.compare("1")==0) << endl;

            bzero(buff, MAX);
            if(rfs.compare("1") == 0){
                string openFile = ".//data//client//";
                openFile.append(fileName);

                // open the file
                file.open(openFile, ios::in | ios::binary);
                if(file.is_open()){
                    cout << "[LOG] : File is ready to Transmit.\n";
                }
                else{
                    cout << "[ERROR] : File loading failed, Exititng.\n";
                    exit(EXIT_FAILURE);
                }
                // transmit to server
                transmit_file(sockfd, file);
            }

        // OPTION 2: DOWNLOAD
        }else if(rfs.compare("2") == 0){
            bzero(buff, sizeof(buff));
            // get the name of the file from user input
            printf("Please enter the name of the file : ");
            getline(cin, fileName);
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

            cout << "RFS: " << rfs;
            cout << "rfs == 2? " << (rfs.compare("2")==0) << endl;

            if (rfs.compare("2") == 0) {
                string openFile = ".//data//client//";
                openFile.append(fileName);
                file.open(openFile, ios::out | ios::trunc | ios::binary);
                if(file.is_open()){
                cout<<"[LOG] : File Created.\n";
                }
                else{
                cout<<"[ERROR] : File creation failed, Exititng.\n";
                exit(EXIT_FAILURE);
                }
                receive_file(sockfd, file);
            }
        }else {
            printf("Should not be here\n");
        }

        
        
    //}
    




    // for (;;) {
    //     bzero(buff, sizeof(buff));
    //     printf("Enter the string : ");
    //     n = 0;
    //     while ((buff[n++] = getchar()) != '\n')
    //         ;
    //     write(sockfd, buff, sizeof(buff));
    //     bzero(buff, sizeof(buff));
    //     read(sockfd, buff, sizeof(buff));
    //     printf("From Server : %s", buff);
    //     if ((strncmp(buff, "exit", 4)) == 0) {
    //         printf("Client Exit...\n");
    //         break;
    //     }
    // }
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