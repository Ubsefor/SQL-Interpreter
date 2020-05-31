#  SQL INTERPRETER 2.0

## This is a readme file, will write it later

![](https://github.com/Ubsefor/SQL-Interpreter/workflows/xcode/badge.svg)

## Building:

make all

tested on Mac OS 10.15.5, Ubuntu 18.04

## Launching:

./server.uex for server

./client.uex for client

Follow instructions in client

## Grammar

### Use this language description to work with DB:

---

< SQL_SENTENCE > -> < SELECT_SENTENCE > | < INSERT_SENTENCE > | < UPDATE_SENTENCE > | < DELETE_SENTENCE > | < CREATE_SENTENCE > | < DROP_SENTENCE >

< SELECT_SENTENCE > -> SELECT < FIELD_LIST > FROM < NAME > < WHERE_CLAUSE >

< FIELD_LIST > -> < NAME > {, < NAME >} | *
< NAME > -> < all printable exept for '> {< all printable exept for '>}

< INSERT_SENTENCE > -> INSERT INTO < NAME > (< FIELD_VALUE > {, < FIELD_VALUE >})

< FIELD_VALUE > -> < SQL_STR > | < SQL_LONG >
< SQL_STR > -> '< SQL_C >{< SQL_C >}'
< SQL_C > -> < all printable exept for ' >

< UPDATE_SENTENCE > -> UPDATE < NAME > SET < NAME > = < EXPRESSION > < WHERE_CLAUSE >
    
< DELETE_SENTENCE > -> DELETE FROM < NAME >  < WHERE_CLAUSE >

< CREATE_SENTENCE > -> CREATE TABLE < NAME > (< LIST_DEFINITIONS >)

< LIST_DEFINITIONS > -> < DEF_FIELD > {, < DEF_FIELD >}
< DEF_FIELD > -> < NAME > < FIELD_TYPE >
< FIELD_TYPE > -> TEXT ( < UNSIGNED > ) | LONG

 < DROP_SENTENCE > -> DROP < TABLE NAME >


< WHERE_CLAUSE > -> 
      WHERE < FIELD_NAME_TEXT > [ NOT ] LIKE < TEMPLATE_STRING > 
    | WHERE < EXPRESSION > [ NOT ] IN (< LIST_CONST >)
    | WHERE < LOGICAL_EXPRESSION >
    | WHERE ALL 
    
< TEMPLATE_STRING > -> < SQL_STR >

< EXPRESSION > -> < LONG_EXPRESSION > | < TEXT_EXPRESSION >
    
< LIST_CONST > -> < SQL_STR > {, < SQL_STR >} | < SQL_LONG > {, < SQL_LONG >}

< LONG_EXPRESSION > -> < LONG_AD > {< AD > < LONG_AD >}
    
< AD > -> + | -
    
< LONG_AD > -> < LONG_ML > { < ML > < LONG_ML >}
    
< ML > -> * | / | %
    
< LONG_ML > -> < LONG_VAL > | (< LONG_EXPRESSION >)

< LONG_VAL > -> < FIELD_NAME_LONG > | < SQL_LONG >

< TEXT_EXPRESSION > -> < FIELD_NAME_TEXT > | < SQL_STR >

< LOGICAL_EXPRESSION > -> < LOGICAL_ML > { AND < LOGICAL_ML >}

< LOGICAL_ML > -> NOT < LOGICAL_ML > | (< LOGICAL_EXPRESSION >) | (< RATIO >)
    
< RATIO > -> < TEXT_RATIO > | < LONG_RATIO >
    
< TEXT_RATIO > -> < TEXT_EXPRESSION > < RATIO_OP > < TEXT_EXPRESSION >

< LONG_RATIO > -> < LONG_EXPRESSION > < RATIO_OP > < LONG_EXPRESSION >

< RATIO_OP > -> = | > | >= | <= | !=

---

## For advanced scheme, at which the programm works, see the pictures and text file
