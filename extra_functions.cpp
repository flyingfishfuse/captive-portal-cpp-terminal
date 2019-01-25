#include "everythingelse.hpp"
bool INPUT_FLAG;

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
    INPUT_FLAG = false;
    std::string message("SHUTTING DOWN");
    update_window(stdscr, message);
    refresh();

};
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
