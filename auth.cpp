#include <openssl/sha.h>
#include <stdio.h>
#include <string.h>
#include <boost/algorithm/string.hpp>
#include <bits/stdc++.h>
#include "everythingelse.hpp"

using namespace std;

std::string generate_sha1hash(const unsigned char* passwd) {
    const unsigned char* password = passwd ;
    const char* pass_length = reinterpret_cast<const char*>(password);
    unsigned char* sha1_output;
    SHA1(password , strlen(pass_length) , sha1_output);
    std::string function_output(reinterpret_cast<char*>(sha1_output));
    return function_output;
};

int save_password (std::string hashed_password, std::string username, std::string filename) {
    try {
        std::string name = username;
            std::string pass = hashed_password;
            auto pwd = boost::filesystem::current_path().string();
            std::string file = pwd + filename;
            std::ofstream credentials(file);
            std::string data(name + ":" + pass + "\n");
            credentials << data;
            return 0;
    } catch (std::exception& e) {
            auto errortext = e.what();
            errprint(errortext);
            return 1;

    };
};


//returns a true false value depending on wether the credentials supplied are correct
// we do not store passwords, we store hashes, those hashes are compared to the input
// if its in the database, auth happens.
bool retrieve_credentials_legal(std::string filename, std::string user, std::string sha1_pass) {
    std::ifstream input_file(filename);
    std::string pass_input = sha1_pass;
    std::string user_input = user;
    std::string user_pass_input;
    std::vector<string> user_pass_output;
    while (std::getline(input_file, user_pass_input)) {
        boost::split(user_pass_output, user_pass_input , boost::is_any_of(":"));
        if (user_input == user_pass_output[0] && sha1_pass == user_pass_output[1]){
            return true;
        } else {
            return false;
        };
    };
};


























