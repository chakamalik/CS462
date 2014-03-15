/********* BFtest.cc **********/
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include "blowfish.h"

#define BUFF_SIZE 1048576  // 1MB
#define NUM_TRIALS 100


int Test(Blowfish *);
double Speed(Blowfish *);


void main()
{
	int result;
	double speed;
	Blowfish BF;

	cout << "Blowfish verification: ";
	if (result = Test(&BF))
	{
		cout << "\aFailed " << (result>0 ? "en" : "de") << "crypting test vector " <<
			abs(result) << endl;
		return;
	}
	else
		cout << "Passed" << endl;

	if ((speed = Speed(&BF)) <= 0)
		cout << "Not enough time elapsed for the test, or something funny happend." <<
		endl;
	else
		cout << "The throughput is " << speed << "MB/s" << endl;
}


int Test(Blowfish *BF)
{
	unsigned int i;
	DWord Test_Vect;
	char *Passwd[2] = { "abcdefghijklmnopqrstuvwxyz", "Who is John Galt?" };
	unsigned int Clr0[2] = { 0x424c4f57, 0xfedcba98 };
	unsigned int Clr1[2] = { 0x46495348, 0x76543210 };
	unsigned int Crypt0[2] = { 0x324ed0fe, 0xcc91732b };
	unsigned int Crypt1[2] = { 0xf413a203, 0x8022f684 };

	for (i = 0; i<2; i++)
	{
		Test_Vect.word0.word = Clr0[i];
		Test_Vect.word1.word = Clr1[i];
		BF->Set_Passwd(Passwd[i]);
		BF->Encrypt((void *)&Test_Vect, 8);
		if (Test_Vect.word0.word != Crypt0[i] || Test_Vect.word1.word != Crypt1[i])
			return (i + 1);
		BF->Decrypt((void *)&Test_Vect, 8);
		if (Test_Vect.word0.word != Clr0[i] || Test_Vect.word1.word != Clr1[i])
			return -(i + 1);
	}
	return 0;
}


double Speed(Blowfish *BF)
{
	char *buff;
	unsigned int i;
	time_t begin, end;

	buff = new char[BUFF_SIZE];
	if (buff == NULL)
	{
		cerr << "\aRan out of memory for the test buffer\n";
		return 0;
	}

	srand(0);
	for (i = 0; i<BUFF_SIZE; i++)
		buff[i] = rand() % 256;
	BF->Set_Passwd("ianchan");

	begin = time(NULL);
	for (i = 0; i<NUM_TRIALS; i++)
		BF->Encrypt((void *)buff, BUFF_SIZE);
	end = time(NULL);

	delete[]buff;
	if (end - begin < 10)
		return 0;
	else
		return double(NUM_TRIALS) / (end - begin);
}
