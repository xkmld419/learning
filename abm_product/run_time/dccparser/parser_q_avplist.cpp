#include <sys/types.h>
#include <list>
#include <string>
#include <iostream>
#include "parser.h"
#include "parser_avp.h"
#include "parser_q_avplist.h"
#include "q_avplist.h"
#include "resultcodes.h"    

static int success_result_flag = 1, request_flag = 1, avp_check_flag = 0;
static 
void parseRawToAppWithoutDict(AAAAvpHeaderList *ahl,
                              AAAAvpContainerList *acl);
  //     throw (AAAErrorStatus);

static
void parseRawToAppWithDict(AAAAvpHeaderList *ahl, AAAAvpContainerList *acl,
			   AAADictionary *dict, int listNum);// throw (AAAErrorStatus);

void setAvpCheck(int avpCheckFlag)
{
	avp_check_flag = avpCheckFlag;
}

int getAvpCheck()
{
	return avp_check_flag;
}

void setResultCode(int request, ACE_UINT32 result_code)
{
  if( request ) 
  {
    success_result_flag = 1;
  }
  else 
  {
	  if( result_code >= 2000 && result_code < 3000 ) 
	    success_result_flag = 1 ;
	  else 
	    success_result_flag = 0;
   }
//   printf("request_flag=%d\n", request_flag);
}


void setRequestFlag(int request)
{
  request_flag = request ;
//   printf("request_flag=%d\n", request_flag);
}

static
void parseAppToRawWithoutDict(AAAMessageBlock *msg,
                              AAAAvpContainerList *acl);
//     throw (AAAErrorStatus);

static
void parseAppToRawWithDict(AAAMessageBlock *msg, AAAAvpContainerList *acl,
			   AAADictionary *dict);// throw (AAAErrorStatus);

template<>
void
QualifiedAvpListParser::parseRawToApp()// throw (AAAErrorStatus)
{
  AAAAvpHeaderList ahl;
#ifdef DEBUG
  struct timeval start_timeval , end_timeval;

  // read buffer and create avp header list 
  gettimeofday(&start_timeval, NULL);
#endif
  try {
    ahl.create(getRawData());
  }
  catch (AAAErrorStatus st)
  {
    throw st;
  }
#ifdef DEBUG
   gettimeofday(&end_timeval, NULL);
   std::cout << "########: ahl.create---period:[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ) /1000 
	 << " ms]-[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec )  << " us]" << std::endl; 
#endif
  if (getDictData() == NULL)
  {
    ::parseRawToAppWithoutDict(&ahl, getAppData());
  }
  else
  {
    ::parseRawToAppWithDict(&ahl, getAppData(), getDictData(), getListNum());
  }
}

template<>
void
QualifiedAvpListParser::parseAppToRaw()// throw (AAAErrorStatus)
{

  if (getDictData() == NULL)
  {
    ::parseAppToRawWithoutDict(getRawData(), getAppData());
  }
  else
  {
    ::parseAppToRawWithDict(getRawData(), getAppData(), getDictData()); //, getListNum());
  }
}

static void
parseRawToAppWithoutDict(AAAAvpHeaderList *ahl, AAAAvpContainerList *acl)
  //  throw (AAAErrorStatus)
{
  AAAAvpContainer *c;
  AAAErrorStatus st;
  const char *name;
  AAAAvpContainerList::iterator i;

  for (i = acl->begin(); i != acl->end(); i++)
  {
    c = *i;
    c->ParseType() = PARSE_TYPE_OPTIONAL;
    name = c->getAvpName();
#ifdef DEBUG
      std::cout << __FUNCTION__ << ": Container "<< name << " matches\n";
#endif

    AAADictionaryEntry* avp;
    // use default dictionary only
    if ((avp = AAAAvpList::instance()->search(name)) == NULL)
	  {
      AAA_LOG(LM_ERROR, "No dictionary entry for %s avp.\n", name);
      st.set(BUG, MISSING_AVP_DICTIONARY_ENTRY);
      throw;
    }

    do 
	  {
      AvpParser ap;
      AvpRawData rawData;
      rawData.ahl = ahl;
      ap.setRawData(&rawData);
      ap.setAppData(c);
      ap.setDictData(avp);
      try {
        ap.parseRawToApp();
      }
      catch (AAAErrorStatus st)
      {
        int type, code;
        st.get(type, code);
        if (type == NORMAL && code == AAA_MISSING_AVP)
        {
          continue;
        }
        else
        {
        // Parse error 
          AAA_LOG(LM_ERROR, "Error in AVP %s.\n", name);
          throw;
        }
      }
	  } while (0);
  }
}

