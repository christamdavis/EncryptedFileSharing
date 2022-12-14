// Scott Gordon
// performs encryption using AES 128-bit
// implemented by modifying the work of @author Cecelia Wisniewska

#include "encrypt.h"
#include "structures.h"

using namespace std;

/* Serves as the initial round during encryption
 * AddRoundKey is simply an XOR of a 128-bit block with the 128-bit key.
 */
void EAddRoundKey(unsigned char * state, unsigned char * roundKey) {
	for (int i = 0; i < 16; i++) {
		state[i] ^= roundKey[i];
	}
}

/* Perform substitution to each of the 16 bytes
 * Uses S-box as lookup table
 */
void ESubBytes(unsigned char * state) {
	for (int i = 0; i < 16; i++) {
		state[i] = s[state[i]];
	}
}

// Shift left, adds diffusion
void EShiftRows(unsigned char * state) {
	unsigned char tmp[16];

	/* Column 1 */
	tmp[0] = state[0];
	tmp[1] = state[5];
	tmp[2] = state[10];
	tmp[3] = state[15];

	/* Column 2 */
	tmp[4] = state[4];
	tmp[5] = state[9];
	tmp[6] = state[14];
	tmp[7] = state[3];

	/* Column 3 */
	tmp[8] = state[8];
	tmp[9] = state[13];
	tmp[10] = state[2];
	tmp[11] = state[7];

	/* Column 4 */
	tmp[12] = state[12];
	tmp[13] = state[1];
	tmp[14] = state[6];
	tmp[15] = state[11];

	for (int i = 0; i < 16; i++) {
		state[i] = tmp[i];
	}
}

 /* MixColumns uses mul2, mul3 look-up tables
  * Source of diffusion
  */
void EMixColumns(unsigned char * state) {
	unsigned char tmp[16];

	tmp[0] = (unsigned char) mul2[state[0]] ^ mul3[state[1]] ^ state[2] ^ state[3];
	tmp[1] = (unsigned char) state[0] ^ mul2[state[1]] ^ mul3[state[2]] ^ state[3];
	tmp[2] = (unsigned char) state[0] ^ state[1] ^ mul2[state[2]] ^ mul3[state[3]];
	tmp[3] = (unsigned char) mul3[state[0]] ^ state[1] ^ state[2] ^ mul2[state[3]];

	tmp[4] = (unsigned char)mul2[state[4]] ^ mul3[state[5]] ^ state[6] ^ state[7];
	tmp[5] = (unsigned char)state[4] ^ mul2[state[5]] ^ mul3[state[6]] ^ state[7];
	tmp[6] = (unsigned char)state[4] ^ state[5] ^ mul2[state[6]] ^ mul3[state[7]];
	tmp[7] = (unsigned char)mul3[state[4]] ^ state[5] ^ state[6] ^ mul2[state[7]];

	tmp[8] = (unsigned char)mul2[state[8]] ^ mul3[state[9]] ^ state[10] ^ state[11];
	tmp[9] = (unsigned char)state[8] ^ mul2[state[9]] ^ mul3[state[10]] ^ state[11];
	tmp[10] = (unsigned char)state[8] ^ state[9] ^ mul2[state[10]] ^ mul3[state[11]];
	tmp[11] = (unsigned char)mul3[state[8]] ^ state[9] ^ state[10] ^ mul2[state[11]];

	tmp[12] = (unsigned char)mul2[state[12]] ^ mul3[state[13]] ^ state[14] ^ state[15];
	tmp[13] = (unsigned char)state[12] ^ mul2[state[13]] ^ mul3[state[14]] ^ state[15];
	tmp[14] = (unsigned char)state[12] ^ state[13] ^ mul2[state[14]] ^ mul3[state[15]];
	tmp[15] = (unsigned char)mul3[state[12]] ^ state[13] ^ state[14] ^ mul2[state[15]];

	for (int i = 0; i < 16; i++) {
		state[i] = tmp[i];
	}
}

/* Each round operates on 128 bits at a time
 * The number of rounds is defined in AESEncrypt()
 */
void ERound(unsigned char * state, unsigned char * key) {
	ESubBytes(state);
	EShiftRows(state);
	EMixColumns(state);
	EAddRoundKey(state, key);
}

 // Same as Round() except it doesn't mix columns
