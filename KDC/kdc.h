#ifndef NELSONDU_KDC_H
#define NELSONDU_KDC_H


#include <iostream>
#include <string>

#include <boost/config.hpp>
#include <boost/program_options/detail/config_file.hpp>
#include <boost/program_options/parsers.hpp>

//for random session keys
#include <boost/nondet_random.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/uniform_int.hpp>

//for reading ini file
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/algorithm/string.hpp>

//for network
#include <boost/array.hpp>
#include <boost/asio.hpp>

//constants
#define KDC_PORT 9642
#define DEBUG

//not entirely sure what this does
using boost::asio::ip::udp;

using namespace std;

void modify();
void start();
bool isValid(string);
int num_clients;


#endif
