Encrypted-File-Storage
Computer and Network Security Course Project - FALL 2022
Scott Gordon, Christa Davis, and Than Lim

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

We are also providing a functioning RSA demo as a part of our submission.
The original goal was to incorporate this into our project, but we could not
get the pieces working together due to some issues.
We still believe the effort we put into the RSA demo was worth submitting as
its own example.


COMPILATION INSTRUCTIONS:
-copy the entire directory onto your local machine
-open terminal and change the directory to the folder containing the source code
-open a second terminal to the same location to serve as the server.
-any files you wish to send to the server should be in the folder './data/client/'.
-run these commands to compile each of the source code files:
    make
    g++ ./rsaDemo/rsaDemo.cpp -o ./rsaDemo/rsaDemo
-run this command to remove all object and executable files if you want to recompile
  after making edits
    make clean

RUNNING INSTRUCTIONS:
-run these commands and follow given instructions to run the program
-when prompted for file names, it is assumed the file is in './data/client/'.
-you must run the server before you run the client
    on server terminal: ./server
    on client terminal: ./client
        enter 1 or 2 depending if you want to upload or download
        enter the name of the file you wish to upload or download (note that on
        the server, the file name will begin with ENC, but you only need to
        type the original file name to download and upload)
        Once it has been uploaded, you can delete the file off the client. Once
        a file has been downloaded, you can delete the ENC file from the server.

The rsa demo is unrelated due to difficulties implementing, but to run you use this command:
    ./rsaDemo/rsaDemo
        You may adjust the message directly in the code.
        The message must be at max 3 characters to function correctly due to its implementation.