static void parseRawToAppWithDict(AAAAvpHeaderList *ahl, AAAAvpContainerList *acl,
		      AAADictionary *dict, int listNum)// throw (AAAErrorStatus)
{
  AAAQualifiedAVP *qavp;
  AAAAvpContainer *c;
  AAAErrorStatus st;
  AAAAvpContainerManager cm;
  unsigned int min, max;
  const char *name;
  int type, num, start_pos;
  AAAQualifiedAvpList::iterator i;
  AAAAvpParseType pt;
#ifdef DEBUG
  struct timeval start_timeval , end_timeval;
  struct timeval start_timeval1 , end_timeval1;
  long diff_time = 0;
#endif
  AAAQualifiedAvpList *qavp_l[4] = 
    {dict->avp_f, dict->avp_f2, dict->avp_r, dict->avp_o};

  if( listNum > 1 ) 
  {
  	start_pos = 4 - listNum ;
  	num = 4 ;
  }
  else 
  {
  	start_pos = 0 ;
  	num = listNum ;
  }

  
#ifdef DEBUG
  gettimeofday(&start_timeval1, NULL);
#endif
  for (int j=start_pos; j< num; j++)
  {
    for (i = qavp_l[j]->begin(); i != qavp_l[j]->end(); i++)
	  {
		
#ifdef DEBUG
      gettimeofday(&start_timeval, NULL);
#endif
      pt = qavp_l[j]->getParseType();
      qavp = *i;
      min  = qavp->qual.min;
      max  = qavp->qual.max;
      name = qavp->avp->avpName.c_str();
      type = qavp->avp->avpType;

#ifdef DEBUG
	    std::cout << __FUNCTION__ << ": Container "<< name << " matches\n";
#endif

      c = cm.acquire(name);
      c->ParseType() = pt;
      
      do 
	    {
#ifdef DEBUG
        gettimeofday(&end_timeval, NULL);
        std::cout << "########2: c->ParseType---period:[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ) /1000 
  			        << " ms]-[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec )  << " us]" << std::endl; 
        diff_time += ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec );
        gettimeofday(&start_timeval, NULL);
#endif

	      AvpParser ap;
	      AvpRawData rawData;
	      rawData.ahl = ahl;
	      ap.setRawData(&rawData);
	      ap.setAppData(c);
	      ap.setDictData(qavp->avp);
#ifdef DEBUG
        gettimeofday(&end_timeval, NULL);
        std::cout << "########3: c->ParseType---period:[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ) /1000 
  			        << " ms]-[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec )  << " us]" << std::endl; 
        diff_time += ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec );
        gettimeofday(&start_timeval, NULL);
#endif
	      try {
		      ap.parseRawToApp();
#ifdef DEBUG
        gettimeofday(&end_timeval, NULL);
        std::cout << "######## ap.parseRawToApp---period:[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ) /1000
  			        << " ms]-[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec )  << " us]" << std::endl; 
        diff_time += ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec );
        gettimeofday(&start_timeval, NULL);
