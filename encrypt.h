#ifndef ENCRYPT_H
#define ENCRYPT_H

#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>

void EAddRoundKey(unsigned char * state, unsigned char * roundKey);
void ESubBytes(unsigned char * state);
void EShiftRows(unsigned char * state);
void EMixColumns(unsigned char * state);
void ERound(unsigned char * state, unsigned char * key);
void EFinalRound(unsigned char * state, unsigned char * key);
void AESEncrypt(unsigned char * message, unsigned char * expandedKey, unsigned char * encryptedMessage);

#endif