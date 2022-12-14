#ifndef DECRYPT_H
#define DECRYPT_H
#include <iostream>
#include <cstring>
#include <fstream>
#include <sstream>
#include <string> 

void DSubRoundKey(unsigned char * state, unsigned char * roundKey);
void DInverseMixColumns(unsigned char * state);
void DShiftRows(unsigned char * state);
void DSubBytes(unsigned char * state);
void DRound(unsigned char * state, unsigned char * key);
void DInitialRound(unsigned char * state, unsigned char * key);
void AESDecrypt(unsigned char * encryptedMessage, unsigned char * expandedKey, unsigned char * decryptedMessage);

#endif


