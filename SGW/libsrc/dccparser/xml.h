#ifndef __UTL_XML_H__
#define __UTL_XML_H__

#include <string>
#ifndef MXML
#include <xercesc/dom/DOM.hpp>
#else
#include <mxml.h>
  #ifndef DOMNode
    typedef mxml_node_t DOMNode;
  #endif
  #ifndef XMLCh
    typedef char XMLCh;
  #endif
#endif
#include "diameter_parser_api.h"

#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace xercesc;
#endif

class DIAMETERPARSER_EXPORT UtilXML
{
  public:
    static std::string getProp(const DOMNode *n, const char* name, char* const dflt);
    static std::string getProp(const DOMNode *n, const char* name);
    static bool matchNode(const DOMNode *n, const char* name);
    static DOMNode* getNextElementNode(DOMNode *n);
    static std::string transcodeFromXMLCh(const XMLCh*);
};

#endif // __UTL_XML_H__
