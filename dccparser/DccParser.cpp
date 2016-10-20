#include "DccMsgParser.h"
#include "DccParser.h"

static DccMsgParser *pDccMsgParser= NULL;
void BeginParser(char *sDictFile)
{
	if (pDccMsgParser!=NULL)
	{
		EndParser();
		RemoveDictionary();
	}
	pDccMsgParser = new DccMsgParser(sDictFile);
}

void EndParser()
{
	if (pDccMsgParser!=NULL)
	{
		delete pDccMsgParser;
		pDccMsgParser = NULL;
	}

}

void setDiameterHeader(int code, int request = true, int appId = 0, int hh=0,
		int ee=0) {
	if (pDccMsgParser==NULL)
	return;
	pDccMsgParser->setDiameterHeader(code, request, appId, hh, ee);
}

void setDiameterHeaderRequest(int request) {
	if (pDccMsgParser==NULL)
	return;
	pDccMsgParser->setDiameterHeaderRequest(request);

}
int parseRawToApp(void *buf, int bufSize,
		ParseOption option = PARSE_STRICT) {
	if (pDccMsgParser==NULL)
	return -1;
	return pDccMsgParser->parseRawToApp(buf, bufSize,option);
}
int parseAppToRaw(void *buf, int bufSize,
		ParseOption option = PARSE_STRICT) {
	if (pDccMsgParser==NULL)
	return -1;
	return pDccMsgParser->parseAppToRaw(buf, bufSize, option);
}

int getGroupedAvpNums(const char *avp_name) {
	if (pDccMsgParser==NULL)
	return -1;
	return pDccMsgParser->getGroupedAvpNums(avp_name);
}

int setAvpString(const char *avp_name, char *data) {
	if (pDccMsgParser==NULL)
	return -1;
	std::string sdata = data;
	return pDccMsgParser->setAvpValue(avp_name, sdata);
}
int getAvpString(const char *avp_name, char *data, int len) {
	if (pDccMsgParser==NULL)
	return -1;
	std::string sdata;
	int ret = pDccMsgParser->getAvpValue(avp_name, sdata);
	len = len > sdata.length() ? sdata.length() : len;
	memcpy(data, sdata.c_str(), len);
	return ret;
}

int setAvpUInt64(const char *avp_name, ACE_UINT64 data) {
	if (pDccMsgParser==NULL)
	return -1;
	return pDccMsgParser->setAvpValue(avp_name, data);
}
int getAvpUInt64(const char *avp_name, ACE_UINT64 *data) {
	if (pDccMsgParser==NULL)
	return -1;
	ACE_UINT64 tmp;
	int ret = pDccMsgParser->getAvpValue(avp_name, tmp);
	memcpy(data,&tmp,sizeof(ACE_UINT64));
	return ret;
}

int setAvpInt32(const char *avp_name, ACE_INT32 data) {
	if (pDccMsgParser==NULL)
		return -1;
	return pDccMsgParser->setAvpValue(avp_name, data);
}
int getAvpInt32(const char *avp_name, ACE_INT32 *data) {
	if (pDccMsgParser==NULL)
		return -1;
	ACE_INT32 tmp;
	int ret = pDccMsgParser->getAvpValue(avp_name, tmp);
	memcpy(data, &tmp, sizeof(ACE_INT32));
	return ret;
}

int setAvpUInt32(const char *avp_name, ACE_UINT32 data) {
	if (pDccMsgParser==NULL)
		return -1;
	return pDccMsgParser->setAvpValue(avp_name, data);
}
int getAvpUInt32(const char *avp_name, ACE_UINT32 *data) {
	if (pDccMsgParser==NULL)
		return -1;
	ACE_UINT32 tmp;
	int ret = pDccMsgParser->getAvpValue(avp_name, tmp);
	memcpy(data, &tmp, sizeof(ACE_UINT32));
	return ret;
}

int setAvpFloat(const char *avp_name, float data) {
	if (pDccMsgParser==NULL)
		return -1;
	return pDccMsgParser->setAvpValue(avp_name, data);
}

int getAvpFloat(const char *avp_name, float *data) {
	if (pDccMsgParser==NULL)
		return -1;
	float tmp;
	int ret = pDccMsgParser->getAvpValue(avp_name, tmp);
	memcpy(data, &tmp, sizeof(float));
	return ret;
}

int setAvpValue(const char *avp_name) {
	if (pDccMsgParser==NULL)
		return -1;
	return pDccMsgParser->setAvpValue(avp_name);
}

int getBufAvpInt32(char *msg_buf, char *avp_name, ACE_INT32 *data) {
	if (msg_buf==NULL||pDccMsgParser==NULL)
		return -1;
	std::string msg = msg_buf;
	ACE_INT32 tmp;
	int ret = pDccMsgParser->getAvpValue(msg, avp_name, tmp);
	memcpy(data, &tmp, sizeof(ACE_INT32));
	return ret;
}

void getAllAvp(bool disp_flag = true) {
	return pDccMsgParser->getAllAvp(disp_flag);
}

void printHeader(){
	return pDccMsgParser->printHeader();
}

void releaseMsgAcl(){
	return pDccMsgParser->releaseMsgAcl();
}

const char *getAvpOutStr(){
	return pDccMsgParser->getAvpOutStr().c_str();
}

const char *getHeaderOutStr(){
	return pDccMsgParser->getHeaderOutStr().c_str();
}

void getServiceType(char *serviceContextId, char *serviceType){
	std::string sServiceContextId = serviceContextId;
	return pDccMsgParser->getServiceType(sServiceContextId,serviceType);
}

int main(int argc,char **argv)
{
	return 0;
}
