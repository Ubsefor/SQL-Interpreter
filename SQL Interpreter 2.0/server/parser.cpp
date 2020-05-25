//
//  parser.cpp
//  server
//
//  Created by Rave Botovski on 22.05.2020.
//  Copyright © 2020 Rave Botovski. All rights reserved.
//

#include "Libraries/parser.hpp"

namespace Parser
{
    using namespace Lex;
    
    bool compute( std::vector < Token > expr, std::string &result )
    {
        std::stack < std::string > op_stack;
        try {
            for ( auto i = expr.begin(); i != expr.end(); i++ )
            {
                int res;
                std::string string_res;
                switch ( i->type )
                {
                    case NAME:
                    case SQL_STR:
                    case SQL_LONG:
                        op_stack.push( i->value );
                        break;
                        
                    case ADD:
                        res = std::stoi( op_stack.top() );
                        op_stack.pop();
                        res += std::stoi( op_stack.top() );
                        op_stack.pop();
                        op_stack.push( std::to_string(res) );
                        break;
                        
                    case SUB:
                        res = std::stoi( op_stack.top() );
                        op_stack.pop();
                        res -= std::stoi( op_stack.top() );
                        op_stack.pop();
                        op_stack.push( std::to_string(res) );
                        break;
                        
                    case MULT:
                        res = std::stoi( op_stack.top() );
                        op_stack.pop();
                        res *= std::stoi( op_stack.top() );
                        op_stack.pop();
                        op_stack.push( std::to_string(res) );
                        break;
                        
                    case DIV:
                        res = std::stoi( op_stack.top() );
                        op_stack.pop();
                        res /= std::stoi( op_stack.top() );
                        op_stack.pop();
                        op_stack.push( std::to_string(res) );
                        break;
                        
                    case PERCENT:
                        res = std::stoi( op_stack.top() );
                        op_stack.pop();
                        res %= std::stoi( op_stack.top() );
                        op_stack.pop();
                        op_stack.push( std::to_string(res) );
                        break;
                        
                    case AND:
                        res = std::stoi(op_stack.top() );
                        op_stack.pop();
                        res = std::stoi(op_stack.top() ) && res;
                        op_stack.pop();
                        op_stack.push( std::to_string(res) );
                        break;
                        
                    case OR:
                        res = std::stoi(op_stack.top() );
                        op_stack.pop();
                        res = std::stoi(op_stack.top() ) || res;
                        op_stack.pop();
                        op_stack.push( std::to_string(res) );
                        break;
                        
                    case POW:
                        res = std::stoi( op_stack.top() );
                        op_stack.pop();
                        res ^= std::stoi( op_stack.top() );
                        op_stack.pop();
                        op_stack.push( std::to_string(res) );
                        break;
                        
                    case N_EQUAL:
                        string_res = op_stack.top();
                        op_stack.pop();
                        string_res = std::to_string(op_stack.top() != string_res);
                        op_stack.pop();
                        op_stack.push( string_res );
                        break;
                        
                    case EQUAL:
                        string_res = op_stack.top();
                        op_stack.pop();
                        string_res = std::to_string(op_stack.top() == string_res);
                        op_stack.pop();
                        op_stack.push( string_res );
                        break;
                        
                    case G_EQUAL:
                        string_res = op_stack.top();
                        op_stack.pop();
                        string_res = std::to_string(op_stack.top() >= string_res);
                        op_stack.pop();
                        op_stack.push( string_res );
                        break;
                        
                    case L_EQUAL:
                        string_res = op_stack.top();
                        op_stack.pop();
                        string_res = std::to_string(op_stack.top() <= string_res);
                        op_stack.pop();
                        op_stack.push( string_res );
                        break;
                        
                    case GREATER:
                        string_res = op_stack.top();
                        op_stack.pop();
                        string_res = std::to_string(op_stack.top() > string_res);
                        op_stack.pop();
                        op_stack.push( string_res );
                        break;
                        
                    case LESS:
                        string_res = op_stack.top();
                        op_stack.pop();
                        string_res = std::to_string(op_stack.top() < string_res);
                        op_stack.pop();
                        op_stack.push( string_res );
                        break;
                        
                    case NOT:
                        res = std::stoi( op_stack.top() );
                        op_stack.pop();
                        res = !res;
                        op_stack.push( std::to_string(res) );
                        break;
                        
                    default:
                        result = "Syntax error! Unknown operator in WHERE.";
                        return false;
                }
            }
        } catch (...){
            result = "Syntax error! Wrong operations in WHERE.";
            return false;
        }
        
            // returning result
        result = op_stack.top();
        return true;
    }
    
    void trypop( std::stack < Token > stack )
    {
        if ( stack.empty() )
        {
            throw std::logic_error( "stack underflow, caret not closed" );
        }
    }
    
