#include <iostream>
#include <cstdlib>
#ifndef MXML
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/validators/DTD/DTDValidator.hpp>
#include <xercesc/framework/XMLValidator.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOMNode.hpp>
#include <xercesc/util/XMLString.hpp>
#include "xml_errorreporter.h"
#endif
#include <list>
#include <string>
#include <stdio.h>
#include <unistd.h>
#include "comlist.h"
#include "avplist.h"
#include "q_avplist.h"
#include "g_avplist.h"
#include "parser.h"
#include "diameter_parser_api.h"
#include "xml.h"
#include "xmldict.h"

AvpTypeList       *AvpTypeList::instance_ = 0;
AAACommandList    *AAACommandList::instance_ = 0;
AAAAvpList        *AAAAvpList::instance_ = 0;
AAAGroupedAvpList *AAAGroupedAvpList::instance_ = 0;
AvpEnumeratedInfo *AvpEnumeratedInfo::instance_ = 0;
AAAMemoryManager_S *AAAMemoryManager_S::instance_ = 0;
	
std::map<std::string, AAA_AVPCode> MyAvpCodeMap;

//AAALogMsg *AAALogMsg::instance_ = 0;
/// The application identifier of the currently parsing dictionary.
/// If it is currently parsing base protocol dictionary,
/// currentApplicationId is set to zero.
static AAAApplicationId currentApplicationId;

typedef struct {
  DOMNode *node;
  AAAGroupedAVP *gavp;
} AAAGroupedAVPNode;

void
parseVendor(DOMNode *n)
{
  std::string id(UtilXML::getProp(n, "id"));
  std::string name(UtilXML::getProp(n, "name"));
#ifdef DEBUG
  std::cout << "id = " << id;
  std::cout << ", name = " << name << std::endl;
#endif
}

void
parseOneAvpRule(DOMNode *n, AAAQualifiedAvpList* qavp_l,  
                int protocol)
{
  AAAQualifiedAVP *qavp;
  AAADictionaryEntry *avp;
  
  std::string name(UtilXML::getProp(n, "name"));
  std::string vendorStr(UtilXML::getProp(n, "vendor-id", "0"));
  std::string scenarioStr(UtilXML::getProp(n, "scenario-id", "0"));
  std::string minStr(UtilXML::getProp(n, "minimum", "0"));
  std::string maxStr(UtilXML::getProp(n, "maximum", "none"));
  ACE_UINT32 min = atoi(minStr.c_str()); 
  ACE_UINT32 max = (maxStr == std::string("none") ? 
		   QUAL_INFINITY : atoi(maxStr.c_str()));
  
#ifdef DEBUG
	 std::cout << "name = " << name;
	 std::cout << ", vendor-id = " << vendorStr << std::endl;
	 std::cout << ", scenario-id = " << scenarioStr << std::endl;
#endif
  int pos = name.find("_", 0 ) ;
  if( pos != std::string::npos) 
  {
#ifdef DEBUG_TEST
  	std::cout << "############parseOneAvpRule:orig_name:"<< name << ",update avpName:" <<  name.substr(0, pos) << std::endl;
#endif
  	name = name.substr(0, pos);
  }
  
  pos = name.find("ALIAS-", 0 ) ;
  if( pos != std::string::npos) 
  {
#ifdef DEBUG_TEST
  	std::cout << "############2.parseOneAvpRule:orig_name:"<< name << ",update avpName:" <<  name.substr(6, name.length()-6) << std::endl;
#endif
  	name = name.substr(6, name.length()-6);
  }

  qavp = new AAAQualifiedAVP;
  if ((avp = AAAAvpList::instance()->search(name.c_str(),
                                            protocol, atoi(scenarioStr.c_str()) )) == NULL)
  {
    AAA_LOG(LM_ERROR, "cannot find AVP named %s,scenario %s\n", name.c_str(), scenarioStr.c_str());
    throw;
  }
  
  qavp->avp = avp; 
  if( pos != std::string::npos) 
  {
#ifdef DEBUG_TEST
	  std::cout << "parseOneAvpRule:update avpName:" <<  name << ",scenario-id:"<< scenarioStr << ",protocol:" << protocol << std::endl;
#endif
  }

  qavp->qual.min = min; 
  qavp->qual.max = max; 
  qavp_l->add(qavp);
}

