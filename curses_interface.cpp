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
//#include "everythingelse.hpp"
#include "termcolor.hpp"
using namespace std;

std::vector<string> output_buffer;
std::vector<char> input_buffer;
bool INPUT_FLAG = true;

WINDOW* window;
WINDOW* output_window;
WINDOW* input_window;
int row = 0, col = 0;
//void* resizehandler(int);


//g++ ./main.cpp -o ./main.o -L/usr/include/boost/program_options.hpp -lboost_program_options -lboost_system -lboost_filesystem -lhttpserver -lpthread -lcurses
void termcolorprint(std::string color, std::string text) {
    if (color == "red"){
        std::cout << termcolor::red << text << termcolor::reset << std::endl;
    } else if (color == "green") {
        std::cout << termcolor::green << text << termcolor::reset << std::endl;
    } else if (color == "magenta") {
        std::cout << termcolor::magenta << text << termcolor::reset << std::endl;
    } else if (color == "blue") {
        std::cout << termcolor::blue << text << termcolor::reset << std::endl;
    } else if (color == "grey") {
        std::cout << termcolor::grey << text << termcolor::reset << std::endl;
    } else if (color == "cyan") {
        std::cout << termcolor::cyan << text << termcolor::reset << std::endl;
    } else if (color == "white") {
        std::cout << termcolor::white << text << termcolor::reset << std::endl;
    } else if (color == "yellow") {
        std::cout << termcolor::yellow << text << termcolor::reset << std::endl;
    };
};

void errprint(auto err) {
    termcolorprint("red" , err);
};



int term_init(){
    //some error handling with a custom message added
    try {
        window = initscr();
        if (window == NULL ) {
            //termcolorprint( "blue" , "ERROR Starting the ncurses display\n" )
            printf("ERROR: init() failed");
        };
    } catch (std::exception& e) {
        auto errortext = e.what();
        errprint(errortext);
        return 1;
        };
    getmaxyx(window, row, col);
    cbreak();
    noecho();
    nodelay(stdscr, true);
    nonl();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
    refresh();
};


void update_output_window(std::string output_string ){
    touchwin(window);
    output_string += "\n";
    wprintw(output_window, output_string.c_str());
    wrefresh(output_window);

};

void update_input_window(std::string output_string ){
    touchwin(window);
    wprintw(input_window, output_string.c_str());
    wrefresh(input_window);

};

void update_output_windowfrominput(std::string output_string ){
    touchwin(window);
    output_string += "\n";
    wprintw(output_window, output_string.c_str());
    wrefresh(output_window);
};

void process_output_buffer(){
   // std::string output_window_string(output_buffer.begin(),output_buffer.end()234);
   // update_output_window(output_window_string.c_str());
};


//redraws the window and contents when you resize the terminal window.
void resizehandler(int sig){
    int new_display_height;
    int new_display_width;
    getmaxyx(stdscr, new_display_height, new_display_width);
    refresh();

};

void shutdown_server(){
    //stop input from happening
    INPUT_FLAG = false;
    std::string message("SHUTTING DOWN");
    update_output_window(message);
    // do stuff to clean up and printto screen the stuff captured and what not

};

//we want to be able to block input from time to time for sensitive commands.
//having a flad to turn off the function and a function to call to turn it back on is uiseful for that
//you can have the function change flags or do it yourself everytime you call the function.
string get_command(){
    INPUT_FLAG == true;
    while (INPUT_FLAG == true) {
        char input_character = getch();
        if (input_character == ERR) {

        } else if(input_character == '\r'){
            std::string input_buffer_string(input_buffer.begin(),input_buffer.end());
            update_output_windowfrominput(input_buffer_string);
        } else {
            input_buffer.push_back(input_character);
            if (input_character == '^G') {
                    if (input_buffer.empty()) {

                    } else {
                        input_buffer.pop_back();
                        std::string input_buffer_string(input_buffer.begin(),input_buffer.end());
                        update_commandline(input_buffer_string);
                    };
            } else if (input_character == KEY_DL) {
                input_buffer.clear();
            };
            std::string input_buffer_string(input_buffer.begin(),input_buffer.end());
            update_input_window(input_buffer_string)
        };

    };
    std::string command(input_buffer.begin(),input_buffer.end());
    input_buffer.clear();
    return command;
};


int main(int argc, char* argv[]) {
// we put things we only want done at the start at the top
// once the loops start, shit gets real.
// even though main() is a "loop", its easier to keep things functional  and orderly
// by reading it like a book instead of like a mobeius strip. no looping main() please.
// its got a start and a finish. unless you need otherwise.

    term_init();
    fflush(stdin);
    INPUT_FLAG = true;
    scrollok(output_window, true);
    output_window = subwin(window, row - 1, col, 0, 0);
    input_window = subwin(window, 1, col, row - 1, 0);

    //beginning thw main loop in the main loop, remember ,its gonna keep looping from top to bottom constantly
    // until it hits a break or exit()
    while(1)
    {
        //if we get a window resize signal, redraw the curses display
        signal(SIGWINCH, resizehandler);
        std::string input(get_command());
        get_command();
        };
    endwin();
    };
};
