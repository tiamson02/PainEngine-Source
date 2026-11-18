/*
    File:   match.h
    Author: eric van beurden
    Date:   21/12/2005

    Namespaces:           "Text"
    System Dependencies:  <none>
    Configuration Macros: <none>
        
*/

#ifndef  __matchNode_H_
# define __matchNode_H_

# include "stdTypes.h"
# include "match.h"


namespace Text{


//class MatchNode: provides storage and matching functionality for a single node
//  of the match pattern.
class Match::MatchNode{
    friend class Match;

public:

    /*! \brief string matching result names
        These names will be retured by the match() function to indicate the 
        result of the matching operation.  MATCH_CONTINUE is reserved for 
        a state in which the matching pattern is invalid or an error occurs.
    */
    typedef enum{
        MATCH_FAIL,         /*!< the string does not match the pattern */
        MATCH_CONTINUE,     /*!< an error occurred during the match */
        MATCH_MATCH,        /*!< the string matched the pattern */
    } Result;


private:
    typedef enum{
        MATCH_NONE,
        MATCH_KLEEN,
        MATCH_CHAR,
        MATCH_PLUS,
        MATCH_LITERAL,
    } NodeType;


    //type of match component for this node
    NodeType    type;

    //actual pattern string used to match for this node
    char *      pattern;

    //length of this node's string (used for literals)
    size_t      len;

    //number of characters to match for a ? node
    int32       count;

    //true if a kleen star exists before this node
    bool        kleenExists;

    //true if this is the first node in the list
    bool        headNode;

    //true if a masked set exists for this node.  The mask will still be valid 
    //even if this is false and the isAllowed() function will return the appropriate
    //result, but this can be used to skip processing a portion of the string when 
    //it is known that all characters will pass.
    bool        maskSet;

    //family of characters that match for this node (defaults to all characters)
    uint32      mask[8];

    //next node in the matching chain
    MatchNode * next;


    //setMask(): parses a character mask and creates a mask set for this node
    void setMask(const char *charMask);

    //setMask(): sets the mask for a single character.
    void setMask(int32 c);

    //isAllowed(): tests the character mask for the character <c>.  Returns true
    //  if the character is allowed in the cvurrent mask, and false otherwise.
    bool isAllowed(int32 c, bool caseSensitive = true) const;

public:
    MatchNode();
    MatchNode(const char *str);
    MatchNode(NodeType nodeType, const char *str, const char *charMask = NULL, int32 matchCount = 1);
    ~MatchNode();

    MatchNode &operator += (MatchNode *node);

    Result match(const char *str, bool isCaseSensitive = true, bool wholeWord = true, const char **failPoint = NULL) const;
};


};  //namespace Text;


#endif