enum {
  AVP_RULE_TYPE_COMMAND=0,
  AVP_RULE_TYPE_GROUPED
} AVPRuleType;

void
parseAvpRule(DOMNode *n, AAAQualifiedAvpList* qavp_l, 
             int protocol)
{
  n = UtilXML::getNextElementNode(n);
  if (!UtilXML::matchNode(n, "avprule"))
  {
    AAA_LOG(LM_ERROR, "Grouped AVP requires one or more AVPs.\n" );
    throw;
  }
  do 
  {
    parseOneAvpRule(n, qavp_l, protocol);
    n = UtilXML::getNextElementNode(n);
  } while (UtilXML::matchNode(n, "avprule"));
}

void
parseCommandRule(DOMNode *n, AAACommand *com, int protocol)
{
  n = UtilXML::getNextElementNode(n);
  com->avp_f = new AAAQualifiedAvpList(PARSE_TYPE_FIXED_HEAD);
  com->avp_r = new AAAQualifiedAvpList(PARSE_TYPE_REQUIRED);
  com->avp_o = new AAAQualifiedAvpList(PARSE_TYPE_OPTIONAL);
  com->avp_f2 = new AAAQualifiedAvpList(PARSE_TYPE_FIXED_TAIL);
  if (UtilXML::matchNode(n, "fixed"))
  {
  	#ifndef MXML
    parseAvpRule(n->getFirstChild(), com->avp_f, protocol);
    #else
    parseAvpRule(n->child, com->avp_f, protocol);
    #endif
    n = UtilXML::getNextElementNode(n);
  }
  if (UtilXML::matchNode(n, "required"))
  {
  	#ifndef MXML
    parseAvpRule(n->getFirstChild(), com->avp_r, protocol);
    #else
    parseAvpRule(n->child, com->avp_r, protocol);
    #endif
    n = UtilXML::getNextElementNode(n);
  }
  if (UtilXML::matchNode(n, "optional"))
  {
  	#ifndef MXML
    parseAvpRule(n->getFirstChild(), com->avp_o, protocol);
    #else
    parseAvpRule(n->child, com->avp_o, protocol);
    #endif
    n = UtilXML::getNextElementNode(n);
  }
  if (UtilXML::matchNode(n, "fixed"))
  {
  	#ifndef MXML
    parseAvpRule(n->getFirstChild(), com->avp_f2, protocol);
    #else
    parseAvpRule(n->child, com->avp_f2, protocol);
    #endif
    com->avp_f2->reverse();
    n = UtilXML::getNextElementNode(n);
  }
}

void
parseCommand(DOMNode *n, int protocol)
{
  std::string name(UtilXML::getProp(n, "name"));
  std::string code(UtilXML::getProp(n, "code"));
  char defaultAppId[11]; // max value in decimal is "4294967296".
  sprintf(defaultAppId, "%d", currentApplicationId);
  std::string appId(UtilXML::getProp(n, "application-id", defaultAppId));
  std::string pbit(UtilXML::getProp(n, "pbit", "0"));
  std::string ebit(UtilXML::getProp(n, "ebit", "0"));
  std::string scenario(UtilXML::getProp(n, "scenario-id"));
  
#ifdef DEBUG
  AAA_LOG(LM_DEBUG, "name = %s, code = %s, applid = %s, scenario=%s\n", 
	       name.c_str(), code.c_str(), appId.c_str(), scenario.c_str());
#endif
	#ifndef MXML
  n = UtilXML::getNextElementNode(n->getFirstChild());
  #else
  n = UtilXML::getNextElementNode(n->child);
  #endif
  if (UtilXML::matchNode(n, "requestrules"))
  { 
    AAACommand *com = new AAACommand;
    std::string rname(name);
    rname += std::string("-Request");
    com->name = rname.c_str();
    com->code = atoi(code.c_str()); 
    com->appId = atoi(appId.c_str()); 
    com->flags.p = atoi(pbit.c_str()); 
    com->flags.r = 1;
    com->flags.e = atoi(ebit.c_str());
    com->scenario = scenario.c_str();
    com->protocol = protocol;
    #ifndef MXML
    parseCommandRule(n->getFirstChild(), com, protocol);
    #else
    parseCommandRule(n->child, com, protocol);
    #endif
    AAACommandList::instance()->add(com);
    n = UtilXML::getNextElementNode(n);
  }
  if (UtilXML::matchNode(n, "answerrules"))
  {
    AAACommand *com = new AAACommand;
    std::string aname(name);
    aname += std::string("-Answer");
    com->name = aname.c_str();
    com->code = atoi(code.c_str()); 
    com->appId = atoi(appId.c_str()); 
    com->flags.p = atoi(pbit.c_str()); 
    com->flags.r = 0;
    com->flags.e = atoi(ebit.c_str()); 
    com->protocol = protocol;
    com->scenario = scenario.c_str();
    #ifndef MXML
    parseCommandRule(n->getFirstChild(), com, protocol);
    #else
    parseCommandRule(n->child, com, protocol);
    #endif
    AAACommandList::instance()->add(com);
    n = UtilXML::getNextElementNode(n);
  }
}