    std::vector < Token > make_poliz( std::vector < Token > expr )
    {
        std::vector < Token > out;
        std::stack < Token > op_stack;
        
        for ( auto i = expr.begin(); i != expr.end(); i++ )
        {
            switch ( i->type )
            {
                case SQL_STR:
                case SQL_LONG:
                case NAME:
                    out.push_back( *i );
                    break;
                    
                case SUB:
                case ADD:
                    while ( !op_stack.empty() && op_stack.top().type != L_BRACKET )
                    {
                        out.push_back( op_stack.top() );
                        trypop( op_stack );
                        op_stack.pop();
                    }
                    op_stack.push( *i );
                    break;
                    
                case MULT:
                    while ( !op_stack.empty() && op_stack.top().type == MULT )
                    {
                        out.push_back( op_stack.top() );
                        trypop( op_stack );
                        op_stack.pop();
                    }
                    op_stack.push( *i );
                    break;
                    
                case POW:
                case AND:
                case OR:
                    while (
                           ( !op_stack.empty() && op_stack.top().type == AND ) ||
                           ( !op_stack.empty() && op_stack.top().type == OR ) )
                    {
                        out.push_back( op_stack.top() );
                        trypop( op_stack );
                        op_stack.pop();
                    }
                    op_stack.push( *i );
                    break;
                    
                case N_EQUAL:
                case EQUAL:
                    while (
                           ( !op_stack.empty() && op_stack.top().type == N_EQUAL ) ||
                           ( !op_stack.empty() && op_stack.top().type == EQUAL ) )
                    {
                        out.push_back( op_stack.top() );
                        trypop( op_stack );
                        op_stack.pop();
                    }
                    op_stack.push( *i );
                    break;
                    
                case G_EQUAL:
                case L_EQUAL:
                case GREATER:
                case LESS:
                    while (
                           ( !op_stack.empty() && op_stack.top().type == MULT ) ||
                           ( !op_stack.empty() && op_stack.top().type == G_EQUAL ) ||
                           ( !op_stack.empty() && op_stack.top().type == L_EQUAL ) ||
                           ( !op_stack.empty() && op_stack.top().type == GREATER ) ||
                           ( !op_stack.empty() && op_stack.top().type == LESS ) )
                    {
                        out.push_back( op_stack.top() );
                        trypop( op_stack );
                        op_stack.pop();
                    }
                    op_stack.push( *i );
                    break;
                    
                case NOT:
                    while ( !op_stack.empty() && op_stack.top().type != L_BRACKET )
                    {
                        out.push_back( op_stack.top() );
                        trypop( op_stack );
                        op_stack.pop();
                    }
                    op_stack.push( *i );
                    break;
                    
                case R_BRACKET:
                    while ( op_stack.top().type != L_BRACKET )
                    {
                        out.push_back( op_stack.top() );
                        trypop( op_stack );
                        op_stack.pop();
                    }
                    trypop( op_stack );
                    op_stack.pop();
                    break;
                    
                case L_BRACKET:
                    op_stack.push( *i );
                    break;
                    
                default:
                    break;
            }
        }
        
        while ( !op_stack.empty() )
        {
            out.push_back( op_stack.top() );
            op_stack.pop();
        }
        return out;
    }
    
    /*
     
     where – > сделать шаблонный полиз, потом пройтись по всем Записям ТАБЛИЦЫ,
     создать полиз с подставленными значениями полей Записи ТАБЛИЦЫ, скормить это все
     computeP, получить значение true/false и на этой основе выполнить операцию
     над Записью ТАБЛИЦЫ
     
     */
    
    bool preprocess( THandle temp_table, Token &lexem, std::string &res_str )
    {
            //  nothing to preprocess if this is not a field name
        if ( lexem.type != NAME  )
        {
            return 1;
        }
        
            //  getting field type
        FieldType type;
        Errors    err_code = getFieldType( temp_table, lexem.value.c_str(), &type );
        
        if ( err_code != Errors::OK )
        {
            std::stringstream out_stream;
            out_stream << "Failed to identify the type of the \"" << lexem.value << "\" field from the database: " << ErrorText[err_code] << std::endl;
            res_str.clear();
            res_str = out_stream.str();
            closeTable( temp_table );
            return 0;
        }
        
            //  replacing field name with the field value withing the lexem
        char *table_text_value = 0;
        long table_long_value   = 0;
        switch ( type )
        {
            case FieldType::Text:
                err_code = getText( temp_table, lexem.value.c_str(), &table_text_value );
                if ( Errors::OK == err_code )
                {
                    lexem.value = table_text_value;
                    
                }
                break;
                
            case FieldType::Long:
                err_code = getLong( temp_table, lexem.value.c_str(), &table_long_value );
                if ( Errors::OK == err_code )
                {
                    lexem.value = std::to_string( table_long_value );
                    
                }
                break;
                
            default:
                err_code = Errors::BadFieldType;
                break;
        }
        
            //  processing error result
        if ( Errors::OK != err_code )
        {
            std::stringstream out_stream;
            out_stream << "Failed to retrieve database field data for the \"" << lexem.value << "\" field: " << ErrorText[err_code] << std::endl;
            res_str.clear();
            res_str = out_stream.str();
            closeTable( temp_table );
            return 0;
        }
        
            //  successfully preprocessed if reached here
        return 1;
    }
    
    bool preprocess( THandle temp_table, std::vector < Token > &poliz, std::string &res_str )
    {
        for ( auto i = poliz.begin(); i != poliz.end(); i++ )
            if ( !preprocess( temp_table, *i, res_str ) )
                return 0;
        return 1;
    }
    
    bool computeIn( std::string value, std::vector < Token > values, std::string &res_str )
    {
            //  checking if "value" exists in the "values" list
        for ( auto i = values.begin(); i != values.end(); i++ )
        {
            switch ( i->type )
            {
                case SQL_STR:
                case SQL_LONG:
                    if ( value == i->value )
                    {
                        res_str = "1";
                        return 1;
                    }
                    else
                        break;
                default:
                    res_str = "Bad value type in the list of possible values in the IN clause";
                    return 0;
            }
        }
            //  nothing found if got here
        res_str = "0";
        return 1;
    }
    
    void make_cregexp( std::string &str, const std::string &from, const std::string &to )
    {
        if ( from.empty() )
        {
            return;
        }
        size_t startPos = 0;
        while ( ( startPos = str.find( from, startPos ) ) != std::string::npos )
        {
            str.replace( startPos, from.length(), to );
            startPos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
        }
    }
    
    bool compute_like( std::string value, std::string condition, std::string &res_str )
    {
        std::size_t occurance = 0;
        
        make_cregexp( condition, "'", "" );
        make_cregexp( condition, "\\", "\\\\" );
        make_cregexp( condition, "$", "\\$" );
        make_cregexp( condition, ".", "\\." );
        make_cregexp( condition, "*", "\\*" );
        make_cregexp( condition, "+", "\\+" );
        make_cregexp( condition, "?", "\\?" );
        make_cregexp( condition, "{", "\\{" );
        make_cregexp( condition, "}", "\\}" );
        make_cregexp( condition, "|", "\\|" );
        make_cregexp( condition, "(", "\\(" );
        make_cregexp( condition, ")", "\\)" );
        make_cregexp( condition, "%", ".*" );
        make_cregexp( condition, "_", "." );
        
        res_str = std::to_string( std::regex_match( value, std::regex( condition ) ) );
        return 1;
    }
    
    bool process( THandle temp_table, std::vector < Token > poliz, std::string &res_str )
    {
        poliz = make_poliz( poliz );
        std::string strError;
        if ( !preprocess( temp_table, poliz, strError ) )
        {
            res_str = strError;
            closeTable( temp_table );
            temp_table = 0;
            return 0;
        }
        
            //  calculating the resulting poliz
        if ( !compute( poliz, res_str ) )
        {
            closeTable( temp_table );
            temp_table = 0;
            return 0;
        }
        
        return 1;
    }
    
