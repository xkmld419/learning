#include "parser_avp.h"

void 
AAAErrorStatus::get(int& type, int& code, std::string& avp)
{
  type = this->type;
  code = this->code;
  avp = this->avp;
}

void 
AAAErrorStatus::get(int& type, int& code)
{
  type = this->type;
  code = this->code;
}

void
AAAErrorStatus::set(int type, int code)
{
  this->type = type;
  this->code = code;
}

void
AAAErrorStatus::set(int type, int code, AAADictionaryEntry* dict)
{
  AAAAvpHeader h;
  AAADictionaryEntry *avp = (AAADictionaryEntry*)dict;
  ACE_UINT32 avpSize = h.length = getMinSize(avp);
  char *buffer = new char[avpSize];
  AvpHeaderParser ahp;
  AAAMessageBlock *aBuffer = AAAMessageBlock::Acquire(buffer, avpSize);
  AvpRawData rawData;
  rawData.msg = aBuffer;
  ahp.setAppData(&h);
  ahp.setRawData(&rawData);
  ahp.setDictData(avp);
  this->type = type;
  this->code = code;
  ahp.parseAppToRaw();
  this->avp.assign(buffer, avpSize);
  aBuffer->Release();
  delete[] buffer;
}

void AAAErrorStatus::set(int type, int code,std::string avp)
{
  this->type = type;
  this->code = code;
  this->avp	 = avp;
}

void AAAErrorStatus::set(int type, int code,std::string avp,int avpcode)
{
  this->type = type;
  this->code = code;
  this->avp	 = avp;
  this->avpcode	 = avpcode;
}

void AAAErrorStatus::get(int& type, int& code, std::string& avp,int &avpcode)
{
  type = this->type;
  code = this->code;
  avp = this->avp;
  avpcode = this->avpcode;
}

