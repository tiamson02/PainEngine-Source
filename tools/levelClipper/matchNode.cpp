/*
    File:   match.cpp
    Author: eric van beurden
    Date:   21/12/2005

    Namespaces:           "Text"
    System Dependencies:  <none>
    Configuration Macros: <none>
        
*/

#include <stdlib.h>
#include <string.h>
#include "match.h"
#include "matchNode.h"
#include "text.h"


//!
namespace Text{

//********************************* Utility Functions *************************************//
int toLower(int c){
    if (c >= 'A' && c <= 'Z')
        return c + ('A' - 'a');

    return c;
}

int toUpper(int c){
    if (c >= 'a' && c <= 'z')
        return c + ('a' - 'A');

    return c;
}

//******************************************* Class Implementation ***************************************//
Match::MatchNode::MatchNode(){
    pattern =       NULL;
    next =          NULL;
    count =         0;
    kleenExists =   false;
    len =           0;
    headNode =      true;

    //all characters are valid
    setMask(NULL);
}


//MatchNode(): constructor: this will create a node that represents a string literal.
//  It will match only to the exact string <str>.
Match::MatchNode::MatchNode(const char *str){

    //this node will always represent a string literal
    pattern =       strcopy(NULL, str);
    next =          NULL;
    count =         1;
    kleenExists =   false;
    len =           strlen(pattern);
    headNode =      true;
    type =          MATCH_LITERAL;

    //all characters are valid
    setMask(NULL);
}

//MatchNode(): constructor: this will create a node that represents a single metacharacter or group
//  of 'single character matcher' metacharacters.  It will match to any set of characters that match
//  the metacharacter's particular requirements (including its mask)
Match::MatchNode::MatchNode(NodeType nodeType, const char *str, const char *charMask, int32 matchCount){
    pattern =       strcopy(NULL, str);
    next =          NULL;
    count =         matchCount;
    kleenExists =   false;
    len =           strlen(pattern);
    headNode =      true;
    type =          nodeType;

    setMask(charMask);
}

Match::MatchNode::~MatchNode(){
    if (pattern)
        delete []pattern;

    if (next)
        delete next;
}


void Match::MatchNode::setMask(int32 c){
    mask[c >> 5] |= (1 << (c & 0x1f));
}

bool Match::MatchNode::isAllowed(int32 c, bool caseSensitive) const{
    if (!caseSensitive){
        int32 l = toLower(c);

        //check the mask for the upper and lower case version of the character
        if (INRANGE(l, 'a', 'z')){
            int32 u = toUpper(c);

            return ((mask[l >> 5] & (1 << (l & 0x1f))) != 0) ||
                   ((mask[u >> 5] & (1 << (u & 0x1f))) != 0);
        }
    }


    return (mask[c >> 5] & (1 << (c & 0x1f))) != 0;
}

void Match::MatchNode::setMask(const char *charMask){
    const char *tok = charMask;

    //no mask to process => assume all characters count
    if (charMask == NULL){
        memset(mask, 0xff, 8 * sizeof(uint32));
        maskSet = false;

        return;
    }


    //skip over the opening bracket
    if (tok[0] == MATCH_META_BLOCK_OPEN)
        tok++;

    memset(mask, 0, 8 * sizeof(uint32));


    //parse the string and mask off all characters that are named in the [] block.
    //if an escape character is found, copy the next character instead of processing 
    //it as a special character.
    while (tok[0] && tok[0] != MATCH_META_BLOCK_CLOSE){

        //escape character => treat the next character as a literal value instead
        //of a special character.
        if (tok[0] == '\\'){
            

            //copy common escape sequences over as their character, otherwise
            //just copy the actual character.
            setMask(strtransesc(tok));
            tok++;
        }

        else{
            setMask(tok[0]);

            //the this character is followed by a range definition (ie: 'a-z')
            if (tok[1] == '-'){
                for (int32 i = tok[0] + 1; i <= tok[2]; i++)
                    setMask(i);

                tok += 2;
            }
        }

        //increment to the next character in the block string
        tok++;
    }

    maskSet = true;
}

Match::MatchNode &Match::MatchNode::operator += (MatchNode *node){
    bool        kleen = false;
    MatchNode * curr = this;

    
    while (curr->next){
        if (curr->type == MATCH_KLEEN || curr->type == MATCH_PLUS)
            kleen = true;

        curr = curr->next;

        curr->kleenExists = kleen;
    }

    curr->next =        node;
    node->kleenExists = kleen || (curr->type == MATCH_KLEEN || curr->type == MATCH_PLUS);
    node->headNode =    false;

    return *node;
}

Match::MatchNode::Result Match::MatchNode::match(const char *str, bool caseSensitive, bool wholeWord, const char **failPoint) const{
    int32 i = 0;


    //end of the element chain [recursion base case]
    if (next == NULL){

        switch (type){

            //zero or more characters match
            case MATCH_KLEEN:

                //only scan the rest of the string if a set of masked characters has been specified
                if (maskSet){

                    //check the remainder of the string to see if each of its characters
                    //are allowed with the masked set.
                    for (i = 0; str[i]; i++){
                        if (!isAllowed(str[i], caseSensitive)){
                            if (kleenExists)
                                return MATCH_CONTINUE;

                            else{
                                if (failPoint)
                                    *failPoint = &str[i];
                                
                                return wholeWord ? MATCH_FAIL : MATCH_MATCH;
                            }
                        }
                    }
                }


                if (failPoint)
                    *failPoint = &str[i];

                return MATCH_MATCH;


            case MATCH_PLUS:

                //at least one character must exist in the string
                if (str[0] == 0){
                    if (failPoint)
                        *failPoint = str;

                    return MATCH_FAIL;
                }


                //only scan the rest of the string if a set of masked characters has been specified
                if (maskSet){

                    //check the remainder of the string to see if each of its characters
                    //are allowed with the masked set.
                    for (i = 0; str[i]; i++){
                        if (!isAllowed(str[i], caseSensitive)){
                            if (kleenExists)
                                return MATCH_CONTINUE;

                            else{
                                if (failPoint)
                                    *failPoint = &str[i];

                                return (!wholeWord && i > 0) ? MATCH_MATCH : MATCH_FAIL;
                            }
                        }
                    }
                }


                if (failPoint)
                    *failPoint = &str[i];

                return MATCH_MATCH;


            case MATCH_CHAR:

                if (maskSet){

                    //make sure the first character is part of the masked set.  Only the first
                    //character needs to be checked because when a masked set is specified, the
                    //value of <count> is always 1.  When <count> is not 1 all characters will
                    //be allowed.  If this character is not part of the set fail iff there is
                    //not a kleen star or plus in the pattern chain before this node.
                    if (!isAllowed(str[0], caseSensitive)){
                        if (kleenExists)
                            return MATCH_CONTINUE;

                        else{
                            if (failPoint)
                                *failPoint = str;

                            return MATCH_FAIL;
                        }
                    }
                }


                for (i = 0; i < count; i++){
                    if (str[i] == 0){
                        if (failPoint)
                            *failPoint = &str[i];

                        return MATCH_FAIL;
                    }
                }


    
                //if the last character ends on the last of the metacharacters for this group
                //return a successful match
                if (str[count] == 0){
                    if (failPoint)
                        *failPoint = &str[count];

                    return MATCH_MATCH;
                }

                
                //otherwise the match failed here... however, any of the previous nodes contained
                //a kleen star or plus then another matching combination could possible exist, so
                //allow the search to continue.
                if (kleenExists)
                    return MATCH_CONTINUE;

                else{
                    if (failPoint)
                        *failPoint = &str[count];

                    return wholeWord ? MATCH_FAIL : MATCH_MATCH;
                }


            case MATCH_LITERAL:

                //if the literal matches up to the end of the string, the whole string matches
                if (wholeWord && 
                    (!caseSensitive && !strcasecmp(pattern, str) || 
                      caseSensitive && !strcmp(pattern, str)) ||
                    !wholeWord &&
                    (!caseSensitive && !strncasecmp(pattern, str, len) || 
                      caseSensitive && !strncmp(pattern, str, len))
                   ){

                    if (failPoint)
                        *failPoint = &str[len];

                    return MATCH_MATCH;
                }

                else{

                    //if this pattern is longer than the string it can't possibly match.  Or if this is the 
                    //head node of the pattern (consists of only a literal), it can't possibly match either.
                    if (headNode || len > strlen(str)){
                        if (failPoint){
                            *failPoint = caseSensitive ? &str[strbeg(pattern, str)]
                                                       : &str[strcasebeg(pattern, str)];
                        }

                        return MATCH_FAIL;
                    }

                    //if the pattern is shorter than the string, it could still match iff a kleen metacharacter
                    //exists in a node before this one.  If no kleen star exists before this node, no re-testing
                    //will be done on this portion of the string therefore it cannot possibly match.
                    else{
                        if (kleenExists)
                            return MATCH_CONTINUE;

                        else{
                            if (failPoint){
                                *failPoint = caseSensitive ? &str[strbeg(pattern, str)]
                                                           : &str[strcasebeg(pattern, str)];
                            }

                            if (wholeWord)
                                return MATCH_FAIL;

                            else{
                                if (caseSensitive)
                                    return !strncmp(pattern, str, len) ? MATCH_MATCH : MATCH_FAIL;

                                else
                                    return !strncasecmp(pattern, str, len) ? MATCH_MATCH : MATCH_FAIL;
                            }
                        }
                    }
                }
        }
    }

    //non-end node of the chain => match using result from previous matches
    else{

        //no string left to match, so fail the match operation
        if (str[0] == 0){
            if (failPoint)
                *failPoint = str;

            return MATCH_FAIL;
        }


        //check based on the type of this element and the next one
        switch (type){
            case MATCH_KLEEN:

                //search the rest of the string attempting a match starting at each character
                for (i = 0; str[i]; i++){
                    switch (next->match(&str[i], caseSensitive, wholeWord, failPoint)){
                        case MATCH_CONTINUE:
                            if (maskSet){

                                //fail this segment of the pattern is the character is 
                                //not allowed in the masked set
                                if (!isAllowed(str[i], caseSensitive)){
                                    if (kleenExists)
                                        return MATCH_CONTINUE;

                                    else{
                                        if (failPoint)
                                            *failPoint = &str[i];

                                        return MATCH_FAIL;
                                    }
                                }
                            }

                            continue;

                        case MATCH_FAIL:
                            return MATCH_FAIL;

                        case MATCH_MATCH:
                            return MATCH_MATCH;
                    }
                }


                //at this point, nothing in the remainder of the string was able to be matched
                //to the rest of the pattern => the fail point is the first character in the
                //string.
                if (failPoint)
                    *failPoint = str;

                return MATCH_FAIL;


            case MATCH_PLUS:

                //make sure at least one character exists
                /*if (str[0] == 0)              //<-- redundant check, but here for completeness of the case
                    return MATCH_FAIL;//*/


                if (maskSet){
                 
                    //make sure at least one character in the masked set exists 
                    if (!isAllowed(str[0], caseSensitive)){
                        if (kleenExists)
                            return MATCH_CONTINUE;

                        else{
                            if (failPoint)
                                *failPoint = str;

                            return MATCH_FAIL;
                        }
                    }
                }

                //search the rest of the string attempting a match starting at each character
                for (i = 1; str[i]; i++){

                    //try matching the remainder of the string with
                    //the remainder of the pattern.  
                    switch (next->match(&str[i], caseSensitive, wholeWord, failPoint)){
                        case MATCH_CONTINUE:
                            if (maskSet){
                     
                                //fail this segment of the pattern is the character is 
                                //not allowed in the masked set
                                if (!isAllowed(str[i], caseSensitive)){
                                    if (kleenExists)
                                        return MATCH_CONTINUE;

                                    else{
                                        if (failPoint)
                                            *failPoint = &str[i];

                                        return MATCH_FAIL;
                                    }
                                }
                            }

                            continue;

                        case MATCH_FAIL:
                            return MATCH_FAIL;

                        case MATCH_MATCH:
                            return MATCH_MATCH;
                    }
                }



                if (failPoint)
                    *failPoint = &str[maskSet ? 1 : 0];

                return MATCH_FAIL;


            case MATCH_CHAR:

                if (maskSet){
                    //make sure the first character is part of the masked set.  Only the first
                    //character needs to be checked because when a masked set is specified, the
                    //value of <count> is always 1.  When <count> is not 1 all characters will
                    //be allowed.  If this character is not part of the set fail iff there is
                    //not a kleen star or plus in the pattern chain before this node.
                    if (!isAllowed(str[0], caseSensitive)){
                        if (kleenExists)
                            return MATCH_CONTINUE;

                        else{
                            if (failPoint)
                                *failPoint = str;

                            return MATCH_FAIL;
                        }
                    }
                }


                //ensure enough characters remain in the string
                for (i = 0; i < count; i++){
                    if (str[count] == 0){
                        if (failPoint)
                            *failPoint = &str[count];

                        return MATCH_FAIL;
                    }
                }

                //this always succeeds as long as a character remains
                return next->match(&str[count], caseSensitive, wholeWord, failPoint);


            //if the string literal matches, recurse; otherwise, try to continue.
            case MATCH_LITERAL:

                //literal is longer than the string, so fail
                if (len > strlen(str)){
                    if (failPoint){
                        *failPoint = caseSensitive ? &str[strbeg(pattern, str)]
                                                   : &str[strcasebeg(pattern, str)];
                    }

                    return MATCH_FAIL;
                }

                if (caseSensitive){

                    //pattern doesn't match => Fail if this is the first node in the pattern
                    if (strncmp(pattern, str, len)){

                        if (headNode){
                            if (failPoint){
                                *failPoint = caseSensitive ? &str[strbeg(pattern, str)]
                                                           : &str[strcasebeg(pattern, str)];
                            }

                            return MATCH_FAIL;
                        }

                        else
                            return MATCH_CONTINUE;
                    }

                    else
                        return next->match(&str[len], caseSensitive, wholeWord, failPoint);
                }

                else{
                    if (strncasecmp(pattern, str, len)){
                        
                        if (headNode){
                            if (failPoint){
                                *failPoint = caseSensitive ? &str[strbeg(pattern, str)]
                                                           : &str[strcasebeg(pattern, str)];
                            }

                            return MATCH_FAIL;
                        }

                        else
                            return MATCH_CONTINUE;
                    }

                    else
                        return next->match(&str[len], caseSensitive, wholeWord, failPoint);
                }
        }
    }


    return MATCH_CONTINUE;
}


};  //namespace Text;