    bool where_clause( THandle temp_table, WhereCondition condition, std::string &res_str )
    {
            //  processing trivial case
        if ( condition.type == ALL )
        {
            res_str = "1";
            return 1;
        }
        
            //  checking if the table is open
        if ( temp_table == 0 )
        {
            res_str = "Process function failed: NULL pointer to the database table provided";
            return 0;
        }
        
            //  rearranging lexems according to the operations priorities
        if (( condition.type == WHERE ) || ( condition.type == IN ) )
            condition.lex_vec1 = make_poliz( condition.lex_vec1 );
        
            //  replacing table field names with the corresponding table values in the 1rst lexem vector
        switch ( condition.type )
        {
            case WHERE:
            case IN:
                if ( !preprocess( temp_table, condition.lex_vec1, res_str ))
                    return 0;
                break;
                
            case LIKE:
                if ( !preprocess( temp_table, condition.lex_token1, res_str ) )
                    return 0;
                break;
                
            case ALL:
                break;
                
            default:
                res_str = "Bad WHERE result type value";
                return 0;
        }
        
            //  calculating condition value
        switch ( condition.type )
        {
            case WHERE:
                if ( !compute( condition.lex_vec1, res_str ) )
                    return 0;
                break;
                
            case IN:
                if ( !compute( condition.lex_vec1, res_str ) )
                    return 0;
                if ( !computeIn( res_str, condition.lex_vec2, res_str ) )
                    return 0;
                break;
                
            case LIKE:
                if ( !compute_like( condition.lex_token1.value, condition.lex_token2.value, res_str ) )
                    return 0;
                break;
                
            case ALL:
                res_str = "1";
                break;
                
            default:
                res_str = "Bad WHERE result type value";
                return 0;
        }
        
            //  applying the NOT condition if necessary
        if ( condition.Not )
            switch ( condition.type )
            {
                case IN:
                case LIKE:
                    if ( res_str == "0" )
                        res_str = "1";
                    else if ( res_str == "1" )
                        res_str = "0";
                    else
                        return 0;
                    break;
            }
        
            //  everything is OK if reached here
        return 1;
    }
    
    bool get_field_names( THandle temp_table, std::vector < std::string > &field_names, std::string &error )
    {
            //  resetting field names array
        field_names.clear();
        
            //  getting table fields count
        unsigned count     = 0;
        Errors   err_code = getFieldsNum( temp_table, &count );
        if ( err_code != Errors::OK )
        {
            std::stringstream err;
            err << "Failed to get the number of table fields: " << ErrorText[err_code];
            error.clear();
            error = err.str();
            return 0;
        }
        else if ( count == 0 )
        {
            return 1;
        }
        
            //  listing tabel field names
        char *tableFieldName = 0;
        for ( int i = 0; i < count; i++ )
        {
            err_code = getFieldName( temp_table, i, &tableFieldName );
            if ( Errors::OK == err_code )
                field_names.push_back( tableFieldName );
            else
            {
                std::stringstream err;
                err << "Failed to get the name of the table field #" << std::to_string( i ) << ": " << ErrorText[err_code];
                error.clear();
                error = err.str();
                
                field_names.clear();
                return 0;
            }
        }
        
            //  successfully exiting
        return 1;
    }
    
    bool exec_new( std::string table_name, std::vector < struct FieldDef > fields, std::string &error )
    {
            //  checking if we have table structure
        if ( fields.size() == 0)
        {
            std::stringstream err;
            err << "Failed to CREATE table " << table_name << ": No table fields specified";
            error.clear();
            error = err.str();
            return 0;
        }
        
            //  assembling table header structure
        TableStruct struTable;
        struTable.numOfFields = fields.size();
        struTable.fieldsDef   = &fields[0];
        
            //  actually, creating the table
        Errors err_code = createTable( table_name.c_str(), &struTable );
        if ( err_code == Errors::OK )
            return 1;
        else
        {
            std::stringstream err;
            err << "Failed to CREATE TABLE " << table_name << ": " << ErrorText[err_code];
            error.clear();
            error = err.str();
            return 0;
        }
    }
    
    bool exec_rm( std::string table_name, std::string &error )
    {
        Errors err_code = deleteTable( table_name.c_str() );
        if ( Errors::OK == err_code )
            return 1;
        else
        {
            std::stringstream out_stream;
            std::cout << "DROP" << std::endl;
            out_stream << "Failed to DROP the " << table_name << " table file: " << ErrorText[err_code] << std::endl;
            error.clear();
            error = out_stream.str();
            return 0;
        }
    }
    
    bool exec_ins( std::string table_name, std::vector < struct InsertField > fields, std::string &res_str )
    {
            // opening the table created
        THandle temp_table    = 0;
        Errors  err_code = openTable( table_name.c_str(), &temp_table );
        if ( Errors::OK != err_code )
        {
            std::stringstream out_stream;
            out_stream << "Failed to open the " << table_name << " table file: " << ErrorText[err_code] << std::endl;
            res_str.clear();
            res_str = out_stream.str();
            return 0;
        }
        
            // creating a new record
        err_code = createNew( temp_table );
        if ( Errors::OK != err_code )
        {
            std::stringstream out_stream;
            out_stream << "Failed to create new record: " << ErrorText[err_code] << std::endl;
            res_str.clear();
            res_str = out_stream.str();
            closeTable( temp_table );
            return 0;
        }
        
            // looping throught the fields array
        unsigned number       = 0;
        char     *table_field_name = 0;
        for ( auto i = fields.begin(); i != fields.end(); i++ )
        {
                // getting the name of the field
            err_code = getFieldName( temp_table, number, &table_field_name );
            if ( Errors::OK != err_code )
            {
                std::stringstream out_stream;
                out_stream << "Failed to get the name of the \"" << number << "\" field from the database: " << ErrorText[err_code] << std::endl;
                res_str.clear();
                res_str = out_stream.str();
                closeTable( temp_table );
                return 0;
            }
            else
                number++;
            
                // sending field data to the database
            switch ( i->type )
            {
                case FieldType::Text:
                    err_code = putTextNew( temp_table, table_field_name, i->str.c_str() );
                    break;
                    
                case FieldType::Long:
                    err_code = putLongNew( temp_table, table_field_name, i->num );
                    break;
                    
                default:
                    err_code = Errors::BadFieldType;
                    break;
            }
            
                //  analyzing error code and send back result
            if ( Errors::OK != err_code )
            {
                std::stringstream out_stream;
                out_stream << "Failed to send the \"" << i->str << "\" field data to the database: " << ErrorText[err_code] << std::endl;
                res_str.clear();
                res_str = out_stream.str();
                closeTable( temp_table );
                return 0;
            }
        }
        
            //  finally, submitting the entire record to the database
        err_code = insertzNew( temp_table );
        if ( Errors::OK != err_code )
        {
            std::stringstream out_stream;
            out_stream << "Failed to submit inserted record to the database table: " << ErrorText[err_code] << std::endl;
            res_str.clear();
            res_str = out_stream.str();
            closeTable( temp_table );
            return 0;
        }
        
            //  closing the table and exiting
        closeTable( temp_table );
        return 1;
    }
    