void EFinalRound(unsigned char * state, unsigned char * key) {
	ESubBytes(state);
	EShiftRows(state);
	EAddRoundKey(state, key);
}

/* The AES encryption function
 * Organizes the confusion and diffusion steps into one function
 */
void AESEncrypt(unsigned char * message, unsigned char * expandedKey, unsigned char * encryptedMessage) {
	unsigned char state[16]; // Stores the first 16 bytes of original message

	for (int i = 0; i < 16; i++) {
		state[i] = message[i];
	}

	int numberOfRounds = 9;

	EAddRoundKey(state, expandedKey); // Initial round

	for (int i = 0; i < numberOfRounds; i++) {
		ERound(state, expandedKey + (16 * (i+1)));
	}

	EFinalRound(state, expandedKey + 160);

	// Copy encrypted state to buffer
	for (int i = 0; i < 16; i++) {
		encryptedMessage[i] = state[i];
	}
}

/*
int main() {

	// get the name of the file to encrypt
	char fileName[1024];
	cout << "Enter file name to encrypt: ";
	
	cin.getline(fileName, sizeof(fileName));

	cout << "Working on: " << fileName << endl;
	string fileNameSTR = "./data/client/";
	fileNameSTR.append(fileName);

	// Read in the message from given file
	string msgstr;
	std::ifstream infile1;
  	infile1.open(fileNameSTR); //open the input file

	if (infile1.is_open())
	{
		std::stringstream strStream;
		strStream << infile1.rdbuf(); //read the file
		msgstr = strStream.str(); //str holds the content of the file
		infile1.close();
	} else cout << "Unable to open file\n";
	char * msg = new char[msgstr.size()+1];
	strcpy(msg, msgstr.c_str());
	char *message;
	message = msg;

	// output the plaintext to be encrypted
	//cout << endl << "Plaintext: " << endl << message << endl << endl;

	// Pad message to 16 bytes
	int originalLen = strlen((const char *)message);
	
	cout << "Message Length in encrypt.cpp :"<< originalLen <<endl;

	int paddedMessageLen = originalLen;

	if ((paddedMessageLen % 16) != 0) {
		paddedMessageLen = (paddedMessageLen / 16 + 1) * 16;
	}

	unsigned char * paddedMessage = new unsigned char[paddedMessageLen];
	for (int i = 0; i < paddedMessageLen; i++) {
		if (i >= originalLen) {
			paddedMessage[i] = 0;
		}
		else {
			paddedMessage[i] = message[i];
		}
	}

	unsigned char * encryptedMessage = new unsigned char[paddedMessageLen];

	string str;
	ifstream infile;
	infile.open("../aes/keyfile", ios::in | ios::binary);

	if (infile.is_open())
	{
		getline(infile, str); // The first line
		infile.close();
	} else cout << "Unable to open file";

	istringstream hex_chars_stream(str);
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

	for (int i = 0; i < paddedMessageLen; i += 16) {
		AESEncrypt(paddedMessage+i, expandedKey, encryptedMessage+i);
	}

	string ENCfileNameSTR = "./data/client/ENC";
	ENCfileNameSTR.append(fileName);
	ENCfileNameSTR.pop_back();
	ENCfileNameSTR.pop_back();
	ENCfileNameSTR.pop_back();
	ENCfileNameSTR.append("txt");
	ofstream MyFile(ENCfileNameSTR);
	for (int i = 0; i < paddedMessageLen; i++) {
		MyFile <<  encryptedMessage[i];
	}
	
	int m;
	for ( m = 0; m < paddedMessageLen; m++);
	cout << "number of encrypted characters: " << m;
	cout << endl << endl;
	
	cout << "Encrypted message in hex:" << endl;
	for ( int n = 0; n < paddedMessageLen; n++) {
		cout << hex << (int) encryptedMessage[n];
		cout << " ";
		
	}
	cout << endl << endl << "Encrypted message stored in: " << ENCfileNameSTR << endl;
	

	// Free memory
	delete[] paddedMessage;
	delete[] encryptedMessage;

	return 0;
}
*/