void
parseTypedefn(DOMNode *n)
{
  std::string typeName(UtilXML::getProp(n, "type-name"));
  std::string typeParent(UtilXML::getProp(n, "type-parent", "none"));
  std::string desc(UtilXML::getProp(n, "description", "none"));
#ifdef DEBUG
  std::cout << "type-name = " << typeName;
  std::cout << ", type-parent = " << typeParent;
  std::cout << "desc = " << desc << std::endl;
#endif
}


void
parseAvpType(DOMNode *n, AAA_AVPDataType& type)
{
  std::string typeName(UtilXML::getProp(n, "type-name"));
#ifdef DEBUG
  std::cout << "type-name = " << typeName << std::endl;
#endif
  do {
    AvpType *avpt;
    avpt = AvpTypeList::instance()->search(typeName.c_str());
    if (avpt == NULL)
    {
	   AAA_LOG(LM_ERROR, "Unknown AVP type %s.\n", typeName.c_str());
	   throw;
    }
    type = avpt->getType();
  } while(0);
}

void
parseGrouped(AAAGroupedAVPNode& gNode, int protocol )
{
  DOMNode *n = gNode.node;
  AAAGroupedAVP* gavp = gNode.gavp;
  #ifndef MXML
  n = UtilXML::getNextElementNode(n->getFirstChild());
  #else
  n = UtilXML::getNextElementNode(n->child);
  #endif
  gavp->avp_f = new AAAQualifiedAvpList(PARSE_TYPE_FIXED_HEAD);
  gavp->avp_r = new AAAQualifiedAvpList(PARSE_TYPE_REQUIRED);
  gavp->avp_o = new AAAQualifiedAvpList(PARSE_TYPE_OPTIONAL);
  gavp->avp_f2 = new AAAQualifiedAvpList(PARSE_TYPE_FIXED_TAIL);
  if (UtilXML::matchNode(n, "fixed"))
  {
  	#ifndef MXML
    parseAvpRule(n->getFirstChild(), gavp->avp_f, protocol);
    #else
    parseAvpRule(n->child, gavp->avp_f, protocol);
    #endif
    n = UtilXML::getNextElementNode(n);
  }
  if (UtilXML::matchNode(n, "required"))
  {
  	#ifndef MXML
    parseAvpRule(n->getFirstChild(), gavp->avp_r, protocol);
    #else
    parseAvpRule(n->child, gavp->avp_r, protocol);
    #endif
    n = UtilXML::getNextElementNode(n);
  }
  if (UtilXML::matchNode(n, "optional"))
  {
  	#ifndef MXML
    parseAvpRule(n->getFirstChild(), gavp->avp_o, protocol);
    #else
    parseAvpRule(n->child, gavp->avp_o, protocol);
    #endif
    n = UtilXML::getNextElementNode(n);
  }
  if (UtilXML::matchNode(n, "fixed"))
  {
  	#ifndef MXML
    parseAvpRule(n->getFirstChild(), gavp->avp_f2, protocol);
    #else
    parseAvpRule(n->child, gavp->avp_f2, protocol);
    #endif
    gavp->avp_f2->reverse();
    n = UtilXML::getNextElementNode(n);
  }
}

void
parseEnum(DOMNode *n, ACE_UINT32 avpCode)
{
  std::string name(UtilXML::getProp(n, "name"));
  std::string code(UtilXML::getProp(n, "code"));
#ifdef DEBUG
  std::cout << "name = " << name;
  std::cout << ", code = " << code << std::endl;
#endif
  AvpEnumeratedInfo::instance()->add(avpCode, atoi(code.c_str()));
}

