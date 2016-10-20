#include "mxml.h"

int main(int argc,char** argv)
{
	mxml_node_t *poXmlDoc = NULL;
        char inXml[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><c><u>уехЩ</u></c>";	
        //char inXml[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?><user_name>zhang</user_name>";	
	//poXmlDoc = mxmlLoadString(NULL, inXml,MXML_NO_CALLBACK );	
	poXmlDoc = mxmlLoadString(NULL, inXml,MXML_TEXT_CALLBACK );	
	
	return 0;
}

