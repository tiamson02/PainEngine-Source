/*
    File:   match.cpp
    Author: eric van beurden
    Date:   24/12/2005

    Namespaces:           "Text"
    System Dependencies:  <none>
    Configuration Macros: <none>
        
*/

#include "match.h"
#include "matchNode.h"
#include <string.h>
#include "trace.h"
#include "text.h"


//!
namespace Text{


const int32     Match::MATCH_META_KLEEN =       '*';
const int32     Match::MATCH_META_CHAR =        '?';
const int32     Match::MATCH_META_PLUS =        '+';
const int32     Match::MATCH_META_BLOCK_OPEN =  '[';
const int32     Match::MATCH_META_BLOCK_CLOSE = ']';
const char *    Match::MATCH_META_KLEEN_S =     "*";
const char *    Match::MATCH_META_CHAR_S =      "?";
const char *    Match::MATCH_META_PLUS_S =      "+";


Match::Match(){
    head = NULL;
}

Match::Match(const char *pattern){
    head = NULL;

    buildPattern(pattern);
}

Match::~Match(){
    reset();
}


bool Match::buildPattern(const char *p){
    bool        escape =    false;
    bool        inBlock =   false;
    int32       n;
    int32       pj =        0;
    size_t      len =       strlen(p);
    char *      buf =       strcopy(NULL, p);
    bool *      mask =      new bool[len];
    char *      pattern =   strcopy(NULL, "", (signed)len + 1);
    char *      start =     NULL;
    char *      block =     NULL;


    //delete the previous pattern if set
    if (head)
        delete head;

    head = NULL;


    //initialize the mask
    memset(mask, 0, len * sizeof(bool));


    //clean up the pattern string: Mark metacharacters that were in the literal as literals, 
    //not metacharacters; collapse neighbouring duplicate metacharacters to a single metacharacter;
    //copy escaped metacharacters.
    for (n = 0; buf[n]; n++){

        //check for escape sequences and copy the following character as a literal.
        if (buf[n] == '\\'){
            mask[pj] = true;
            
            pattern[pj++] = strtransesc(&buf[n]);
            n++;

            continue;
        }


        pattern[pj++] = buf[n];


        //filter out equivalent metapatterns.
        if (!escape){
            if (buf[n] == MATCH_META_BLOCK_OPEN){
                inBlock = true;

                continue;
            }
            else if (buf[n] == MATCH_META_BLOCK_CLOSE){
                inBlock = false;

                //if the character immediately following the block is a kleen star or a plus, it is part
                //of the block, not a normal metacharacter => copy it to the pattern, skip over the character,
                //and don't compress equivalent patterns.
                if (buf[n + 1] == MATCH_META_KLEEN || buf[n + 1] == MATCH_META_PLUS){
                    n++;
                    pattern[pj++] = buf[n];    
                }

                continue;
            }

            if (inBlock)
                continue;


            //"**" is equivalent to "*"
            if (buf[n] == MATCH_META_KLEEN && buf[n + 1] == MATCH_META_KLEEN)
                pj--;

            //"+*" is equivalent to "+"
            //"*+" is equivalent to "+"
            if (buf[n] == MATCH_META_PLUS &&  buf[n + 1] == MATCH_META_KLEEN ||
                buf[n] == MATCH_META_KLEEN && buf[n + 1] == MATCH_META_PLUS 
               )
                n++;

            //"*?" is equivalent to "+"
            //"?*" is equivalent to "+"
            if (buf[n] == MATCH_META_KLEEN && buf[n + 1] == MATCH_META_CHAR ||
                buf[n] == MATCH_META_PLUS &&  buf[n + 1] == MATCH_META_KLEEN
               ){
                pattern[pj - 1] = MATCH_META_PLUS;
                n++;
                //buf[n + 1] = MatchNode::MATCH_META_KLEEN;
            } 
        }

        escape = false;
    }


    //terminate pattern string
    pattern[pj] = 0;  
    inBlock = false;


    //mark non-metacharacters off in the pattern string
    for (n = 0; pattern[n]; n++){
        if (mask[n]) 
            continue;

        if (pattern[n] == MATCH_META_BLOCK_OPEN){
            inBlock = true;

            continue;
        }

        else if (pattern[n] == MATCH_META_BLOCK_CLOSE){
            inBlock = false;
            
            continue;
        }

        // \todo check if this character is inside a block too
        if (!inBlock && !(  pattern[n] == MATCH_META_KLEEN || 
                            pattern[n] == MATCH_META_CHAR ||
                            pattern[n] == MATCH_META_PLUS
                         )
           )
            mask[n] = true;
    }


    //build the pattern chain from the pattern string
    for (n = 0; pattern[n]; n++){
        if (!mask[n]){

            if (block == NULL){

                //found a kleen star => add to the pattern chain
                if (pattern[n] == MATCH_META_KLEEN){

                    //part of a string literal has been found
                    if (start){
                        pattern[n] = 0;


                        //create a new match node to represent this literal
                        if (head)
                            *head += new MatchNode(start);

                        else
                            head = new MatchNode(start);


                        //repair the pattern string
                        pattern[n] =    MATCH_META_KLEEN;
                        start =         NULL;
                    }

                    //add the kleen star as a new match node as well
                    if (head)
                        *head += new MatchNode(MatchNode::MATCH_KLEEN, MATCH_META_KLEEN_S);

                    else
                        head = new MatchNode(MatchNode::MATCH_KLEEN, MATCH_META_KLEEN_S);
                }

                //found a plus matcher => add to the pattern chain
                else if (pattern[n] == MATCH_META_PLUS){

                    //part of a string literal has been found
                    if (start){
                        pattern[n] = 0;


                        //create a new match node to represent this literal
                        if (head)
                            *head += new MatchNode(start);

                        else
                            head = new MatchNode(start);


                        //repair the pattern string
                        pattern[n] =    MATCH_META_PLUS;
                        start =         NULL;
                    }

                    //add the kleen star as a new match node as well
                    if (head)
                        *head += new MatchNode(MatchNode::MATCH_PLUS, MATCH_META_PLUS_S);

                    else
                        head = new MatchNode(MatchNode::MATCH_PLUS, MATCH_META_PLUS_S);
                }

                //found a single character matcher metacharacter => add to the pattern chain
                else if (pattern[n] == MATCH_META_CHAR){
                    int32 k;

                    //part of a string literal has been found
                    if (start){
                        pattern[n] = 0;


                        //create a new match node to represent this literal
                        if (head)
                            *head += new MatchNode(start);

                        else
                            head = new MatchNode(start);


                        //repair the pattern string
                        pattern[n] =    MATCH_META_CHAR;
                        start =         NULL;
                    }


                    //count how many single character matcher metacharacters there are in a row
                    for (k = n; pattern[n] == MATCH_META_CHAR; n++);

                    n--;

                    //add the string of metachars as a new match node as well
                    if (head)
                        *head += new MatchNode(MatchNode::MATCH_CHAR, MATCH_META_CHAR_S, NULL, n - k + 1);

                    else
                        head = new MatchNode(MatchNode::MATCH_CHAR, MATCH_META_CHAR_S, NULL, n - k + 1);
                }

                //found a block start character => dump any existing literal and scan to the end of the block
                else if (pattern[n] == MATCH_META_BLOCK_OPEN){
                    block = &pattern[n];

                    //part of a string literal has been found
                    if (start){
                        pattern[n] = 0;


                        //create a new match node to represent this literal
                        if (head)
                            *head += new MatchNode(start);

                        else
                            head = new MatchNode(start);


                        //repair the pattern string
                        pattern[n] =    MATCH_META_BLOCK_OPEN;
                        start =         NULL;
                    }
                }
            }

            //found a block end character
            else if (pattern[n] == MATCH_META_BLOCK_CLOSE){

                //block is followed by a star => match zero or more of the characters in the block
                if (pattern[n + 1] == MATCH_META_KLEEN){
                    pattern[n + 1] = 0;

                    if (head)
                        *head += new MatchNode(MatchNode::MATCH_KLEEN, MATCH_META_KLEEN_S, block);

                    else 
                        head = new MatchNode(MatchNode::MATCH_KLEEN, MATCH_META_KLEEN_S, block);

                    pattern[n + 1] = MATCH_META_KLEEN;
                    n++;
                }

                //block is followed by a plus => match one or more of the characters in the block
                else if (pattern[n + 1] == MATCH_META_PLUS){
                    pattern[n + 1] = 0;

                    if (head)
                        *head += new MatchNode(MatchNode::MATCH_PLUS, MATCH_META_PLUS_S, block);

                    else 
                        head = new MatchNode(MatchNode::MATCH_PLUS, MATCH_META_PLUS_S, block);

                    pattern[n + 1] = MATCH_META_PLUS;
                    n++;
                }

                //block is followed by a question mark => match one of the characters in the block
                else if (pattern[n + 1] == MATCH_META_CHAR){
                    pattern[n + 1] = 0;

                    if (head)
                        *head += new MatchNode(MatchNode::MATCH_CHAR, MATCH_META_CHAR_S, block);

                    else 
                        head = new MatchNode(MatchNode::MATCH_CHAR, MATCH_META_CHAR_S, block);

                    pattern[n + 1] = MATCH_META_CHAR;
                    n++;
                }

                //block is followed by some other character (possibly a '?') => treat it as a single character matcher
                else{
                    char brake = pattern[n + 1];

                    pattern[n + 1] = 0;

                    if (head)
                        *head += new MatchNode(MatchNode::MATCH_CHAR, MATCH_META_CHAR_S, block, 1);

                    else 
                        head = new MatchNode(MatchNode::MATCH_CHAR, MATCH_META_CHAR_S, block, 1);

                    pattern[n + 1] = brake;
                }

                block = NULL;
            }
        }

        //mark the start of a new string literal in the pattern
        else if (start == NULL)
            start = &pattern[n];
    }


    //one last string literal remains in the pattern => add it to the chain as a final match node
    if (start){
        if (head)
            *head += new MatchNode(start);

        else
            head = new MatchNode(start);

        start = NULL;
    }


    //clean up temporary memory
    delete []mask;
    strcopy(&buf, NULL);
    strcopy(&pattern, NULL);

    return true;
}

bool Match::setPattern(const char *pattern){
    return buildPattern(pattern);
}

void Match::reset(){
    if (head)
        delete head;

    head = NULL;
}

bool Match::match(const char *str, bool caseSensitive, bool wholeWord) const{

    //no match chain was created => fail
    if (head == NULL)
        return false;


    //attempt to match the string to the pattern chain
    switch (head->match(str, caseSensitive, wholeWord)){
        case MatchNode::MATCH_MATCH:
            return true;

        case MatchNode::MATCH_FAIL:
            return false;

        case MatchNode::MATCH_CONTINUE:
            TRACE("[File]::match(): ERROR-> MATCH_CONTINUE returned from pattern matcher\n");
            return false;
    }


    return false;
}

int32 Match::matchStart(const char *str, bool caseSensitive, bool wholeWord) const{
    const char *failPoint;


    //no match chain was created => fail
    if (head == NULL)
        return -1;


    //attempt to match the string to the pattern chain
    switch (head->match(str, caseSensitive, wholeWord, &failPoint)){
        case MatchNode::MATCH_MATCH:
            return (int32)(failPoint - str);

        case MatchNode::MATCH_FAIL:
            return -((int32)(failPoint - str)) - 1;

        case MatchNode::MATCH_CONTINUE:
            TRACE("[File]::matchStart(): ERROR-> MATCH_CONTINUE returned from pattern matcher\n");
            return -1;
    }


    return -1; 
}


bool Match::isMetaChar(int32 c){
    if (c == MATCH_META_KLEEN ||
        c == MATCH_META_CHAR ||
        c == MATCH_META_PLUS ||
        c == MATCH_META_BLOCK_OPEN ||
        c == MATCH_META_BLOCK_CLOSE)

        return true;

    return false;
}

const char *Match::findFirstMetaChar(const char *str){
    
    while (str[0]){

        //the next two characters are part of an escape sequence, so skip them
        if (str[0] == '\\'){
            str += 2;

            continue;
        }

        //stop on the first metacharacter in the string
        if (isMetaChar(str[0]))
            return str;

        str++;
    }

    return NULL;
}

const char *Match::findPathComponent(const char *pattern){
    const char *path = pattern;

    while (pattern[0]){

        //the next two characters are part of an escape sequence, so skip them
        if (pattern[0] == '\\'){
            pattern++;

            //the next character is a DOS path seperator
            if (pattern[0] == '\\')
                path = pattern - 1;

            pattern++;

            continue;
        }


        //check if the next character is a path seperator
        if (pattern[0] == '/')
            path = pattern;


        //stop on the first metacharacter in the string
        if (isMetaChar(pattern[0]))
            return path;

        pattern++;
    }

    return path;
}

};  //namespace Text;