#endif
	      }
	      catch (AAAErrorStatus st)
		    {
		      int type, code;
		      st.get(type, code);
		      if (type == NORMAL && code == AAA_MISSING_AVP)
		      {
		      // AVP was not found
		        c->releaseEntries();
		        cm.release(c);
            
/*		      if (pt == PARSE_TYPE_OPTIONAL || !request_flag) 
		        {
#ifdef DEBUG
		          gettimeofday(&end_timeval, NULL);
              std::cout << "########: Optional AvpParser---period:[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ) /1000 
   			        << " ms]-[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec )  << " us]" << std::endl; 
              diff_time += ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec );
#endif
			        continue;
            }
*/
		        AAA_LOG(LM_ERROR, "missing %s avp.\n", name);
		        throw;
		      }
		      else
		      {
		      // Parse error 
		        AAA_LOG(LM_ERROR, "Error in AVP %s.\n", name);
		        cm.release(c);
		        throw;
		      }
		    }
		    
	      int type, code;
	      ap.getErrorStatus(type, code);
		    if ( type == NORMAL ) 
		    {
		      if( code == AAA_MISSING_AVP )
		      {
		        // AVP was not found
		        c->releaseEntries();
		        cm.release(c);

		        if (pt == PARSE_TYPE_OPTIONAL || !request_flag) 
		        {
#ifdef DEBUG
		          gettimeofday(&end_timeval, NULL);
              std::cout << "########2: Optional AvpParser---period:[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ) /1000 
   			        << " ms]-[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec )  << " us]" << std::endl; 
              diff_time += ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec );
#endif
			        continue;
            }
		        st.set(type, code, qavp->avp);
		        AAA_LOG(LM_ERROR, "2.missing %s avp,type:[%d],code:[%d],pt:[%d].\n", name, type, code, pt);
		        throw st;
		      }
		      else if( code != 0 ) 
		      {
		        cm.release(c);
		        st.set(type, code);
		        AAA_LOG(LM_ERROR, "3.type:[%d],code:[%d],pt:[%d].\n", type, code, pt);
		        throw st;
		      }
		    }
        
	      // Check number of containers
	      if (c->size() < min)
		    {
		      AAA_LOG(LM_ERROR, "at lease min %s avp needed.\n", name);
		      st.set(NORMAL, AAA_MISSING_AVP, qavp->avp);
		      c->releaseEntries();
		      cm.release(c);
		      throw st;
		    }
	      if (c->size() > max)
		    {
		      AAA_LOG(LM_ERROR, "at most max %s avp allowed.\n", name);
		      st.set(NORMAL, AAA_AVP_OCCURS_TOO_MANY_TIMES, qavp->avp);
		      c->releaseEntries();
		      cm.release(c);
		      throw st;
		    }
#ifdef DEBUG
		    gettimeofday(&end_timeval, NULL);
        std::cout << "########: 3.AvpParser---period:[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ) /1000 
   			 << " ms]-[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec )  << " us]" << std::endl; 
        diff_time += ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec );
		    gettimeofday(&start_timeval, NULL);
#endif

	      acl->add(c);
#ifdef DEBUG
		    gettimeofday(&end_timeval, NULL);
        std::cout << "########: 4.AvpParser-acl->add--period:[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ) /1000 
   			 << " ms]-[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec )  << " us]" << std::endl; 
        diff_time += ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec );
		    gettimeofday(&start_timeval, NULL);
#endif
	    } while (0);
#ifdef DEBUG
		    gettimeofday(&end_timeval, NULL);
        std::cout << "########: AvpParser---period:[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec ) /1000 
   			 << " ms]-[" << ((end_timeval.tv_sec- start_timeval.tv_sec)*1000000 + end_timeval.tv_usec - start_timeval.tv_usec )  << " us]" << std::endl; 
		    gettimeofday(&start_timeval, NULL);
#endif
	  }
  }
#ifdef DEBUG
		    gettimeofday(&end_timeval1, NULL);
        std::cout << "########: parseRawToAppWithDict---period:[" << ((end_timeval1.tv_sec- start_timeval1.tv_sec)*1000000 + end_timeval1.tv_usec - start_timeval1.tv_usec ) /1000 
   			 << " ms]-[" << ((end_timeval1.tv_sec- start_timeval1.tv_sec)*1000000 + end_timeval1.tv_usec - start_timeval1.tv_usec )  << " us]" << std::endl; 
        std::cout << "########: diff-time:[" << diff_time << " us]\n" ;

#endif

}


