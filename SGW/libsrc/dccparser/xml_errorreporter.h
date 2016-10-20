#ifndef MXML
#include "diameter_parser_api.h"
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/sax/ErrorHandler.hpp>

class myDOMTreeErrorReporter  : public ErrorHandler
{
public:
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
/*
    myDOMTreeErrorReporter (int i) :
       fSawErrors(false)
    {

    }
*/
    myDOMTreeErrorReporter ()
    {
			fSawErrors = false;
    }




//    myDOMTreeErrorReporter ();

    ~myDOMTreeErrorReporter ()
    {
    }

    // -----------------------------------------------------------------------
    //  Implementation of the error handler interface
    // -----------------------------------------------------------------------
    void warning(const SAXParseException& toCatch);
    void error(const SAXParseException& toCatch);
    void fatalError(const SAXParseException& toCatch);
    void resetErrors();

    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    bool getSawErrors() const;

    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fSawErrors
    //      This is set if we get any errors, and is queryable via a getter
    //      method. Its used by the main code to suppress output if there are
    //      errors.
    // -----------------------------------------------------------------------
    bool    fSawErrors;
};

inline bool myDOMTreeErrorReporter ::getSawErrors() const
{
    return fSawErrors;
}

#endif
