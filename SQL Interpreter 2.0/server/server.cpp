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
#include <sstream>
#include <vector>
#include <string>
#include <cstdio>
#include <stack>
#include <memory>
#include <regex>

using namespace Lex;
using namespace Parser;

int main(int argc, const char * argv[]) {
    
    
    
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