void parseAvp(DOMNode *n, std::list<AAAGroupedAVPNode>& gavpNodeList,
         int protocol)
{
  std::string name(UtilXML::getProp(n, "name"));
  std::string code(UtilXML::getProp(n, "code"));
  std::string mayEncrypt(UtilXML::getProp(n, "may-encript", "yes"));
  std::string mandatory(UtilXML::getProp(n, "mandatory", "must"));
  std::string protect(UtilXML::getProp(n, "protected", "may"));
  std::string vendorid(UtilXML::getProp(n, "vendor-id", "0"));
  std::string scenarioid(UtilXML::getProp(n, "scenario-id", "0"));
  AAA_AVPDataType type;
  AAAGroupedAVP* gavp;

#ifdef DEBUG
  std::cout << "name = " << name;
  std::cout << ", code = " << code;
  std::cout << ", may-encrypt = " << mayEncrypt;
  std::cout << ", mandatory = " << mandatory;
  std::cout << ", protected = " << protect;
  std::cout << ", vendor-id = " << vendorid << std::endl;
  std::cout << ", scenario-id = " << scenarioid << std::endl;
  std::cout << ", protocol  = " << protocol << std::endl;
#endif
  int pos = name.find("_", 0);
  if( pos != std::string::npos) 
  {
//  	tmp_name = name;
#ifdef DEBUG_TEST
   	std::cout << "############parseAvp:orig_avp_name:"<< name << ",update avpName:" << name.substr(0, pos) << "scenario-id =" <<  scenarioid  << std::endl;
#endif
  	name = name.substr(0, pos);
  }

  pos = name.find("ALIAS-", 0 ) ;
  if( pos != std::string::npos) 
  {
#ifdef DEBUG_TEST
  	std::cout << "############2.parseAvp:orig_avp_name:"<< name << ",update avpName:" <<  name.substr(6, name.length()-6) << std::endl;
#endif
  	name = name.substr(6, name.length()-6);
  }

  pos = name.find("LCOCS-", 0 ) ;
  if( pos != std::string::npos) 
  {
  	char *p = new char [name.length()+1];
  	strcpy(p, name.c_str());
    MyAvpCodeMap[p] = atoi(code.c_str()) ;
//  	std::cout << "############2.LCOCS:avp_name:"<< name << ",avp_code:" <<  code << std::endl;
    
  }

	#ifndef MXML
  n = UtilXML::getNextElementNode(n->getFirstChild());
  #else
  n = UtilXML::getNextElementNode(n->child);
  #endif
  if (UtilXML::matchNode(n, "type"))
  {
    parseAvpType(n, type);
    n = UtilXML::getNextElementNode(n);
  }
  else if (UtilXML::matchNode(n, "grouped"))
  {
    AAAGroupedAVPNode gavpNode;
    type = AAA_AVP_GROUPED_TYPE;
    gavp = new AAAGroupedAVP;
    gavp->code = atoi(code.c_str());
    gavp->vendorId = atoi(vendorid.c_str()); 
    gavp->scenarioId = atoi(scenarioid.c_str()); 
    AAAGroupedAvpList::instance()->add(gavp);  // to be revisited
					   // after parsing
					   // all avps.
    gavpNode.gavp = gavp;
    gavpNode.node = n;
    gavpNodeList.push_back(gavpNode); // parse lator.
    n = UtilXML::getNextElementNode(n);
  }
  while (UtilXML::matchNode(n, "enum"))
  { 
    parseEnum(n, atoi(code.c_str()));
    n = UtilXML::getNextElementNode(n);
  }
  // create runtime avp dictionary 
  do {
    AAADictionaryEntry* avp = 
      new AAADictionaryEntry(atoi(code.c_str()),
			     name.c_str(),
			     type,
			     atoi(vendorid.c_str()),
			     atoi(scenarioid.c_str()),
			     AAA_AVP_FLAG_NONE,
                             protocol);
    avp->flags = AAA_AVP_FLAG_NONE;
    avp->flags |= 
      ((mayEncrypt == std::string("yes")) ? AAA_AVP_FLAG_ENCRYPT : 0);
    avp->flags |= 
      (((mandatory == std::string("may")) || 
	      (mandatory == std::string("must"))) ? 
       AAA_AVP_FLAG_MANDATORY : 0);
    avp->flags |= 
      (((protect == std::string("may")) || 
	      (protect == std::string("must"))) ?
       AAA_AVP_FLAG_END_TO_END_ENCRYPT : 0);
    avp->flags |= 
      ((vendorid == std::string("0")) ? 0 : AAA_AVP_FLAG_VENDOR_SPECIFIC);
    if (avp->avpCode != 0)  // Do not add "AVP" AVP
    {
	    AAAAvpList::instance()->add(avp);
#ifdef DEBUG
				std::cout << "#################add avpName:" << avp->avpName << std::endl;
#endif	      
    }
    else {
        delete avp;
    }
  } while (0);
}

