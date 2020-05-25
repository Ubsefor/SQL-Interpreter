    //
    //  lexer.hpp
    //  server
    //
    //  Created by Rave Botovski on 15.05.2020.
    //  Copyright © 2020 Rave Botovski. All rights reserved.
    //

#ifndef lexer_hpp
#define lexer_hpp

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

/*
 <SQL_SENTENCE> -> <SELECT_SENTENCE> | <INSERT_SENTENCE> | <UPDATE_SENTENCE> | <DELETE_SENTENCE> | <CREATE_SENTENCE> | <DROP_SENTENCE>
 
 <SELECT_SENTENCE> -> SELECT <FIELD_LIST> FROM <NAME> <WHERE_CLAUSE>
 
 <FIELD_LIST> -> <NAME> {, <NAME>} | *
 <NAME> -> id
 
 V <INSERT_SENTENCE> -> INSERT INTO <NAME> (<FIELD_VALUE> {, <FIELD_VALUE>})
 
 <FIELD_VALUE> > <SQLSTRING> | <SQLLONG>
 <SQLstring> -> '<SQLCHAR>{<SQLCHAR>}'
 <SQLchar> -> <im(input)\'>
 
 <UPDATE_SENTENCE> -> UPDATE <NAME> SET <NAME> = <EXPRESSION> <WHERE_CLAUSE>
 
 
 <DELETE_SENTENCE> -> DELETE FROM <NAME> <WHERE_CLAUSE>
 
 
 V <CREATE_SENTENCE> -> CREATE TABLE <NAME> (<LIST_DEFINITIONS>)
 
 <LIST_DEFINITIONS> -> <DEF_FIELD> {, <DEF_FIELD>}
 <DEF_FIELD> -> <NAME> <FIELD_TYPE>
 <FIELD_TYPE> -> TEXT ( <UNSIGNED> ) | LONG
 
 V <DROP_SENTENCE> -> DROP <TABLE NAME>
 
 
 <WHERE_CLAUSE> -> WHERE <FIELD_NAME_TEXT> [NOT] LIKE <TEMPLATE_STRING> | <EXPRESSION> [NOT] IN (<LIST_CONST>) | WHERE <LOGICAL_EXPRESSION | WHERE ALL
 <TEMPLATE_STRING> -> <SQLSTRING>
 <EXPRESSION> -> <LONG_EXPRESSION> | <TEXT_EXPRESSION>
 <LIST_CONST> -> <SQLSTRING> {, <SQLSTRING>} | <SQLLONG> {, <SQLLONG>}
 <LONG_EXPRESSION> -> <LONG_AD> {<AD> <LONG_AD>}
 <AD> -> + | -
 <LONG_AD> -> <LONG_ML> { <ML> <LONG_ML>}
 <ML> -> * | / | %
 <LONG_ML> -> <LONG_VAL> | (<LONG_EXPRESSION>)
 <LONG_VAL> -> <FIELD_NAME_LONG> | <SQLLONG>
 <TEXT_EXPRESSION> -> <FIELD_NAME_TEXT> | <SQLSTRING>
 <LOGICAL_EXPRESSION> -> <LOGICAL_ML> { AND <LOGICAL_ML>}
 <LOGICAL_ML> -> NOT <LOGICAL_ML> | (<LOGICAL_EXPRESSION>) | (<RATIO>)
 <RATIO> -> <TEXT_RATIO> | <LONG_RATIO>
 <TEXT_RATIO> -> <TEXT_EXPRESSION> <RATIO_OP> <TEXT_EXPRESSION>
 <LONG_RATIO> -> <LONG_EXPRESSION> <RATIO_OP> <LONG_EXPRESSION>
 <RATIO_OP> -> = | > | >= | <= | !=
 
 */

namespace Lex {
    
    enum TokenType
    {
        SELECT, INSERT, UPDATE, DELETE,
        CREATE, DROP, FROM, WHERE,
        INTO, TABLE, TEXT, LONG,
        AND, OR, NOT, LIKE,
        IN, ALL, SET, ADD,
        SUB, MULT, DIV, PERCENT,
        EQUAL, GREATER, LESS, G_EQUAL,
        L_EQUAL, N_EQUAL, NAME, END,
        COMMA, L_BRACKET, R_BRACKET, L_SQ_BRACKET,
        R_SQ_BRACKET, POW, SQL_STR,
        SQL_LONG, ERR
    };
    
    bool inline IsShowableC( int c );
    bool inline IsNumeric( int c );
    bool inline IsShowableSTR( int c );
    bool inline IsEOF( int c );
    bool inline IsSeparator( int c );
    
    struct Token
    {
        enum TokenType type  = END;
        std::string    value  = "";
        
    };
    
    TokenType DefTokenType( std::string & );
    
    Token GetToken( std::stringstream & );
    
    std::vector<Token> Tokenize ( std::stringstream & );
        
        
}

#endif /* lexer_hpp */
