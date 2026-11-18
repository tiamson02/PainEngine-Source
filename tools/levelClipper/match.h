/*
    File:   match.h
    Author: eric van beurden
    Date:   24/12/2005

    Namespaces:           "Text"
    System Dependencies:  <none>
    Configuration Macros: <none>
        
*/

#ifndef  __match_H_
# define __match_H_

# include "stdTypes.h"

//!
namespace Text{


/*! \addtogroup stringlib
    @{
*/
/*! \brief Regular expression matcher class
    \ingroup classes

    Provides regular expression pattern matching functionality.  The test pattern may be specified 
    in the constructor or through the setPattern() member function.  Once a pattern is set, the 
    object may be used to test multiple strings to see if they match that pattern.  Pattern matching 
    is only returned as true or false.  If an error occurs in processing the pattern string there 
    will be no error report when matching, just a failure.  The following regex metacharacters are
    supported:
        - '?': matches any single character
        - '*': matches any 0 or more characters
        - '+': matches any one or more characters
        - '[]': specifies a mask of characters that are allowed for the next character or set
            of characters.  This block may optionally be followed by one of the above metacharacters
            to specify repitition of the block.  Inside the block characters may be specified 
            singly or may be given as ranges using the '-' character.  For example, '[a-z]' or '[a-z]?'
            will match a single lower case letter, '[a-z]*' will match zero or more lower case letters, 
            and '[0-9]+' will match one or more decimal digits.
        .
    Strings can be compared to the pattern in either a case-sensitive or case-insensitive way.  Note
    that if case is being disregarded when matching, the character block '[a-z]' will be equivalent
    to the block '[a-zA-Z]'.  Comparing in a case-insensitive way will not affect the pattern itself
    just the way it is interpreted.  Multiple strings may be compared to the same pattern in both a 
    case-sensitive and a case-insensitive way without any modification.

*/
class Match{
    static const int32  MATCH_META_KLEEN;
    static const int32  MATCH_META_CHAR;
    static const int32  MATCH_META_PLUS;
    static const int32  MATCH_META_BLOCK_OPEN;
    static const int32  MATCH_META_BLOCK_CLOSE;
    static const char * MATCH_META_KLEEN_S;
    static const char * MATCH_META_CHAR_S;
    static const char * MATCH_META_PLUS_S;


private:
    class MatchNode;

    //start of the pattern chain
    MatchNode *head;

    //buildPattern(): builds a pattern chain from the string <pattern>.  Returns
    //  true.
    bool buildPattern(const char *pattern);

public:

    /*! \brief Default constructor: creates a matcher with no set pattern.
        \param - No parameters.
        \return No return value.
        \remarks This constructor creates an empty pattern matcher.  Any calls to 
            match() will immediately fail.  A pattern must be set using the 
            setPattern() function before this matcher will return a valid result.
        \sa setPattern() | match()
    */
    Match();

    /*! \brief constructor
        \param pattern null terminated string containing the pattern that this is 
            to match.
        \return No return value.
        \remarks This constructor will create a valid pattern matcher.  The pattern
            chain will be set and match() may be called immediately.
        \sa setPattern() | match()
    */
    Match(const char *pattern);

    /*! \brief destructor
        \param - No parameters.
        \return No return value.
        \remarks Destroys this matcher.  The pattern chain will be destroyed
        \sa Match() | setPattern()
    */
    ~Match();


    /*! \name Matching Functions
        @{
    */
    /*! \brief creates a new pattern chain
        \param pattern null terminated string containing the pattern string to build
        \return Returns true.
        \remarks This function builds a pattern chain that is used to attempt to match
            other strings.  This function must be called to change the pattern or if
            no pattern is specified upon construction.
        \sa match()
    */
    bool setPattern(const char *pattern);

    /*! \brief clears the current pattern chain.
        \param - No parameters.
        \return No return value.
        \remarks Destroys the current pattern.  This matcher will always fail until
            a new pattern is specified using setPattern().
        \sa setPattern() | match()
    */
    void reset();

    /*! \brief tests a string against the current pattern
        \param str null terminated string containing the text to be matched against
            the current pattern.
        \param caseSensitive true if the pattern is to be matched in a case sensitive
            manner, and false otherwise.  Case insensitive checking (false) is useful
            for comparing filenames.
        \param wholeWord true if the pattern must match to the entire test string \a str.
            False if the test string \a str may contain other characters after the 
            pattern is successfully matched.
        \return Returns true if the string matches the pattern.
        \return Returns false if the string does not match the pattern.
        \remarks This function is used to test a string against the current pattern
            chain.
        \sa setPattern() | matchStart()
    */
    bool match(const char *str, bool caseSensitive = true, bool wholeWord = true) const;

    /*! \brief tests a string to see if its prefix matches the current pattern
        \param str null terminated string containing the text to be matched against
            the current pattern.
        \param caseSensitive true if the pattern is to be matched in a case sensitive
            manner, and false otherwise.  Case insensitive checking (false) is useful
            for comparing filenames.
        \param wholeWord true if the pattern must match to the entire test string \a str.
            False if the test string \a str may contain other characters after the 
            pattern is successfully matched.
        \return Returns an integer greater than or equal to 0 if the current pattern
            matches the beginning of the string.  The start of the string must match
            the entire pattern, but may have extra characters after it to be considered
            a successful match.  The return value will indicate the character offset
            into the original string \a str where the pattern was matched up to.
        \return Returns a negative integer if the current pattern does not match the
            prefix of the string \a str.  -1 plus the absolute value of the return 
            value will indicate the index of the first character in the string \a
            str that did not match the current pattern.
        \remarks This function is used to test a string against the current pattern
            to see if at least the beginning portion of the string matches the pattern.
            Extra characters may exist in the test string \a str after the pattern 
            is matched (ie: it need not match the entire string to succeed).
        \sa match() | setPattern()
    */
    int32 matchStart(const char *str, bool caseSensitive = true, bool wholeWord = false) const;
    //! @}


    /*! \name String Testing Functions
        @{
    */
    /*! \brief tests if a character is a metacharacter recognized by this class
        \param c character to test
        \return Returns true if \a c is a metacharacter that will be processed differently
        \return Returns false if \a c is not a metacharacter.
        \remarks This function tests to see if a single character will be recognized
            as a metacharacter.  It will not be able to handle escaped characters however
            (ie: "\[").
        \sa findFirstMetaChar() | findPathComponent()
    */
    static bool isMetaChar(int32 c);

    /*! \brief searches a string for the first occurrence of a metacharacter
        \param pattern null terminated string that will be searched for a metacharacter.
        \return Returns a pointer to the first metacharacter in the string if found
        \return Returns NULL if no metacharacter is present.
        \remarks none.
        \sa isMetaChar()
    */
    static const char *findFirstMetaChar(const char *pattern);

    /*! \brief searches a string for a non-wildcard path component 
        \param pattern null terminated string containing the string that will be searched
            for a non-wildcard path component.  
        \return Returns a pointer to the first character beyond the last non-wildcard 
            path component of a regex pattern.  The first character in the returned string
            will always be a path seperator if a path component was found.
        \return Returns \a pattern if no path components are found or the pattern is 
            not a filename.
        \remarks none.
        \sa findFirstMetaChar() | isMetaChar()
    */
    static const char *findPathComponent(const char *pattern);
    //! @}
};
//! @}

};  //namespace Text;


#endif