void parseBase(DOMNode *n, int protocol)
{
  std::string uri(UtilXML::getProp(n, "uri", "default uri"));
  currentApplicationId = 0;
  DOMNode *comNode = NULL;  // Node that contains command definitions.
  std::list<AAAGroupedAVPNode> gavpNodeList;
#ifdef DEBUG
  std::cout << "uri = " << uri << std::endl;
#endif
	#ifndef MXML
  n = UtilXML::getNextElementNode(n->getFirstChild());
  #else
  n = UtilXML::getNextElementNode(n->child);
  #endif
  if (UtilXML::matchNode(n, "command"))
  {
    comNode = n;
    while (UtilXML::matchNode(n, "command"))
	  {
	    n = UtilXML::getNextElementNode(n);  // parse later
	  }
  }
  while (UtilXML::matchNode(n, "typedefn"))
  {
    parseTypedefn(n);
    n = UtilXML::getNextElementNode(n);
  }
  while (UtilXML::matchNode(n, "avp"))
  {
    parseAvp(n, gavpNodeList, protocol);
    n = UtilXML::getNextElementNode(n);
  }
  do 
  { // parse grouped avp definition now.
    std::list<AAAGroupedAVPNode>::iterator i;
    for (i=gavpNodeList.begin(); i!=gavpNodeList.end(); i++)
    {
	    parseGrouped(*i, protocol);
    }
  } while (0);
  n = comNode;
  while (UtilXML::matchNode(n, "command"))
  {
    parseCommand(n, protocol);
    n = UtilXML::getNextElementNode(n);
  }
}

void parseApplication(DOMNode *n, int protocol)
{
  std::string applicationId(UtilXML::getProp(n, "id"));
  currentApplicationId = atol(applicationId.c_str()); // Set the current applicationId.
  std::string applicationName(UtilXML::getProp(n, "name", "default application"));
  std::string uri(UtilXML::getProp(n, "uri", "default uri"));
  DOMNode *comNode = NULL;  // Node that contains command definitions.
  std::list<AAAGroupedAVPNode> gavpNodeList;
#ifdef DEBUG
  std::cout << "Application Identifier = " << applicationId << std::endl;
  std::cout << "Application Name = " << applicationName << std::endl;
  std::cout << "URI = " << uri << std::endl;
#endif
	#ifndef MXML
  n = UtilXML::getNextElementNode(n->getFirstChild());
  #else
  n = UtilXML::getNextElementNode(n->child);
  #endif
  if (UtilXML::matchNode(n, "command"))
  {
    comNode = n;
    while (UtilXML::matchNode(n, "command"))
	  {
	    n = UtilXML::getNextElementNode(n);  // parse later
	  }
  }

  while (UtilXML::matchNode(n, "typedefn"))
  {
    parseTypedefn(n);
    n = UtilXML::getNextElementNode(n);
  }
  while (UtilXML::matchNode(n, "avp"))
  {
    parseAvp(n, gavpNodeList, protocol);
    n = UtilXML::getNextElementNode(n);
  }
  do 
  { // parse grouped avp definition now.
    std::list<AAAGroupedAVPNode>::iterator i;
    for (i=gavpNodeList.begin(); i!=gavpNodeList.end(); i++)
    {
	    parseGrouped(*i, protocol);
    }
  } while (0);
  n = comNode;
  while (UtilXML::matchNode(n, "command"))
  {
    parseCommand(n, protocol);
    n = UtilXML::getNextElementNode(n);
  }
}

void
parseDictionary(DOMNode *n, int protocol)
{
  n = UtilXML::getNextElementNode(n);
  while (UtilXML::matchNode(n, "vendor"))
  {
    parseVendor(n);
    n = UtilXML::getNextElementNode(n);
  }
  parseBase(n, protocol);
  n = UtilXML::getNextElementNode(n);
  // Opening each Diameter application dictionary.
  while (UtilXML::matchNode(n, "application"))
  {
    parseApplication(n, protocol);
    n = UtilXML::getNextElementNode(n);
  }
}

