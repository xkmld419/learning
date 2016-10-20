// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#ifndef MXML
#include <xercesc/sax/SAXParseException.hpp>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "xml.h"
#include "xml_errorreporter.h"

//
// Utility class for string conversion
//
class DOMStrX {
    public:
       DOMStrX(const XMLCh * const toTranscode) { localRef_ = XMLString::transcode(toTranscode); }

       ~DOMStrX() { XMLString::release(&this->localRef_); }

       char *localRef() { return this->localRef_; }

    private:
       char *localRef_;
};

// Global streaming operator for DOMString is defined in DOMPrint.cpp
//extern ostream& operator<<(ostream& target, const DOMString& s);

void myDOMTreeErrorReporter ::warning(const SAXParseException& toCatch)
{
    DOMStrX strx(toCatch.getMessage());

    //
    // Ignore all warnings.
    //
    std::cout << "Warning at file \"" 
	 << UtilXML::transcodeFromXMLCh((XMLCh*)toCatch.getSystemId())
	 << std::flush
	 << "\", line " << toCatch.getLineNumber()
	 << ", column " << toCatch.getColumnNumber()
         << "\n   [" << strx.localRef() << "]"
	 << std::endl;
}

void myDOMTreeErrorReporter ::error(const SAXParseException& toCatch)
{
    DOMStrX strx(toCatch.getMessage());
    fSawErrors = true;
/*
    cerr << "Error at file \"" << DOMString(toCatch.getSystemId())
		 << "\", line " << toCatch.getLineNumber()
		 << ", column " << toCatch.getColumnNumber()
         << "\n   Message: " << DOMString(toCatch.getMessage()) << endl;
*/
    std::cout << "Error at file \"" 
	 << UtilXML::transcodeFromXMLCh((XMLCh*)toCatch.getSystemId())
	 << std::flush
	 << "\", line " << toCatch.getLineNumber()
	 << ", column " << toCatch.getColumnNumber()
         << "\n   [" << strx.localRef() << "]"
	 << std::endl;
}

void myDOMTreeErrorReporter ::fatalError(const SAXParseException& toCatch)
{
    DOMStrX strx(toCatch.getMessage());
    fSawErrors = true;
/*
    cerr << "Fatal Error at file \"" << DOMString(toCatch.getSystemId())
		 << "\", line " << toCatch.getLineNumber()
		 << ", column " << toCatch.getColumnNumber()
         << "\n   Message: " << DOMString(toCatch.getMessage()) << endl;
*/
    std::cout << "Fatal Error at file \"" 
	 << UtilXML::transcodeFromXMLCh((XMLCh*)toCatch.getSystemId()) 
	 << std::flush
	 << "\", line " << toCatch.getLineNumber()
	 << ", column " << toCatch.getColumnNumber()
         << "\n   [" << strx.localRef() << "]"
	 << std::endl;
}

void myDOMTreeErrorReporter ::resetErrors()
{
    // No-op in this case
}
#endif

