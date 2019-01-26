#ifndef EVERYTHINGELSE
#define EVERYTHINGELSE
#include "termcolor.hpp"
#include <curses.h>
#include <signal.h>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iterator>
#include <stdio.h>
#include <cstdlib>
#include <bits/stdc++.h>
#include <stdlib.h>
#include <unistd.h>
#include <boost/filesystem.hpp>

#include </usr/include/stdio.h>

using namespace std;

//WINDOW* main_window;

void update_window(WINDOW* , std::string);
void termcolorprint(std::string, std::string);
void errprint(auto);
void resizehandler(int);
void get_command();
void update_output_window();
void shutdown_server();
std::string string_vector_to_string(std::vector<char> );
std::string generate_sha1hash(unsigned char);
int save_password (std::string , std::string , std::string );
bool retrieve_credentials_legal(std::string , std::string , std::string );


#endif