void
parseTree(DOMNode *n)
{
  for (n = UtilXML::getNextElementNode(n); 
       n != NULL; n = UtilXML::getNextElementNode(n))
  {
    if (!UtilXML::matchNode(n, "dictionary"))
    {
	    AAA_LOG(LM_ERROR, "Bad dictionary format.\n");
	    throw;
	  }
    std::string proto(UtilXML::getProp(n, "protocol"));
    int currentProtocol = atol(proto.c_str()); // Set the current protocol.
    #ifndef MXML
    parseDictionary(n->getFirstChild(), currentProtocol);
    #else
    parseDictionary(n->child, currentProtocol);
    #endif
  }
}

#ifdef MXML
static mxml_type_t			/* O - Node type */
load_cb(mxml_node_t *node)		/* I - Node */
{
  if (!strcmp(node->value.element.name, "description"))
    return (MXML_OPAQUE);
  else
    return (MXML_TEXT);
}
#endif

void parseXMLDictionary(char* xmlFile)
{
	#ifndef MXML
  try
  {
    XMLPlatformUtils::Initialize();
  }
  catch (const XMLException& toCatch)
  {
    //ACE_UNUSED_ARG(toCatch);
    AAA_LOG(LM_ERROR, "Error during initialization! Message:\n");
    XMLPlatformUtils::Terminate();
    throw;
  }

  int errorCount = 0;
  XercesDOMParser::ValSchemes    valScheme = XercesDOMParser::Val_Auto;
  //  XercesDOMParser* parser = new XercesDOMParser;
  //  myDOMTreeErrorReporter *errReporter = new myDOMTreeErrorReporter();
  std::auto_ptr<XercesDOMParser> parser(new XercesDOMParser);
  std::auto_ptr<myDOMTreeErrorReporter> errReporter(new myDOMTreeErrorReporter());
  parser->setErrorHandler(errReporter.get());
  bool errorsOccured = false;

  parser->setValidationScheme(valScheme);
  try
  {
    parser->parse(xmlFile);
    errorCount = parser->getErrorCount();
    if (errorCount > 0)
    {
	    AAA_LOG(LM_ERROR, "Error while parsing Dictionary XML file.\n");
	    errorsOccured = true;
    }
  }
  catch (const DOMException& e)
  {
    //ACE_UNUSED_ARG(e);
    AAA_LOG(LM_ERROR, "DOM error.\n");
    errorCount = parser->getErrorCount(); 
    errorsOccured = true;
  }
  catch (const XMLException& e)
  {
    //ACE_UNUSED_ARG(e);
    AAA_LOG(LM_ERROR, "XML error.\n");
    errorsOccured = true;
  }

  if(errorsOccured)
  {
    //	delete errReporter;
    //	delete parser;
	  XMLPlatformUtils::Terminate();
	  throw (1);
  }
  
  DOMNode *doc = parser->getDocument();
  parseTree(doc->getFirstChild());
  #else
  FILE		*fp = NULL;			/* File to read */
  DOMNode	*parser = NULL;			/* XML documentation tree */
  DOMNode	*doc = NULL;			/* XML documentation tree */
  if ((fp = fopen(xmlFile, "r")) != NULL)
  {
    parser = mxmlLoadFile(NULL, fp, load_cb);
    fclose(fp);
    if (!parser)
    {
      AAA_LOG(LM_ERROR,"Unable to read the dictionary file \"%s\"!\n", xmlFile);
    }
    else if ((doc = mxmlFindElement(parser, parser, "dictionary", NULL,
                                      	NULL, MXML_DESCEND)) == NULL)
    {
      AAA_LOG(LM_ERROR, "Bad dictionary format.\n%s\n",xmlFile);
      mxmlDelete(parser);
      parser = NULL;
    }
  }
  else
  {
    parser     = NULL;
  }
  if (doc!=NULL)
  {
	  std::string proto(UtilXML::getProp(doc, "protocol"));
	  int currentProtocol = atol(proto.c_str()); // Set the current protocol.
	  parseDictionary(doc->child, currentProtocol);
  }
  mxmlDelete(parser);
  parser = NULL;
  #endif
}
