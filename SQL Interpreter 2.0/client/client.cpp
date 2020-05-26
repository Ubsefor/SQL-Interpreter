    //
    //  main.cpp
    //  client
    //
    //  Created by Rave Botovski on 22.05.2020.
    //  Copyright © 2020 Rave Botovski. All rights reserved.
    //

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <string>
#include <vector>

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT    8080

int main()
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    
    char        received_size[sizeof ( long )] = { 0 };
    char        * ans;
    
    std::string input = "";
    
    try {
        if ( ( sock = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
            throw "Socket creation error!";
        
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port   = htons( PORT );
        
            // Convert IPv4 and IPv6 addresses from text to binary form
        if ( inet_pton( AF_INET, "127.0.0.1", &serv_addr.sin_addr ) <= 0 )
            throw "Invalid address or not supported.";
        
            // Connecting to server
        if ( connect( sock, (struct sockaddr *) &serv_addr, sizeof ( serv_addr ) ) < 0 )
            throw "Error connecting to server.";
        
        std::cout << "Hello! This is the client.\nUse SELECT, INSERT, UPDATE, DELETE, CREATE and DROP to manipulate the DB!\nPassing QUIT will immediately close server and client.\n";
        std::cout << "Input query: ";
        std::getline( std::cin, input );
        
        
        if ( write( sock, std::to_string( input.size() + 1 ).c_str(), sizeof ( long ) ) < 0 )
            throw "Error sending size";
        
        if ( write( sock, input.c_str(), input.size() + 1 ) < 0 )
            throw "Error sending message";
        
        if ( input != "QUIT" )
        {
            if ( read( sock, received_size, sizeof ( long ) ) < 0 )
            {
                throw "Error receiving message size.";
            }
            
            long size2 = atoi( received_size );
            ans = new char[size2 + 1];
            
            if ( read( sock, ans, size2 + 1 ) < 0 )
            {
                throw "Error receiving message size.";
            }
            
            std::string result = ans;
            std::cout << result << std::endl;
            
            delete [] ans;
        }
    } catch( char const* exept )
    
    {
        std::cerr << "An error occured during client work: " << exept << std::endl;
    }
}