    bool exec_del( std::string table_name, WhereCondition whereCond, std::string &res_str )
    {
            //  opening the table created
        THandle temp_table    = 0;
        Errors  err_code = openTable( table_name.c_str(), &temp_table );
        if ( Errors::OK != err_code )
        {
            std::stringstream out_stream;
            out_stream << "Failed to open the " << table_name << " table file: " << ErrorText[err_code] << std::endl;
            res_str.clear();
            res_str = out_stream.str();
            return 0;
        }
        
            //  moving cursor to the 1rst table record
        err_code = moveFirst( temp_table );
        if ( Errors::OK != err_code )
        {
            closeTable( temp_table );
            temp_table = 0;
            std::stringstream out_stream;
            out_stream << "Failed to open the " << table_name << " table file: " << ErrorText[err_code] << std::endl;
            res_str.clear();
            res_str = out_stream.str();
            
            return 0;
        }
        
            //  looping through records
        while ( !afterLast( temp_table ) )
        {
                //  checking if the current record meets the "where" condition
            std::string whereResult;
            if ( !where_clause( temp_table, whereCond, whereResult ) )
            {
                std::cout << temp_table << std::endl;
                closeTable( temp_table );
                temp_table = 0;
                return 0;
            }
            else if ( whereResult == "1" )
            {
                    //  deleting the record
                err_code = deleteRec( temp_table );
                if ( err_code != Errors::OK)
                {
                    closeTable( temp_table );
                    temp_table = 0;
                    std::stringstream out_stream;
                    out_stream << "Failed to delete record: " << ErrorText[err_code] << std::endl;
                    res_str.clear();
                    res_str = out_stream.str();
                    return 0;
                }
            }
            
            moveNext( temp_table );
        }
        
            //  closing the table and exiting
        closeTable( temp_table );
        return 1;
    }
    
    bool exec_upd( std::string table_name, PolizExpr got_poliz, WhereCondition where_cond, std::string &res_str )
    {
            //  opening the table
        THandle temp_table    = 0;
        Errors  err_code = openTable( table_name.c_str(), &temp_table );
        if ( err_code != Errors::OK )
        {
            std::stringstream out_stream;
            out_stream << "Failed to open the " << table_name << " table file: " << ErrorText[err_code] << std::endl;
            res_str.clear();
            res_str = out_stream.str();
            return 0;
        }
        
            //  moving cursor to the 1st table record
        err_code = moveFirst( temp_table );
        if ( err_code != Errors::OK )
        {
            closeTable( temp_table );
            temp_table = 0;
            std::stringstream out_stream;
            out_stream << "Failed to open the " << table_name << " table file: " << ErrorText[err_code] << std::endl;
            res_str.clear();
            res_str = out_stream.str();
            
            return 0;
        }
        
            //  looping through records
        while ( !afterLast( temp_table ) )
        {
                //  checking if the current record meets the "where" condition
            std::string whereResult;
            if ( !where_clause( temp_table, where_cond, whereResult ) )
            {
                std::cout << temp_table << std::endl;
                closeTable( temp_table );
                temp_table = 0;
                return 0;
            }
            else if ( whereResult != "1" )
            {
                moveNext( temp_table );
                continue;
            }
            
                //  opening the record for editing
            err_code = startEdit( temp_table );
            if ( err_code != Errors::OK )
            {
                closeTable( temp_table );
                temp_table = 0;
                std::stringstream out_stream;
                out_stream << "Failed to open the table record for editing: " << ErrorText[err_code] << std::endl;
                res_str.clear();
                res_str = out_stream.str();
                return 0;
            }
            
                
            std::string poliz_result;
                    //  calculating the poliz to put result to the table
            if ( !process( temp_table, got_poliz.poliz, poliz_result ) )
                {
                    closeTable( temp_table );
                    temp_table = 0;
                    std::stringstream out_stream;
                    out_stream << "Failed to process poliz for the: " << got_poliz.fieldName << "field of the " << table_name << " table: " << poliz_result;
                    res_str.clear();
                    res_str = out_stream.str();
                    return 0;
                }
                
                    //  getting corresponding field type
                FieldType type;
            err_code = getFieldType( temp_table, got_poliz.fieldName.c_str(), &type );
                if ( err_code != Errors::OK )
                {
                    closeTable( temp_table );
                    temp_table = 0;
                    std::stringstream out_stream;
                    out_stream << "Failed to read the " << got_poliz.fieldName << " field type from the " << table_name << " table: " << ErrorText[err_code] << std::endl;
                    res_str.clear();
                    res_str = out_stream.str();
                    return 0;
                }
                
                    //  sending field data to the database
                switch ( type )
                {
                    case FieldType::Text:
                        err_code = putText( temp_table, got_poliz.fieldName.c_str(), poliz_result.c_str() );
                        break;
                        
                    case FieldType::Long:
                        err_code = putLong( temp_table, got_poliz.fieldName.c_str(), atol( poliz_result.data() ) );
                        break;
                        
                    default:
                        err_code = Errors::BadFieldType;
                        break;
                }
                
                    //  analyzing data send result
                if ( err_code != Errors::OK )
                {
                    std::stringstream out_stream;
                    out_stream << "Failed to send the \"" << got_poliz.fieldName << "\" field data to the database: " << ErrorText[err_code] << std::endl;
                    res_str.clear();
                    res_str = out_stream.str();
                    closeTable( temp_table );
                    return 0;
                }
            
            
                //  submitting the entire record to the database
            err_code = finishEdit( temp_table );
            if ( err_code != Errors::OK )
            {
                std::stringstream out_stream;
                out_stream << "Failed to submit updated record to the database table: " << ErrorText[err_code] << std::endl;
                res_str.clear();
                res_str = out_stream.str();
                closeTable( temp_table );
                return 0;
            }
            
                //  moving to the next table record
            moveNext( temp_table );
        }
        
            //  closing the table and exiting
        closeTable( temp_table );
        
        return 1;
    }
    