static void
parseAppToRawWithoutDict(AAAMessageBlock *msg, AAAAvpContainerList *acl)
  //  throw (AAAErrorStatus)
{
  AAAAvpContainer *c;
  AAAErrorStatus st;
  const char *name;
  AAAAvpContainerList::iterator i;

  for (i = acl->begin(); i != acl->end(); i++)
  {
    c = *i;
    c->ParseType() = PARSE_TYPE_OPTIONAL;
    name = c->getAvpName();
    
    AAADictionaryEntry* avp;
    // use default dictionary only
    if ((avp = AAAAvpList::instance()->search(name)) == NULL)
    {
      AAA_LOG(LM_ERROR, "No dictionary entry for %s avp.\n", name);
      st.set(BUG, MISSING_AVP_DICTIONARY_ENTRY);
      throw;
    }
    
    if (c->size() == 0)
    {
      continue;
    }
    
    AvpParser ap;
    AvpRawData rawData;
    rawData.msg = msg;
    ap.setRawData(&rawData);
    ap.setAppData(c);
    ap.setDictData(avp);
    
    try {
     	ap.parseAppToRaw();
    }
    catch (AAAErrorStatus st)
    {
      AAA_LOG(LM_ERROR, "Error in AVP %s.\n", name);
      throw;
    }
  }
}

static void
parseAppToRawWithDict(AAAMessageBlock *msg, AAAAvpContainerList *acl,
   	              AAADictionary *dict)// throw (AAAErrorStatus);
{
  AAAQualifiedAVP *qavp;
  AAAAvpContainer *c;
  AAAErrorStatus st;
  unsigned int min, max;
  const char *name;
  int type;
  AAAQualifiedAvpList::iterator i;
  AAAAvpParseType pt;

  AAAQualifiedAvpList *qavp_l[4] = 
    {dict->avp_f, dict->avp_f2, dict->avp_r, dict->avp_o};
/*	printf("Start parseAppToRawWithDict...\n");
	std::vector<std::string> sAvpName;
	printf("!!!!!!!!Start disp all avp...\n");
	DispAllAvp(acl, sAvpName);
*/
//  printf("success_result_flag=%d\n", success_result_flag);
  for (int j=0; j<4; j++)
  {
    for (i = qavp_l[j]->begin(); i != qavp_l[j]->end(); i++)
		{
	  	pt = qavp_l[j]->getParseType();
	  	qavp = *i;
	  	min = qavp->qual.min;
	  	max = qavp->qual.max;
	  	name = qavp->avp->avpName.c_str();
	  	type = qavp->avp->avpType;

	  	if ((c = acl->search(qavp->avp)) == NULL)
	    {
	      if ( min > 0 && max > 0 && pt != PARSE_TYPE_OPTIONAL && success_result_flag )
				{
		  		AAA_LOG(LM_ERROR, "missing avp %s [%d] in container.\n", name, qavp->avp->scenarioId);
		  		st.set(BUG, MISSING_CONTAINER);
		  		throw st;
				}
	      continue;
	    }

	  	if (max == 0)
	    {
	      AAA_LOG(LM_ERROR, "%s must not appear in container.\n", name);
	      st.set(BUG, PROHIBITED_CONTAINER);
	      throw st;
	    }
	  	if (c->size() < min)
	    {
	      AAA_LOG(LM_ERROR, "less than min entries for the AVP.\n");
	      st.set(BUG, TOO_LESS_AVP_ENTRIES);
	      throw st;
	    }
	  	if (c->size() > max)
	    {
	      AAA_LOG(LM_ERROR, "more than max entries for the AVP.\n");
	      st.set(BUG, TOO_MUCH_AVP_ENTRIES);
	      throw st;
	    }
	  	if (c->size() == 0)
	    {
//	      AAA_LOG(LM_INFO, "container is empty.\n");
	      continue;
	    }

#ifdef DEBUG
	    std::cout << __FUNCTION__ << ": Container "<< name << " matches.\n";
#endif
      c->ParseType() = pt;
      AvpParser ap;
      AvpRawData rawData;
      rawData.msg = msg;
      ap.setRawData(&rawData);
      ap.setAppData(c);
      ap.setDictData(qavp->avp);
      
      try {
        ap.parseAppToRaw();
      }
      catch (AAAErrorStatus st)
      {
        AAA_LOG(LM_ERROR, "Error in AVP %s.\n", name);
        throw;
      }
#ifdef DEBUG
	    std::cout << __FUNCTION__ << ": 2.Container "<< name << " matches,type:" << type << ".\n";
#endif

	  }
  }
}

