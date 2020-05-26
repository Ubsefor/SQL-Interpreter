    //
    //  main.cpp
    //  server
    //
    //  Created by Rave Botovski on 22.05.2020.
    //  Copyright © 2020 Rave Botovski. All rights reserved.
    //


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



#include "Libraries/table.hpp"
#include "Libraries/lexer.hpp"
#include "Libraries/parser.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sstream>
#include <vector>
#include <string>
#include <cstdio>
#include <stack>
#include <memory>
#include <regex>

#define PORT    8080

using namespace Lex;
using namespace Parser;


int main()
{
    int                socket_fd = 0, new_socket = 0, opt = 1, size;
    struct sockaddr_in address;
    socklen_t          addr_len = sizeof ( address );
    char               * packet = nullptr;
    char               received_size[sizeof ( long )] = { 0 };
    std::stringstream  received_message;
    std::string        answer;
    
    try {
            // Creating socket file descriptor
        if ( ( socket_fd = socket( AF_INET, SOCK_STREAM, 0 ) ) == 0 )
            throw "Socket creation Error!";
        
            // Forcefully attaching socket to the port 8080
        if ( setsockopt( socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof ( opt ) ) )
            throw "Error setting some socket options.";
        
        address.sin_family      = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port        = htons( PORT );
        
            // Forcefully attaching socket to the port 8080
            // bind(int socket, const struct sockaddr *address, socklen_t address_len);
        
        if ( bind( socket_fd, (struct sockaddr *) &address, (socklen_t) addr_len ) < 0 )
            throw  "Socket binding failed!";
        
        
        while ( 1 )
        {
                // Creating listening socket
            if ( listen( socket_fd, 3 ) < 0 )
                throw "Error at creating listening socket!";
            
                // Creating accept socket
            if ( ( new_socket = accept( socket_fd, (struct sockaddr *) &address,
                                       (socklen_t *) &addr_len ) ) < 0 )
                throw "Creating accept socket failed!";
            
            if ( read( new_socket, received_size, sizeof ( long ) ) < 0 )
            {
                throw "Error receiving message size.";
            }
            
            size = atoi( received_size );
            
            packet = new char[size + 1]; // leak
            memset( packet, 0, ( size + 1 ) );
            
            if ( read( new_socket, packet, size + 1 ) < 0 )
            {
                throw "Error receiving message size.";
            }
            
            if ( strcmp( packet, "QUIT" ) == 0 )
                break;
            
            received_message << packet << std::endl;
            std::cout << packet << std::endl << std::endl;
            std::string result = "";
            
            std::vector < Token > got_tokens = Tokenize( received_message );
            if ( !check_sentence( got_tokens ) )
            {
                result = "Lexical error! Please check your query.";
                if ( write( new_socket, std::to_string( result.size() + 1 ).c_str(), sizeof ( long ) ) < 0 )
                    throw "Error sending size";
                
                if ( write( new_socket, result.c_str(), result.size() + 1 ) < 0 )
                    throw "Error sending message";
            }
            else
            {
                parse_sentence( got_tokens, result );
                if ( result == "" )
                {
                    result = "OK";
                }
                std::cout << result << std::endl;
                
                if ( write( new_socket, std::to_string( result.size() + 1 ).c_str(), sizeof ( long ) ) < 0 )
                    throw "Error sending size";
                
                if ( write( new_socket, result.c_str(), result.size() + 1 ) < 0 )
                    throw "Error sending message";
            }
            
            result.erase();
            got_tokens.erase( got_tokens.begin(), got_tokens.end() );
            received_message.clear();
            delete [] packet;
            close( new_socket );
        }
        
        delete [] packet;
        shutdown( socket_fd, SHUT_RDWR );
        close( new_socket );
    } catch( char const* exept )
    {
        std::cerr << "An error occured during server work: " << exept << std::endl;
        close( new_socket );
        delete [] packet;
        return 1;
    }
    
    return 0;
}