    bool exec_sel( std::vector < std::string > field_names, std::string table_name, WhereCondition where_cond, std::string &res_str )
    {
        bool look_all = field_names[0] == "*";
        
            //  opening the table created
        THandle temp_table    = 0;
        Errors  err_code = openTable( table_name.c_str(), &temp_table );
        if ( Errors::OK != err_code )
        {
            std::stringstream out_stream;
            std::cout << "Where" << std::endl;
            out_stream << "Failed to open the " << table_name << " table file: " << ErrorText[err_code] << std::endl;
            res_str.clear();
            res_str = out_stream.str();
            return 0;
        }
        
        
        
            //  getting table field names if necessary
        std::string error;
        bool test = get_field_names( temp_table, field_names, error );
        if (( field_names.size() == 0 ) && ( !test ) )
        {
            closeTable( temp_table );
            temp_table          = 0;
            res_str = error;
            return 0;
        }
        if ( field_names.size() == 0 && !look_all )
        {
            closeTable( temp_table );
            temp_table = 0;
            
            std::stringstream out_stream;
            out_stream << "The " << table_name << " table does not have fields" << std::endl;
            res_str.clear();
            res_str = out_stream.str();
            
            return 0;
        }
        
            //  moving cursor to the 1rst table record
        err_code = moveFirst( temp_table );
        if ( err_code != Errors::OK )
        {
            closeTable( temp_table );
            temp_table = 0;
            std::stringstream out_stream;
            out_stream << "Failed to open the " << table_name << " table file: " << ErrorText[err_code] << std::endl;
            res_str.clear();
            res_str = out_stream.str();
            
            return 0;
        }
        
            //  looping through records
        char *t_field_value = 0;
        long l_field_value   = 0;
        while ( !afterLast( temp_table ) )
        {
                //  checking the "where" condition for record
            std::string whereResult;
            if ( !where_clause( temp_table, where_cond, whereResult ) )
            {
                std::cout << temp_table << std::endl;
                closeTable( temp_table );
                temp_table = 0;
                return 0;
            }
            else if ( whereResult != "1" )
            {
                moveNext( temp_table );
                continue;
            }
            
                //  getting current record data
            for ( auto i = field_names.begin(); i != field_names.end(); i++ )
            {
                    //  getting corresponding field type
                FieldType type;
                err_code = getFieldType( temp_table, i->c_str(), &type );
                if ( err_code != Errors::OK )
                {
                    closeTable( temp_table );
                    temp_table = 0;
                    
                    std::stringstream out_stream;
                    out_stream << "Failed to read the " << i->c_str() << " field type from the " << table_name << " table: " << ErrorText[err_code] << std::endl;
                    res_str.clear();
                    res_str = out_stream.str();
                    
                    return 0;
                }
                
                    //  getting field value
                std::string value;
                switch ( type )
                {
                    case FieldType::Text:
                        err_code = getText( temp_table, i->c_str(), &t_field_value );
                        value     = t_field_value;
                        break;
                    case FieldType::Long:
                        err_code = getLong( temp_table, i->c_str(), &l_field_value );
                        value     = std::to_string( l_field_value );
                        break;
                    default:
                        err_code = Errors::BadFieldType;
                        break;
                }
                
                    //  adding field value retrieved to the result string
                if ( Errors::OK == err_code )
                {
                        //  adding field or record separator to the result string
                    if ( i != field_names.begin() )
                    {
                        res_str = res_str + "\t";
                    }
                    res_str = res_str + value;
                }
                else
                {
                    closeTable( temp_table );
                    temp_table = 0;
                    
                    std::stringstream out_stream;
                    out_stream << "Failed to read the " << i->data() << " field value from the " << table_name << " table: " << ErrorText[err_code] << std::endl;
                    res_str.clear();
                    res_str = out_stream.str();
                    
                    return 0;
                }
            }
            res_str = res_str + "\n";
            moveNext( temp_table );
        }
        
            //  closing the table
        closeTable( temp_table );
        temp_table = NULL;
        return 1;
    }
    
    bool check_sentence (std::vector<Token> got_tokens){
        auto iter = got_tokens.begin();
        while (iter->type != END){
            if (iter->type == ERR){
                return false;
            }
            iter++;
        }
        return true;
    }
    
    
    bool parse_sentence (std::vector<Token> got_tokens, std::string &result)
    {
        auto iter = got_tokens.begin() + 1;
        bool ans;
        switch (got_tokens[0].type) {
                
            case SELECT:
                ans = select_table_call(got_tokens, result);
                return ans;
                break;
                
            case INSERT:
                ans = insert_table_call(got_tokens, result);
                return ans;
                break;
            
            case UPDATE:
                ans = update_table_call(got_tokens, result);
                return ans;
                break;
                
            case DELETE:
                ans = delete_table_call(got_tokens, result);
                return ans;
                break;
                
            case CREATE:
                ans = create_table_call(got_tokens, result);
                return ans;
                break;
                
            case DROP:
                ans = drop_table_call(got_tokens, result);
                return ans;
                break;
                
            default:
                result = "Syntax error! No such operation.";
                break;
        }
            // something went wrong if we got here
        result = "Unknown Error!";
        return false;
        
    }
    
