#ifndef NELSONDU_SERVER_H
#define NELSONDU_SERVER_H

//INCLUDE
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

//for reading ini file
#include <boost/lexical_cast.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

//for string on ; parsing
#include <boost/algorithm/string.hpp>

//fer blowfish
#define DWORD  		unsigned long
#define WORD  		unsigned short
#define BYTE  		unsigned char

//USING
using boost::asio::ip::udp;
using namespace std;

//GLOBAL
string clientList[9];
string this_client;
int num_clients;
string kdc_address;
string kdc_port;

//DEBUG = ON
#define DEBUG


//FUNCTIONS
//returns nonce
//Tans function, I beleive.
long unsigned int generateNonce();

//Reads the information in from ini file.
void parseIni();

//BLOWFISH - will be removed
string encrypt(string key, string msg);
string decrypt(string key, string msg);

//Via KDC, connect to B(choice)
void talk(int choice);

//I am B (reciever)
void waitForConnect();

//main
int main(int argc, char* argv[]);

#endif
