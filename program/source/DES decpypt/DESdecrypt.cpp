// DES.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <cstdlib>
#include <sstream>

using namespace std;

int IP[] = {14,6,12,4,8,10,16,2,9,11,5,1,7,13,3,15};
int inverseIP[] = {12,8,15,4,11,2,13,5,9,6,10,3,14,1,16,7};
int E[] = {8, 1, 2, 3, 4, 5, 4, 5, 6, 7, 8, 1};

int SBOX3[4][16] = {
	{10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8},
	{13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1},
	{13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7},
	{1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12}
};

int SBOX4[4][16] =  {
	{7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15},
	{ 13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9},
	{ 10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4},
	{ 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14}
};

int PC1[] = {12,5,14,1,10,2,6,9,15,4,13,7,11,3};

int PC2[] = {6,11,4,
			 8,13,3,
			 12,5,1,
			 10,2,9};

string permute(string*, int*, int);
void strip(string*, string*, string*, int);
string xor(string* ,string*); 
int convertToInt(char);
char convertToChar(int* i);
string sbox(string*, int);

//Pre:  int is number of shift bit . string is text
//Post: 
void rotates(string*);

//Pre: a correct file is existed. path's correct also. two strings do not need to intialize
//Post: gerenrate two keys
void keyGeneralize( string*, string*);

//Pre: a block of bit in string. size is 16. A suvkey that is 12 bits
//Post: encrypted block
string feistel(string*,string* subkey);

int _tmain(int argc, _TCHAR* argv[])
{
	ifstream textFile;
	string c;
	vector<string> blocks; 
	vector<string> encpyptBlocks;

	system("chdir");
	cout << endl << "Please press any key for contiune." << endl;
	getchar();
	textFile.open("cipher.txt",fstream::in);
	
	
	if(!textFile.good())
	{
		cout << "Could not find file" << endl;
		cout << "Current path:" << system("chdir") << endl;
		getchar();
		return 0;
	}
	
	while(textFile.good()){

		int tmp1,tmp2;

		
		tmp1 = textFile.get();
			
		
		if (textFile.good()) {
			
		/* convert to ascii code then convert to binary,
		otherwise chars can not being conver to binary */
			c = std::bitset<8> (tmp1).to_string(); 
		}
		else {
			break;
		}

		tmp2 = textFile.get();

		if (textFile.good()) {
			//same as below
			c += std::bitset<8> (tmp2).to_string();
		}
		else {
			// add 0000 if one char left
			c += std::bitset<8> (0).to_string();
		}

		blocks.push_back(c);
		
		
	}

	textFile.close();

	cout << "Reading message finish" << endl;

	for(int i=0; i < blocks.size(); ++i) {
		
		string cipher;
		
		//cout << "No." << i << " 16 bits block: " << blocks[i] << endl;
		
		cipher =blocks[i];

		// Permute with IP
		cipher = permute(&cipher, IP,16);
	
		//Gerenalte keys
		string key1,key2;
		keyGeneralize(&key1,&key2);

		
		string lptext,rptext,oldlptext,oldrptext;
		strip(&cipher, &lptext, &rptext,8);
		
		//Save perivours R value
		oldrptext = rptext;
		//F begins
		rptext = feistel(&rptext,&key2);
		
		//Xor with left part and make new R
		rptext = xor(&lptext,&rptext);
		
		oldlptext = lptext;
		// make new L
		lptext = oldrptext;

		//Sceond round
		//Save perivours R value
		oldrptext = rptext;

		//F begins
		rptext = feistel(&rptext,&key1);
		
		//Xor with left part and make new R
		rptext = xor(&lptext,&rptext);
		
		oldlptext = lptext;
		// make new L
		lptext = oldrptext;

		//Permuatation with inverse of IP
		cipher = rptext + lptext;
		cipher = permute(&cipher,inverseIP,16);
		
		
		encpyptBlocks.push_back(cipher);
		// 3:25 AM
	}

	// 5:20 AM
	ofstream cipherFile;
	string d;

	cipherFile.open("decoded_text.txt",fstream::out);
	
	if(!cipherFile.good())
	{
		cout << "Could not create file" << endl;
		system("chdir");
		return 0;
	}

	for(int i=0; i < encpyptBlocks.size(); ++i) {
	

			// give d the first element of encpyptBlock.
			d = encpyptBlocks[i];
			
			// divide by 8 bits: 1 char
			string first,last;
			strip(&d,&first, &last,8);

			//convert first to binary and then convert binary to int
			int dec1;
			dec1 = std::bitset<8> (first).to_ulong(); 
			
			//Write dec as a ascii (opposite of get()) into file
			cipherFile.put(dec1);
			

			//convert first to binary and then convert binary to int
			int dec2;
			dec2 = std::bitset<8> (last).to_ulong(); 
			
			//write dec as a ascii (opposite of get()) into file
			cipherFile.put(dec2);

	}

	cipherFile.close();
	
	cout << "Decrtpying finished" << endl;

	return 0;
}