    bool create_table_call(std::vector<Token> got_tokens, std::string &result)
    {
        auto iter = got_tokens.begin() + 1;
        if (iter->type != TABLE){
            result = "Syntax error! Expected a TABLE keyword after CREATE.";
            return false;
        }
        iter++;
        if (iter->type != NAME){
            result = "Syntax error! Expected a table name after TABLE.";
            return false;
        }
        std::string table = iter->value;
        iter++;
        if (iter->type != L_BRACKET){
            result = "Syntax error! Expected an opening bracket to pass field list.";
            return false;
        }
        iter++;
        
        FieldDef temp;
        memset(temp.name, 0, 30);
        temp.len = 0;
        temp.type = Long;
        
        std::vector <FieldDef> def_vec;
        
        while (iter->type != R_BRACKET){
            
            if (iter->type != NAME){
                result = "Syntax error! Expected a field name.";
                return false;
            }
                // Yes, I simply cut all letters above name size because why tf not
            strcpy(temp.name, iter->value.c_str());
            iter++;
            switch (iter->type) {
                    
                case TEXT:
                    temp.type = Text;
                    iter++;
                    if (iter->type != L_BRACKET){
                        result = "Syntax error! Expected '(' .";
                        return false;
                    };
                    iter++;
                    if (iter->type != SQL_LONG){
                        result = "Syntax error! Expected field size.";
                        return false;
                    }
                    temp.len = std::stoi(iter->value);
                    iter++;
                    if (iter->type != R_BRACKET){
                        result = "Syntax error! Expected ')' .";
                        return false;
                    }
                    break;
                    
                case LONG:
                    temp.type = Long;
                    break;
                    
                default:
                    result = "Syntax error! Expected a field type after it's name.";
                    return false;
                    break;
            }
            
            def_vec.push_back(temp);
            temp.len = 0;
            temp.type = Long;
            memset(temp.name, 0, 30);
            iter++;
            if (iter->type == COMMA){
                iter++;
                continue;
            } else if (iter->type != R_BRACKET){
                result = "Syntax error! Expected a closing bracket.";
                return false;
            }
        }
        
        iter++;
        if (iter->type != END){
            result = "Unexpected text after last closing bracket!";
            return false;
        }
        
            // calling exec function
        return exec_new(table, def_vec, result);
    }
    
    bool insert_table_call(std::vector<Token> got_tokens, std::string &result)
    {
        auto iter = got_tokens.begin() + 1;
        if (iter->type != INTO){
            result = "Syntax error! Expected a INTO keyword after INSERT.";
            return false;
        }
        iter++;
        if (iter->type != NAME){
            result = "Syntax error! Expected a table name after INTO.";
            return false;
        }
        std::string table = iter->value;
        iter++;
        if (iter->type != L_BRACKET){
            result = "Syntax error! Expected an opening bracket to pass field list.";
            return false;
        }
        iter++;
        
        /*
         struct InsertField
         {
            FieldType    type;
            std::string  str;
            unsigned int num;
         };
        */
        
        std::vector<InsertField> fields;
        InsertField temp;
        temp.num = 0;
        temp.str = "";
        temp.type = Logic;
        
        while (iter->type != R_BRACKET){
            switch (iter->type) {
                case SQL_STR:
                    temp.type = Text;
                    temp.str = iter->value;
                    fields.push_back(temp);
                    temp.str = "";
                    iter++;
                    if (iter->type == COMMA){
                        iter++;
                    }
                    break;
                    
                case SQL_LONG:
                    temp.type = Long;
                    temp.num = std::stoi(iter->value);
                    fields.push_back(temp);
                    temp.str = "";
                    iter++;
                    if (iter->type == COMMA){
                        iter++;
                    }
                    break;
                    
                case R_BRACKET:
                    break;
                    
                default:
                    result = "Syntax error! Expected either a string in hypfens or a long value.";
                    return false;
                    break;
            }
            
        }
            // calling exec function
        return exec_ins(table, fields, result);
    }
    
    bool drop_table_call(std::vector<Token> got_tokens, std::string &result)
    {
        if (got_tokens[1].type != NAME){
            result = "Syntax error! Expected a table name after DROP";
            return false;
        }
        return exec_rm(got_tokens[1].value, result);
        
    }
    
