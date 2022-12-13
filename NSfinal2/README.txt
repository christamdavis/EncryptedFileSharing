Encrypted-File-Storage
Scott Gordon, Than Lim

This program was developed as the course project for COMP 5610.

STRUCTURE: 
A standard text file is encrypted using AES 128-bit encryption.
The encrypted file is then sent via socket communication to the server.
The server stores the encrypted file.
The user requests the file back by filename.
The server sends the file back to the user.
The user can decrypt the file using AES 128-bit decryption.

The work here is built off of the AES implementation provided by Cecelia Wisniewska.
The original work can be found here: https://github.com/ceceww/aes

We are also providing a funcioning RSA demo as a part of our submission.
The original goal was to encorperate this into our project, but we could not
get the pieces working together due to some issues.
We still believe the effort we put into the RSA demo was worth submitting as
its own example.


COMPILATION INSTRUCTIONS: 
-copy the entire directory onto your local machine
-open terminal and change the directory to the folder containing the source code
-open a second terminal to the same location to serve as the server.
-any files you wish to encrypt or store should be in the folder './data/client/'.
-run these commands to cmopile each of the source code files:
    g++ client.cpp -o ./client  
    g++ server.cpp -o ./server  
    g++ ./aes/encrypt.cpp -o ./aes/encrypt  
    g++ ./aes/decrypt.cpp -o ./aes/decrypt  
    g++ ./rsaDemo/rsaDemo.cpp -o ./rsaDemo/rsaDemo  
  
RUNNING INSTRUCTIONS:  
-run these commands and follow given instructions to run the program  
-when prompted for file names, it is assumed the file is in './data/client/'.  
-you must run the server before you run the client  
    on client terminal: ./aes/encrypt  
        enter the name of the file you wish to encrypt.  
        resulting filename will have 'ENC' prepended.  
    on server terminal: ./server  
    on client terminal: ./client  
        choose to upload file, then enter file name of the encrypted file.  
    you may delete the file form the client folder.  
        on server terminal: ./server  
    on client terminal: ./client  
        choose to download file, then enter file name of the encrypted file.  
    on client terminal: ./aes/decrypt  
        enter the name of the encrypted file to decrypt.  
        resulting filename will remove the 'ENC' prepend.  

The rsa demo is unrelated due to difficulties implementing, but to run you use this command: 
    ./rsaDemo/rsaDemo  
        You may adjust the message directly in the code.  
        The message must be at max 3 characters to function correctly due to its implementation.  
