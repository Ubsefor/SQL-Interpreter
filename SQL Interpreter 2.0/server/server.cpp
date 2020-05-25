    //
    //  main.cpp
    //  server
    //
    //  Created by Rave Botovski on 22.05.2020.
    //  Copyright © 2020 Rave Botovski. All rights reserved.
    //


#include "Libraries/table.hpp"
#include "Libraries/lexer.hpp"
#include "Libraries/parser.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include <sstream>
#include <vector>

#include <string>
#include <cstdio>
#include <stack>
#include <memory>
#include <regex>

using namespace Lex;
using namespace Parser;

#define PORT           8080

/*
 Tests from book. Just copy and paste to string and recompile.
 
 "CREATE TABLE Students ( First_name TEXT ( 10 ) , Surname TEXT ( 15 ) , Age LONG , Phone TEXT ( 9 ) )";
 "INSERT INTO Students ( 'Alexey' , 'Petrov' , 20 , '343-65-45' )";
 "SELECT * FROM Students WHERE ALL";
 "DELETE FROM Students WHERE First_name = 'Alexey'";
 "SELECT * FROM Students WHERE ALL";
 "INSERT INTO Students ( 'Alexey' , 'Petrov' , 20 , '343-65-45' )";
 "INSERT INTO Students ( 'Andrey' , 'Fedorov' , 23 , '123-45-18' )";
 "INSERT INTO Students ( 'Alexandre' , 'Zaharov' , 20 , '345-33-33' )";
 "SELECT * FROM Students WHERE ALL ";
 "SELECT First_name , Surname FROM Students WHERE Age IN ( 18 , 19 , 20 ) ";
 "SELECT * FROM Students WHERE Phone LIKE '%-%-45'";
 "SELECT * FROM Students WHERE Phone LIKE '%45%'";
 "SELECT Phone FROM Students WHERE Surname = 'Petrov'";
 "SELECT Surname FROM Students WHERE ALL";
 "SELECT * FROM Students WHERE First_name LIKE '[ABC][^mno]_x%'";
 "SELECT Surname , Phone FROM Students WHERE ( Age > 19 ) AND ( Surname > 'M' )";
 "SELECT * FROM Students WHERE ALL";
 "UPDATE Students SET Age = Age + 1 WHERE ALL";
 "SELECT * FROM Students WHERE ALL";
 "DROP TABLE Students";
 */



int main(int argc, const char * argv[]) {
    
    int                socketFD, newSocket, opt = 1;
    struct sockaddr_in address;
    socklen_t          addrlen = sizeof ( address );
    std::stringstream  receivedMessage;
    
    try {
            // Creating socket file descriptor
        if ( ( socketFD = socket( AF_INET, SOCK_STREAM, 0 ) ) == 0 )
            throw "Socket creation Error!";
        
            // Forcefully attaching socket to the port 8080
        if ( setsockopt( socketFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof ( opt ) ) )
            throw "Error setting some socket options.";
        
        address.sin_family      = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port        = htons( PORT );
        
            // Forcefully attaching socket to the port 8080
            // bind(int socket, const struct sockaddr *address, socklen_t address_len);
        if ( bind( socketFD, (struct sockaddr *) &address, addrlen ) < 0 )
            throw  "Socket binding failed!";
        
        if ( listen( socketFD, 3 ) < 0 )
            throw "Error at creating listening socket.";
        
        if ( ( newSocket = accept( socketFD, (struct sockaddr *) &address,
                                  (socklen_t *) &addrlen ) ) < 0 )
            throw "Creating accept socket failed!";
        
        std::string test;
        
    } catch( char const* exept )
    {
        std::cerr << "An error occured during server work: " << exept << std::endl;
    }
        
    
    
    
    
    
    
    
    
    
    
    
    std::stringstream test;
    test << "SELECT * FROM Students WHERE ALL" << std::endl;
    
    std::vector <Token> tok_vector = Tokenize(test);
        
    for (auto iter = tok_vector.begin(); iter != tok_vector.end(); iter++){
        std::cout << iter->value << std::endl;
    }
    
    std::string result;
    if (!check_sentence(tok_vector)){
        result = "Hey look at ya tokens, m8";
        std::cout << result << std::endl;
        return 1;
    }
    
    parse_sentence(tok_vector, result);
    std::cout << result << std::endl;
    
    
    
    return 0;
}