string permute(string *b, int* p,int length) {
	
	string s;

	for (int i =0; i < length; ++i) {
		s += b->substr( p[i] - 1,1);
	}
	
	return s;
}


void strip(string* o,string* parta,string* partb,int pos) {
	*parta = o->substr(0,pos);
	*partb = o->substr(o->length() - pos,o ->length());	
}

string xor(string *ptext, string* k) {
	
	int bit,bitk;
	string s(ptext->size(),'0');
	
	for( int i =0; i < ptext->size() ; ++i) {
		
		
		stringstream(ptext->substr(i,1)) >> bit; 
		stringstream(k->substr(i,1)) >> bitk;
		
		bit = bit ^ bitk;

		s[i] = convertToChar(&bit);
	} 

	return s;
}


char convertToChar(int* i) {

	if ( *i == 0 ) return '0';
	else if ( *i == 1 ) return '1';
	

	return 'E';
}


string sbox(string *o, int n)
{
	string tmp,result;
	//strip r into two part

	
	int row,col;

	//  Concatenate first and last part to one string
	tmp = o->substr(0,1) + o->substr(5,1);
	// first convert to binary by string then convert to int. it had a bit bad smell.
	row = std::bitset<4> (tmp).to_ulong();
	
	//  Concatenate the rest of bits to one string
	tmp = o->substr(1, 4);
	col = std::bitset<4> (tmp).to_ulong();
	
	
	stringstream ss;
	

	if (n == 3) {
		ss << std::bitset<4> (SBOX3[row][col]);
	}

	if (n == 4) {
		ss << std::bitset<4> (SBOX4[row][c
			ol]);
	}
	
	
	
	
	return  ss.str();
}


void keyGeneralize(string* finalkey1,string* finalkey2) {

	
	string k; 
	
	ifstream key("key.txt");

	if(!key.good())
	{
		cout << "Could not find key file" << endl;
		getchar();
		exit(0);
	}
	
	while(key.good()){
		string c;
		key >> c;
		k.append(c);
	}

	key.close();
	
	k = permute(&k,PC1,14);
	
	string ctext,dtext;
	//strip into two parts
	strip(&k,&ctext,&dtext,7);
	
	// rotate C and D part
	rotates(&ctext);
	rotates(&dtext);
	
	// combine C and D part
	k = ctext + dtext;
	//Permute with PC2
	*finalkey1 = permute(&k,PC2,12);

	// rotate C and D part
	rotates(&ctext);
	rotates(&dtext);
	
	// combine C and D part
	k = ctext + dtext;
	//Permute with PC2
	*finalkey2 = permute(&k,PC2,12);
}

void rotates(string* text) {

	// using permutation to shift bit
	int shiftbit[] = {3,4,5,6,7,1,2};

	*text = permute(text,shiftbit,7);
}

string feistel(string* rptext, string* subkey) {

			// Permute with E
		*rptext = permute(rptext, E, 12);

		//XOR
		*rptext = xor( rptext, subkey);
	
		//Substitution function
		string Rl,Rr;
		strip(rptext,&Rl,&Rr,6);
		*rptext = sbox(&Rl,4) + sbox(&Rr,3);

		//premulation with P-box
		int P[] = {6, 4, 7, 3, 5, 1, 8, 2};
		*rptext = permute(rptext,P,8);
	
		return *rptext;
}