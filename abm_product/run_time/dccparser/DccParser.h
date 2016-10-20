#ifndef DCCPARSER_H_
#define DCCPARSER_H_
#include <Basic_Types.h>
#define DLL_EXPORT extern "C" __declspec(dllexport)

DLL_EXPORT void BeginParser(char *sDictFile);
DLL_EXPORT void EndParser();
DLL_EXPORT void setDiameterHeader(int code, int request, int appId, int hh,
		int ee);
DLL_EXPORT void setDiameterHeaderRequest(int request);
DLL_EXPORT int parseRawToApp(void *buf, int bufSize,ParseOption option);
DLL_EXPORT int parseAppToRaw(void *buf, int bufSize,ParseOption option);
DLL_EXPORT int getGroupedAvpNums(const char *avp_name);
DLL_EXPORT int setAvpString(const char *avp_name, char *data);
DLL_EXPORT int getAvpString(const char *avp_name, char *data, int len);
DLL_EXPORT int setAvpUInt64(const char *avp_name, ACE_UINT64 data);
DLL_EXPORT int getAvpUInt64(const char *avp_name, ACE_UINT64 *data);

DLL_EXPORT int setAvpInt32(const char *avp_name, ACE_INT32 data);
DLL_EXPORT int getAvpInt32(const char *avp_name, ACE_INT32 *data);
DLL_EXPORT int setAvpUInt32(const char *avp_name, ACE_UINT32 data);
DLL_EXPORT int getAvpUInt32(const char *avp_name, ACE_UINT32 *data);
DLL_EXPORT int setAvpFloat(const char *avp_name, float data);
DLL_EXPORT int getAvpFloat(const char *avp_name, float *data);
DLL_EXPORT int setAvpValue(const char *avp_name);
DLL_EXPORT int getBufAvpInt32(char *msg_buf, char *avp_name, ACE_INT32 *data);
DLL_EXPORT void getAllAvp(bool disp_flag);
DLL_EXPORT void printHeader();
DLL_EXPORT void releaseMsgAcl();
DLL_EXPORT const char *getAvpOutStr();
DLL_EXPORT const char *getHeaderOutStr();
DLL_EXPORT void getServiceType(char *serviceContextId, char *serviceType);
#endif /*MSGPARSER_H_*/
