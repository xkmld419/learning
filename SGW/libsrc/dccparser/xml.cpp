// Common utility for XML/DOM handling

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string>
#include <iostream>
#ifndef MXML
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>
#endif
#include "xml.h"

DOMNode*
UtilXML::getNextElementNode(DOMNode *n)
{
	#ifndef MXML
  for (n=n->getNextSibling(); n != NULL; n=n->getNextSibling())
  {
    if (n->getNodeType() == DOMNode::ELEMENT_NODE)
    {
      return n;
    }
  }
  #else
  for (n=n->next; n != NULL; n=n->next)
  {
    if (n->type == MXML_ELEMENT && strncmp(n->value.element.name,"!--",3))
    {
      return n;
    }
  }
  #endif
  return n;
}

bool
UtilXML::matchNode(const DOMNode *n, const char *name)
{
  if (n == NULL)
    return false;
	
	#ifndef MXML
  std::string nodeName(transcodeFromXMLCh(n->getNodeName()));
  #else
  std::string nodeName(n->value.element.name);
  #endif
  return (nodeName == std::string(name));
}

std::string
UtilXML::getProp(const DOMNode *n, const char *name)
{
	#ifndef MXML
  XMLCh *tmp = XMLString::transcode(name); // XML transcoder allocates memory
  DOMElement *e = (DOMElement*)n;
  const XMLCh *c_data = e->getAttribute(tmp);
  XMLString::release(&tmp);
  if (c_data == NULL)
  {
    std::cout << e->getTagName() << "requires " << name 
		  << "attribute" << std::endl;
    exit(1);
  }
  
  return (transcodeFromXMLCh(c_data));
  #else
  std::string s="";
  int i = 0;
  for (;i<n->value.element.num_attrs;i++)
  {
  	if (!strcmp(n->value.element.attrs[i].name,name))
  	{
  		s = n->value.element.attrs[i].value;
  		break;
  	}
  }
  return s;
  #endif
}

std::string
UtilXML::getProp(const DOMNode *n, const char* name, char* const dflt)
{
  if (dflt == NULL)
  {
    AAA_LOG(LM_ERROR, "default string cannot be null\n");
    exit(1);
  }
  #ifndef MXML
  XMLCh *tmp = XMLString::transcode(name);  // XML transcoder allocates memory
  DOMElement *e = (DOMElement*)n;
  const XMLCh *c_data = e->getAttribute(tmp);
  XMLString::release(&tmp);
  std::string s1(dflt);
  if (c_data == NULL)
  {
    return s1;
  }
  std::string s2(transcodeFromXMLCh(c_data));
  if (s2.size() == 0) // #IMPLIED field contains null string
  {
    return s1;
  }
  return s2;
  #else
  std::string s(dflt);
  for (int i=0;i<n->value.element.num_attrs;i++)
  {
  	if (!strcmp(n->value.element.attrs[i].name,name))
  	{
  		s = n->value.element.attrs[i].value;
  		break;
  	} 
  }
  return s;
  #endif
}

std::string
UtilXML::transcodeFromXMLCh(const XMLCh *xmlCh) 
{
	#ifndef MXML
  char *c_str;
  c_str = XMLString::transcode(xmlCh); // XML transcoder allocates memory
  std::string s(c_str);
  XMLString::release(&c_str);
  #else
  std::string s(xmlCh);
  #endif
  return s;
}
