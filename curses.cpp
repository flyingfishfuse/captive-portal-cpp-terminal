#include "everythingelse.hpp"


bool INPUT_FLAG = true;

using namespace std;

void update_output_windowfrominput(std::string output_string){
    int bottom_of_output_window;
    int width_of_window;
    int height_of_window;
    getmaxyx(stdscr, height_of_window, width_of_window);
    //bottom_of_output_window = display_height - 1;
    touchwin(stdscr);
    output_string += "\n";
    //wprintw(stdscr, bottom_of_output_window, 0, output_string.c_str() );
    //wprintw(output_window, output_string.c_str(), );
    refresh();
    //wrefresh(output_window);
    //clrtoeol();
};

void update_window(WINDOW* windoh, std::string output_string ){
    touchwin(stdscr);
    //output_string += "\n";
    wprintw(windoh, output_string.c_str());
    wrefresh(windoh);

};
std::string string_vector_to_string(std::vector<char> input_vector) {
    std::string output_string(input_vector.begin(), input_vector.end());
    return output_string;

};

void resizehandler(int sig){
    int new_display_height;
    int new_display_width;
    getmaxyx(stdscr, new_display_height, new_display_width);
    //display_height = new_display_height;
    //display_width = new_display_width;
    refresh();

};

void get_command(){
    int cursor_x_location = 0;
    wmove(stdscr, LINES -1, cursor_x_location);
    INPUT_FLAG == true;
    std::string output_string;
    //output_string += "\n";
    do {
        char input_character = getch();
        if (input_character != ERR) {
            input_buffer.push_back(input_character);
            if(input_character == '\n'){
                input_buffer_string = string_vector_to_string(input_buffer);
                input_buffer.clear();
                output_string = input_buffer_string ;
                wprintw(stdscr, output_string.c_str());
                refresh();
                clrtoeol();
            /*
             if(input_character == '\r' || '\n'){
                input_buffer_string = string_vector_to_string(input_buffer);
                //update_output_windowfrominput(input_buffer_string);
                input_buffer.clear();
                output_string = input_buffer_string;
                wprintw(stdscr, output_string.c_str() );
                wrefresh(stdscr);
                clrtoeol();
            }
        */  } else if (input_character == '^T') {
                //input_buffer.push_back(input_character);
                //printw()
            } else if (input_character == '^?') {
                //int new_cursor_x_location = cursor_x_location -1;
                //wmove(stdscr, LINES -1, new_cursor_x_location -1 );
                input_buffer.pop_back();
                delch();
                delch();
            };
        } else if (input_character == KEY_BACKSPACE) { input_buffer.pop_back(); };
               //input_buffer_string = string_vector_to_string(input_buffer);
    } while (INPUT_FLAG == true);

    //input_buffer.clear();
};