    bool select_table_call(std::vector<Token> got_tokens, std::string &result)
    {
        auto iter = got_tokens.begin() + 1;
        std::vector <std::string> field_names;
        std::string table_name;
        if (iter->type != NAME && iter->type != MULT ){
            result = "Syntax error! Expected field name(s) after select!";
            return false;
        }
        
        while (iter->type != FROM) {
            switch (iter->type) {
                case MULT:
                case NAME:
                    field_names.push_back(iter->value);
                    iter++;
                    if (iter->type == COMMA){
                        iter++;
                    }
                    break;
                    
                default:
                    result = "Syntax error! Expected field name(s) after SELECT!";
                    return false;
                    break;
            }
        }
        //  iter type == from
        iter++;
        if (iter->type != NAME){
            result = "Syntax error! Expected table name after FROM!";
            return false;
        }
        table_name = iter->value;
        
        //  parsing where
        
        iter++;
        if (iter->type != WHERE){
            result = "Syntax error! Expected WHERE after table name!";
            return false;
        }
        WhereCondition passed_where_expr;
        
            // ALL type
        if ((iter+1)->type == Lex::ALL)
        {
            passed_where_expr.type = ALL;
            passed_where_expr.Not = 0;
            return exec_sel(field_names, table_name, passed_where_expr, result);
        }
        
            // LIKE type
        if ( ( ( iter + 1 )->type == NAME ) && (( iter + 2 )->type == NOT || (iter + 2)->type == LIKE ) )
        {
            iter++;
            passed_where_expr.Not = 0;
            passed_where_expr.type = LIKE;
                // field name
            passed_where_expr.lex_token1 = *iter;
            iter++;
            if (iter->type == NOT) {
                passed_where_expr.Not = 1;
                iter++;
            }
            if (iter->type != LIKE){
                result = "Syntax error! Expected LIKE after NOT!";
                return false;
            }
            iter++;
            if (iter->type != SQL_STR){
                result = "Syntax error! Expected template string after LIKE";
                return false;
            }
            passed_where_expr.lex_token2 = *iter;
            return exec_sel(field_names, table_name, passed_where_expr, result);
        }
        
            // save iterator at token after WHERE for ez parsing afterwards
        auto iter_at_where = (iter + 1);
        
            // IN type
        while (iter->type != END){
            if (iter->type == IN){
                passed_where_expr.type = IN;
                if ((iter - 1)->type == NOT ) passed_where_expr.Not = 1;
                iter++;
                if (iter->type != L_BRACKET){
                    result = "Syntax error! Expected a list of constants after IN";
                    return false;
                }
                
                iter++;
                while ( iter->type != R_BRACKET ){
                    switch (iter->type) {
                        case SQL_LONG:
                            passed_where_expr.lex_vec2.push_back(*iter);
                            iter++;
                            if (iter->type == COMMA) iter++;
                            break;
                            
                        case SQL_STR:
                            passed_where_expr.lex_vec2.push_back(*iter);
                            iter++;
                            if (iter->type == COMMA) iter++;
                            break;
                            
                        case R_BRACKET:
                            break;
                            
                        default:
                            result = "Syntax error! Expected a text or numeric constant";
                            return false;
                            break;
                    }
                }
                iter++;
                
                if (iter->type != END ){
                    result = "Syntax error! Unexpected text after constant list.";
                    return false;
                }
                
                    // passing before IN contents
                iter = iter_at_where;
                while (iter->type != IN){
                    switch (iter->type) {
                            
                        case Lex::SQL_STR:
                        case Lex::SQL_LONG:
                        case Lex::L_BRACKET:
                        case Lex::R_BRACKET:
                        case Lex::ADD:
                        case Lex::SUB:
                        case Lex::DIV:
                        case Lex::PERCENT:
                        case Lex::MULT:
                        case Lex::NAME:
                        case NOT:
                            passed_where_expr.lex_vec1.push_back(*iter);
                            iter++;
                            break;
                            
                        default:
                            result = "Syntax error! Unexpected tokens in expression.";
                            return false;
                            break;
                    }
                    
                }
                
                return exec_sel(field_names, table_name, passed_where_expr, result);
                break;
            }
            iter++;
        }
        
            // Logical
        iter = iter_at_where;
        passed_where_expr.type = WHERE;
        while (iter->type != END) {
            switch (iter->type) {
                case Lex::SELECT:
                case Lex::UPDATE:
                case Lex::DELETE:
                case Lex::CREATE:
                case Lex::INSERT:
                case Lex::DROP:
                case Lex::IN:
                case Lex::LIKE:
                case Lex::TABLE:
                case Lex::FROM:
                case Lex::INTO:
                case Lex::TEXT:
                case Lex::LONG:
                case Lex::ALL:
                case Lex::SET:
                case Lex::ERR:
                case Lex::R_SQ_BRACKET:
                case Lex::L_SQ_BRACKET:
                    result = "Syntax error! Illegal WHERE sentence.";
                    return false;
                    break;
                    
                default:
                    passed_where_expr.lex_vec1.push_back(*iter);
                    iter++;
                    break;
            }
        }
        
        return exec_sel(field_names, table_name, passed_where_expr, result);
    }
    
