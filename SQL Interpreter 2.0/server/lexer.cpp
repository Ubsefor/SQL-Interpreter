    //
    //  lexer.cpp
    //  server
    //
    //  Created by Rave Botovski on 22.05.2020.
    //  Copyright © 2020 Rave Botovski. All rights reserved.
    //

#include "Libraries/lexer.hpp"


namespace Lex {
    /*
     enum tokenType
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
     */
    bool inline IsShowableC( int c )
    {
        return ( ( c >= 'a' ) && ( c <= 'z' ) ) || ( ( c >= 'A' ) && ( c <= 'Z' ) ) || ( c == '_' );
    }
    
    bool inline IsNumeric( int c )
    {
        return ( c >= '0' ) && ( c <= '9' );
    }
    
    bool inline IsShowableSTR( int c )
    {
        return ( ( c >= 'a' ) && ( c <= 'z' ) ) ||
        ( ( c >= 'A' ) && ( c <= 'Z' ) ) ||
        ( ( c >= '0' ) && ( c <= '9' ) ) ||
        ( c == '_' );
    }
    
    bool inline IsEOF( int c )
    {
        return c == EOF;
    }
    
    bool inline IsSeparator( int c )
    {
        return ( c == '\n' ) || ( c == ' ' );
    }
    
    TokenType DefTokenType( std::string& name )
    {
        if ( name == "SELECT" )
            return SELECT;
        
        else if ( name == "INSERT" )
            return INSERT;
        
        else if ( name == "UPDATE" )
            return UPDATE;
        
        else if ( name == "DELETE" )
            return DELETE;
        
        else if ( name == "CREATE" )
            return CREATE;
        
        else if ( name == "DROP" )
            return DROP;
        
        else if ( name == "FROM" )
            return FROM;
        
        else if ( name == "WHERE" )
            return WHERE;
        
        else if ( name == "INTO" )
            return INTO;
        
        else if ( name == "TABLE" )
            return TABLE;
        
        else if ( name == "TEXT" )
            return TEXT;
        
        else if ( name == "LONG" )
            return LONG;
        
        else if ( name == "AND" )
            return AND;
        
        else if ( name == "OR" )
            return OR;
        
        else if ( name == "NOT" )
            return NOT;
        
        else if ( name == "LIKE" )
            return LIKE;
        
        else if ( name == "IN" )
            return IN;
        
        else if ( name == "ALL" )
            return ALL;
        
        else if ( name == "SET" )
            return SET;
        
        else
            return NAME;
    }
    
    Token GetToken( std::stringstream& got_stream )
    {
        Token ret_val;
        int   c;
        
        c = got_stream.get();
        
        while ( ( c == ' ' ) || ( c == '\n' ) )
            c = got_stream.get();
        
        if ( ( c == EOF ) && got_stream.eof() )
            return ret_val;
        
        if ( c == '\'' )
        {
            ret_val.type = SQL_STR;
            
            do
            {
                c = got_stream.get();
                ret_val.value.push_back( c );
                if (got_stream.eof()){
                    ret_val.type = ERR;
                    break;
                }
            } while ( c != '\'' );
            ret_val.value.pop_back();
            return ret_val;
        }
        
        else if ( IsShowableC( c ) )
        {
            do
            {
                ret_val.value.push_back( c );
                c = got_stream.get();
            } while ( IsShowableSTR( c ) );
            
            if ( !IsEOF( c ) )
                got_stream.putback( c );
            ret_val.type = DefTokenType( ret_val.value );
            return ret_val;
        }
        
        else if ( c == '+' )
        {
            ret_val.type = ADD;
            ret_val.value.push_back( '+' );
    
            return ret_val;
        }
        
        else if ( c == '-' )
        {
            ret_val.type = SUB;
            ret_val.value.push_back( '-' );
            
            return ret_val;
        }
        
        else if ( c == '*' )
        {
            ret_val.type = MULT;
            ret_val.value.push_back( '*' );
            
            return ret_val;
        }
        
        else if ( c == '/' )
        {
            ret_val.type = DIV;
            ret_val.value.push_back( '/' );
            
            return ret_val;
        }
        
        else if ( c == '%' )
        {
            ret_val.type = PERCENT;
            ret_val.value.push_back( '%' );
            
            return ret_val;
        }
        
        else if ( c == '=' )
        {
            ret_val.type = EQUAL;
            ret_val.value.push_back( '=' );
            
            return ret_val;
        }
        
        else if ( c == '>' )
        {
            if ( got_stream.peek() == '=' )
            {
                got_stream.get();
                ret_val.type = G_EQUAL;
                ret_val.value = ">=";
                return ret_val;
            }
            else
            {
                ret_val.type = GREATER;
                ret_val.value.push_back( '>' );
                
                return ret_val;
            }
        }
        
        else if ( c == '<' )
        {
            if ( got_stream.peek() == '=' )
            {
                got_stream.get();
                ret_val.type = L_EQUAL;
                ret_val.value = "<=";
                return ret_val;
            }
            else
            {
                ret_val.type = LESS;
                ret_val.value.push_back( '<' );
                
                return ret_val;
            }
        }
        
        else if ( c == '!' )
        {
            c = got_stream.get();
            
            if ( c != '=' )
            {
                ret_val.type = ERR;
                ret_val.value = "Expected '=' after '!' !";
                return ret_val;
            }
            else
            {
                ret_val.type = N_EQUAL;
                ret_val.value = "!=";
                return ret_val;
            }
        }
        
        else if ( c == '(' )
        {
            ret_val.type = L_BRACKET;
            ret_val.value.push_back( '(' );
            
            return ret_val;
        }
        
        else if ( c == ')' )
        {
            ret_val.type = R_BRACKET;
            ret_val.value.push_back( ')' );
            
            return ret_val;
        }
        
        else if ( c == '[' )
        {
            ret_val.type = L_SQ_BRACKET;
            ret_val.value.push_back( '[' );
            
            return ret_val;
        }
        
        else if ( c == ']' )
        {
            ret_val.type = R_SQ_BRACKET;
            ret_val.value.push_back( ']' );
            
            return ret_val;
        }
        
        else if ( c == '^' )
        {
            ret_val.type = POW;
            ret_val.value.push_back( '^' );
            
            return ret_val;
        }
        
        else if ( IsNumeric( c ) )
        {
            ret_val.type = SQL_LONG;
            
            do
            {
                ret_val.value.push_back( c );
                c          = got_stream.get();
            } while ( IsNumeric( c ) );
            
            if ( !IsEOF( c ) )
                got_stream.putback( c );
            
            return ret_val;
        }
        
        else if ( c == ',' )
        {
            ret_val.type = COMMA;
            ret_val.value.push_back( ',' );
            
            return ret_val;
        }
        
        else
        {
            ret_val.type = ERR;
            ret_val.value = "Unexpected Symbol: ";
            ret_val.value.push_back( c );
            
            return ret_val;
        }
    }
    
    std::vector<Token> Tokenize ( std::stringstream& got_stream )
    {
        
        std::vector<Token> ret_vec;
        
        Token got_token = GetToken(got_stream);
        ret_vec.push_back(got_token);
        
        do {
            Token got_token = GetToken(got_stream);
            ret_vec.push_back(got_token);
            if (got_token.type == ERR){
                break;
            }
        } while ( got_token.type != END && !got_stream.eof() );
        
        return ret_vec;
    }
    
}
