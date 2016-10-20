#include "JsonMgr.h"
#include <fstream>
using namespace std;

JsonMgr::JsonMgr()
{
    
}

JsonMgr::~JsonMgr()
{   
     
}


    
long JsonMgr::writeJsonFile(char * sFileName, vector<string> &vecStr, char sDiv)
{
    Json::Value Jroot;
    
    Json::Value JRecord;
    //char sLine[4096];
    string sLine;
    int index = 0;
    int iLineCnt=0;
    
    if(vecStr.size() < 1)
        return false;
        
    vector<string> vecHead;
    //strncpy(sLine, vecStr[0].c_str(), 4095);
    //sLine[4095]=0;
    sLine = vecStr[0];
    char * pbegin = (char *)sLine.c_str();
    char * p=0;
    p = strchr(pbegin, sDiv );
    while(p){
        *p = 0;
        if(*pbegin)vecHead.push_back(pbegin);
        p++;
        pbegin = p;
        p = strchr(pbegin, sDiv );
    }
    if( pbegin && (*pbegin) )
        vecHead.push_back(pbegin);

                
    for(int i=1; i<vecStr.size(); i++ ){
        
        //strncpy(sLine, vecStr[i].c_str(), 4095);
        //sLine[4095]=0;
        sLine = vecStr[i];
        index = 0;
        p=0;
        p = strchr((char *)sLine.c_str(), sDiv );
        pbegin = (char *)sLine.c_str();
        
        while(p || pbegin ){
            if(p) *p = 0;
            
            switch( vecHead[index].c_str()[0] ){
              case 'I':
                {
                int iTemp = atoi(pbegin);
                Json::Value theValue( iTemp );
                JRecord[ vecHead[index].c_str()+2 ] = theValue;
                }
                break;
              case 'F':
                {
                float fTemp = atof(pbegin);
                Json::Value theValue( fTemp );
                JRecord[ vecHead[index].c_str()+2 ] = theValue;
                }
                break;
              case 'S':
              default:
                {
                Json::Value theValue( pbegin );
                JRecord[ vecHead[index].c_str()+2 ] = theValue;
                }
                break;
                
            }
            
            index++;
            if( index>=vecHead.size() )
                break;
            
            if( !p )
                break;
                
            p++;
            pbegin = p;
            if(pbegin)  p = strchr(pbegin, sDiv );
        }
        
        Jroot[iLineCnt] = JRecord;
        iLineCnt++;
        
    }
    
   
    Json::FastWriter writer;
    

    
////    printf("\n%s\n" , sContent.c_str() );
    
    char sFileTemp[256]={0};
    snprintf( sFileTemp,sizeof(sFileTemp), "%s.tmp",sFileName );
    
    FILE *fp=fopen(sFileTemp, "w+" );
    if(fp ==NULL)
        return -1;
for(int i=0; i<Jroot.size(); i++ ){
    string sContent = writer.write(Jroot[i]);
    //sContent.append("\n");
    if(fwrite( sContent.c_str(), sContent.size(), 1, fp) !=1 ){
        fclose(fp);
        return -1;
    }
}
    fclose(fp);
    if( rename(sFileTemp, sFileName) )
        return -1;
    
    struct stat statbuf;
   stat(sFileName,&statbuf) ;
	 long  size;
    size = statbuf.st_size ;
	   return  size ;   
    
    
    
}


bool JsonMgr::writeKpiJson(char *sFileName,char* sKPI, 
    vector<string> &vecFileName , char *sOut, int iLen, int & iCnt )
{


    Json::Reader reader;

    Json::Value JrootNew;
    int iLineCnt=0;
    
    for(int i=0; i< vecFileName.size(); i++ ){
        ifstream file( vecFileName[i].c_str() );
        if(!file.is_open()){
            snprintf (sOut, iLen, "文件打开失败：%s", vecFileName[i] );
            return false;
        }
        Json::Value Jroot;
        Json::Value JRecord;
//        if  (!reader.parse(file, Jroot, false )) {
//            snprintf (sOut, iLen, "JSON文件解析失败：%s", vecFileName[i] );
//            return false;
//        }
        
        int iLine =0;


        while(!file.eof()){

   std::string stemp;
   std::getline(file, stemp ,'\n');
   	if (strlen(stemp.c_str()) <3) 
   		continue;
            if(!reader.parse(stemp, JRecord, false )) {

           // if(!reader.parse(file, JRecord, false )) {
                snprintf (sOut, iLen, "JSON文件%s的第%d行解析失败", 
                    vecFileName[i], iLine );
                file.close();    
                return false;
            }
            Jroot[iLine]=JRecord;
            iLine++;
        }
        
        file.close();
                
        for(int j=0; j< Jroot.size(); j++){
            JRecord = Jroot[j];
            if( strcmp( JRecord["KPIID"].asString().c_str() , sKPI  )==0){
                JrootNew[iLineCnt]=JRecord;
                iLineCnt++;
            }
        }
       

    }


    
    Json::FastWriter writer;
    
    char sFileTemp[256]={0};
    snprintf( sFileTemp,sizeof(sFileTemp), "%s.tmp",sFileName );
    
    FILE *fp=fopen(sFileTemp, "w+" );
    if(fp ==NULL){
        snprintf (sOut, iLen, "文件打开失败：%s", sFileTemp );
        return false;
    }


for(int i=0; i<JrootNew.size(); i++ ){
    string sContent = writer.write(JrootNew[i]);
    //sContent.append("\n");
    if(fwrite( sContent.c_str(), sContent.size(), 1, fp) !=1 ){
        fclose(fp);
        snprintf (sOut, iLen, "文件写入内容失败：%s", sFileTemp );
        return false;
    }
}
    

    fclose(fp);
    if( rename(sFileTemp, sFileName) ){
        snprintf (sOut, iLen, "文件改名失败：%s", sFileTemp );
        return false;
    }
    
    iCnt = JrootNew.size();
    return true;
    
    
}
