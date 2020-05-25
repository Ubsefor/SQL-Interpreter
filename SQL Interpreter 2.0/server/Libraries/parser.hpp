//
//  parser.hpp
//  server
//
//  Created by Rave Botovski on 22.05.2020.
//  Copyright © 2020 Rave Botovski. All rights reserved.
//

#ifndef parser_hpp
#define parser_hpp

#include "table.hpp"
#include "lexer.hpp"
#include <stack>
#include <regex>

namespace Parser {
    
    using namespace Lex;
    
    std::vector < Token > parse_expr( std::stringstream& );
    
    enum PolizType
    {
        NUMERIC,
        LOGIC
    };
    
    struct PolizExpr
    {
        std::string fieldName;
        std::vector < Lex::Token > poliz;
    };
    
    std::vector < Token > parse_expr( std::stringstream& );
    
    bool compute( std::vector < Lex::Token > , std::string& );
    
    struct InsertField
    {
        FieldType    type;
        std::string  str;
        unsigned int num;
    };
    
    enum UpdateExprType
    {
        FIELD_NAME,
        OPERATOR,
        NUMBER
    };
    
    
    struct WhereCondition
    {
        TokenType type;
        std::vector < Lex::Token > lex_vec1;
        std::vector < Lex::Token > lex_vec2;
        Lex::Token     lex_token1;
        Lex::Token     lex_token2;
        bool           Not;
    };
    
    using namespace Lex;
    
    std::vector < Token > make_poliz( std::vector < Token > );
    
    bool preprocess( THandle, Token &, std::string& );
    
    bool preprocess( THandle, std::vector < Token > &, std::string& );
    
    bool compute_in( std::string , std::vector < Token >, std::string& );
    
    void make_regexp( std::string& , const std::string& , const std::string&  );
    
    bool compute_like( std::string , std::string , std::string&  );
    
    bool process( THandle, std::vector < Token >, std::string& );
    
    bool where_clause( THandle , WhereCondition, std::string& );
    
    bool get_field_names( THandle, std::vector < std::string > &, std::string& );
    
    
    bool exec_new( std::string, std::vector < struct FieldDef > , std::string&);
    
    bool exec_rm( std::string, std::string& );
    
    bool exec_ins( std::string , std::vector < struct InsertField >, std::string&  );
    
    
    bool exec_del( std::string , WhereCondition, std::string& );
    
    bool exec_upd( std::string , PolizExpr , WhereCondition, std::string& );
    
    bool exec_sel( std::vector < std::string >, std::string, WhereCondition, std::string& );
    
    
    bool check_sentence ( std::vector<Token> );
    
    bool parse_sentence( std::vector <Token>, std::string & );
    
    bool drop_table_call(std::vector<Token>, std::string &);
    
    bool create_table_call(std::vector<Token>, std::string &);
    
    bool insert_table_call(std::vector<Token>, std::string &);
    
    bool select_table_call(std::vector<Token>, std::string &);
    
    bool update_table_call(std::vector<Token>, std::string &);
    
    bool delete_table_call(std::vector<Token>, std::string &);
    
}


#endif /* parser_hpp */