    bool delete_table_call(std::vector<Token> got_tokens, std::string &result)
    {
        auto iter = got_tokens.begin() + 1;
        if (iter->type != FROM){
            result = "Syntax error! Expected FROM after delete.";
            return false;
        }
        iter++;
        std::string table_name;
        if (iter->type != NAME){
            result = "Syntax error! Expected table name after FROM.";
            return false;
        }
        table_name = iter->value;
        
        
            // parsing where
        iter++;

        if (iter->type != WHERE){
            result = "Syntax error! Expected WHERE after table name!";
            return false;
        }
        WhereCondition passed_where_expr;
        
            // ALL type
        if ((iter+1)->type == Lex::ALL)
        {
            passed_where_expr.type = ALL;
            passed_where_expr.Not = 0;
            return exec_del(table_name, passed_where_expr, result);
        }
        
            // LIKE type
        if ( ( ( iter + 1 )->type == NAME ) && (( iter + 2 )->type == NOT || (iter + 2)->type == LIKE ) )
        {
            iter++;
            passed_where_expr.Not = 0;
            passed_where_expr.type = LIKE;
                // field name
            passed_where_expr.lex_token1 = *iter;
            iter++;
            if (iter->type == NOT) {
                passed_where_expr.Not = 1;
                iter++;
            }
            if (iter->type != LIKE){
                result = "Syntax error! Expected LIKE after NOT!";
                return false;
            }
            iter++;
            if (iter->type != SQL_STR){
                result = "Syntax error! Expected template string after LIKE";
                return false;
            }
            passed_where_expr.lex_token2 = *iter;
            return exec_del(table_name, passed_where_expr, result);
        }
        
            // save iterator at token after WHERE for ez parsing afterwards
        auto iter_at_where = (iter + 1);
        
            // IN type
        while (iter->type != END){
            if (iter->type == IN){
                passed_where_expr.type = IN;
                if ((iter - 1)->type == NOT ) passed_where_expr.Not = 1;
                iter++;
                if (iter->type != L_BRACKET){
                    result = "Syntax error! Expected a list of constants after IN";
                    return false;
                }
                
                iter++;
                while ( iter->type != R_BRACKET ){
                    switch (iter->type) {
                        case SQL_LONG:
                            passed_where_expr.lex_vec2.push_back(*iter);
                            iter++;
                            if (iter->type == COMMA) iter++;
                            break;
                            
                        case SQL_STR:
                            passed_where_expr.lex_vec2.push_back(*iter);
                            iter++;
                            if (iter->type == COMMA) iter++;
                            break;
                            
                        case R_BRACKET:
                            break;
                            
                        default:
                            result = "Syntax error! Expected a text or numeric constant";
                            return false;
                            break;
                    }
                }
                iter++;
                
                if (iter->type != END ){
                    result = "Syntax error! Unexpected text after constant list.";
                    return false;
                }
                
                    // passing before IN contents
                iter = iter_at_where;
                while (iter->type != IN){
                    switch (iter->type) {
                            
                        case Lex::SQL_STR:
                        case Lex::SQL_LONG:
                        case Lex::L_BRACKET:
                        case Lex::R_BRACKET:
                        case Lex::ADD:
                        case Lex::SUB:
                        case Lex::DIV:
                        case Lex::PERCENT:
                        case Lex::MULT:
                        case Lex::NAME:
                        case NOT:
                            passed_where_expr.lex_vec1.push_back(*iter);
                            iter++;
                            break;
                            
                        default:
                            result = "Syntax error! Unexpected tokens in expression.";
                            return false;
                            break;
                    }
                    
                }
                
                return exec_del(table_name, passed_where_expr, result);
                break;
            }
            iter++;
        }
        
            // Logical
        iter = iter_at_where;
        passed_where_expr.type = WHERE;
        while (iter->type != END) {
            switch (iter->type) {
                case Lex::SELECT:
                case Lex::UPDATE:
                case Lex::DELETE:
                case Lex::CREATE:
                case Lex::INSERT:
                case Lex::DROP:
                case Lex::IN:
                case Lex::LIKE:
                case Lex::TABLE:
                case Lex::FROM:
                case Lex::INTO:
                case Lex::TEXT:
                case Lex::LONG:
                case Lex::ALL:
                case Lex::SET:
                case Lex::ERR:
                case Lex::R_SQ_BRACKET:
                case Lex::L_SQ_BRACKET:
                    result = "Syntax error! Illegal WHERE sentence.";
                    return false;
                    break;
                    
                default:
                    passed_where_expr.lex_vec1.push_back(*iter);
                    iter++;
                    break;
            }
        }
        
        return exec_del(table_name, passed_where_expr, result);
    }
    
    
    bool update_table_call( std::vector<Token> got_tokens, std::string &result )
    {
        auto iter = got_tokens.begin() + 1;
        PolizExpr passed_poliz_expr;
        
        if (iter->type != NAME){
            result = "Syntax error! Expected a table name after UPDATE.";
            return false;
        }
        std::string table_name = iter->value;
        iter++;
        if (iter->type != SET){
            result = "Syntax error! Expected SET after table name.";
            return false;
        }
        iter++;
        if (iter->type != NAME){
            result = "Syntax error! Expected a field name after SET.";
            return false;
        }
        passed_poliz_expr.fieldName = iter->value;
        iter++;
        if (iter->type != EQUAL){
            result = "Syntax error! Expected '=' after field name.";
            return false;
        }
        iter++;
        while (iter->type != WHERE && iter->type != END){
            passed_poliz_expr.poliz.push_back(*iter);
            iter++;
        }
            
            // parsing where
        if (iter->type != WHERE){
            result = "Syntax error! Expected WHERE.";
            return false;
        }
        
        WhereCondition passed_where_expr;
        
            // ALL type
        if ((iter+1)->type == Lex::ALL)
        {
            passed_where_expr.type = ALL;
            passed_where_expr.Not = 0;
            return exec_upd(table_name, passed_poliz_expr, passed_where_expr, result);
        }
        
            // LIKE type
        if ( ( ( iter + 1 )->type == NAME ) && (( iter + 2 )->type == NOT || (iter + 2)->type == LIKE ) )
        {
            iter++;
            passed_where_expr.Not = 0;
            passed_where_expr.type = LIKE;
                // field name
            passed_where_expr.lex_token1 = *iter;
            iter++;
            if (iter->type == NOT) {
                passed_where_expr.Not = 1;
                iter++;
            }
            if (iter->type != LIKE){
                result = "Syntax error! Expected LIKE after NOT!";
                return false;
            }
            iter++;
            if (iter->type != SQL_STR){
                result = "Syntax error! Expected template string after LIKE";
                return false;
            }
            passed_where_expr.lex_token2 = *iter;
            return exec_upd(table_name, passed_poliz_expr, passed_where_expr, result);
        }
        
            // save iterator at token after WHERE for ez parsing afterwards
        auto iter_at_where = (iter + 1);
        
            // IN type
        while (iter->type != END){
            if (iter->type == IN){
                passed_where_expr.type = IN;
                if ((iter - 1)->type == NOT ) passed_where_expr.Not = 1;
                iter++;
                if (iter->type != L_BRACKET){
                    result = "Syntax error! Expected a list of constants after IN";
                    return false;
                }
                
                iter++;
                while ( iter->type != R_BRACKET ){
                    switch (iter->type) {
                        case SQL_LONG:
                            passed_where_expr.lex_vec2.push_back(*iter);
                            iter++;
                            if (iter->type == COMMA) iter++;
                            break;
                            
                        case SQL_STR:
                            passed_where_expr.lex_vec2.push_back(*iter);
                            iter++;
                            if (iter->type == COMMA) iter++;
                            break;
                            
                        case R_BRACKET:
                            break;
                            
                        default:
                            result = "Syntax error! Expected a text or numeric constant";
                            return false;
                            break;
                    }
                }
                iter++;
                
                if (iter->type != END ){
                    result = "Syntax error! Unexpected text after constant list.";
                    return false;
                }
                
                    // passing before IN contents
                iter = iter_at_where;
                while (iter->type != IN){
                    switch (iter->type) {
                            
                        case Lex::SQL_STR:
                        case Lex::SQL_LONG:
                        case Lex::L_BRACKET:
                        case Lex::R_BRACKET:
                        case Lex::ADD:
                        case Lex::SUB:
                        case Lex::DIV:
                        case Lex::PERCENT:
                        case Lex::MULT:
                        case Lex::NAME:
                        case NOT:
                            passed_where_expr.lex_vec1.push_back(*iter);
                            iter++;
                            break;
                            
                        default:
                            result = "Syntax error! Unexpected tokens in expression.";
                            return false;
                            break;
                    }
                    
                }
                
                return exec_upd(table_name, passed_poliz_expr, passed_where_expr, result);
                break;
            }
            iter++;
        }
        
            // Logical
        iter = iter_at_where;
        passed_where_expr.type = WHERE;
        while (iter->type != END) {
            switch (iter->type) {
                case Lex::SELECT:
                case Lex::UPDATE:
                case Lex::DELETE:
                case Lex::CREATE:
                case Lex::INSERT:
                case Lex::DROP:
                case Lex::IN:
                case Lex::LIKE:
                case Lex::TABLE:
                case Lex::FROM:
                case Lex::INTO:
                case Lex::TEXT:
                case Lex::LONG:
                case Lex::ALL:
                case Lex::SET:
                case Lex::ERR:
                case Lex::R_SQ_BRACKET:
                case Lex::L_SQ_BRACKET:
                    result = "Syntax error! Illegal WHERE sentence.";
                    return false;
                    break;
                    
                default:
                    passed_where_expr.lex_vec1.push_back(*iter);
                    iter++;
                    break;
            }
        }
        
        return exec_upd(table_name, passed_poliz_expr, passed_where_expr, result);
    }
}
