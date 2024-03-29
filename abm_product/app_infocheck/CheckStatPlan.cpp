#include "CheckStatPlan.h"

//#define _debug

TOCIQuery * CheckStatPlan::m_pLockQry = NULL;
PlanItem * CheckStatPlan::m_pPlan = NULL;

char ** CheckStatPlan::m_argv;

void CheckStatPlan::startPlan (PlanItem &oPlan, char ** argv)
{
    m_argv = argv;
    
    StatInterface::m_pPlan = &oPlan;
    
    //##置运行状态
    bool bret = updateState (oPlan.m_iPlanID, "RUN", "<Message> 开始执行...");
    
    if (!bret) {
        Log::log (0,"计划执行失败[Plan_ID=%d],因执行 updateState(RUN) 不成功", oPlan.m_iPlanID);
        return;
    }
    
    while (oPlan.m_iDebugFlag == 1)
        waitDebug (oPlan); //等待调试, 置B_Check_Stat_Plan.DEBUG_FLAG=0 后跳出循环
    
    try {
        //置下一次运行时间
        sleep (1);
        
        setNextRunTime (oPlan);
        
        if (oPlan.m_iAppType == 0)
        {
            switch (atoi (oPlan.m_sStatName))
            {
            
            	///////////////////////HSS数据信息点300000/////////////////////////////////////////
            	case 800001://2.2.1	数据接收平衡（AuditId=0001）
            	{
            		CheckStatInfo oReceBalance;		
            		oReceBalance.checkDataRecvBalance("0001",50,MIDL_FREG);
            		break;
            	}	
            	
            	case 800002://2.2.2	数据加载平衡（AuditId=0002）
            	{
            		CheckStatInfo oReceBalance;
            		oReceBalance.checkDataLoadBalance("0002",50,MIDL_FREG);
            		break;
            	}
            	
            	case 800003://2.2.3	数据入库平衡（AuditId=0003）
            	{
            		CheckStatInfo oReceBalance;
            		oReceBalance.checkDataInputBalance("0003",50,MIDL_FREG);
            		break;
            	}
            	
            	case 800004://2.2.4	数据发布平衡（AuditId=0004）
            	{
            		CheckStatInfo oCheckStatInfo;
            		oCheckStatInfo.checkOutputBalance();
            		break;
            	}
            	
            	case 800005://2.2.5	实例数变更（AuditId=0005）
            	{
            		CheckStatInfo oCheckStatInfo;
            		oCheckStatInfo.checkInstanceUpdate();
            		break;
            	}
            	
            	case 800006://2.2.6	进程日志（AuditId=0006）
            	{
                	CheckStatInfo oCheckHSSStatInfo;
                	oCheckHSSStatInfo.checkHSSAlertLog();            		
            		break;
            	}            	            	            	            	            	            	
            	
            	case 800007://2.2.7	鉴权异常（AuditId=0008）
            	{
            		CheckStatInfo oCheckStatInfo;
            		oCheckStatInfo.checkAuthenticationError();
            		break;
            	}   
            	
            	///////////////////////HSS信息点500000/////////////////////////////////////////
            	//2.3.1 接收接口处理（AuditId=0100）
            	case 502311://	省内非计费域接口（AuditId=0101）	性能指标（AuditId=0101,type=40）
            	{	
            		CheckStatInfo oCheckStatInfo;
            		oCheckStatInfo.checkNoBillingCap();
            		break;
            	}
             	case 502312://		集团HSS接口（AuditId=0102）	性能指标	（AuditId=0102,type = 40）
            	{	
            		CheckStatInfo oCheckStatInfo;
            		oCheckStatInfo.checkHssCap();
            		break;
            	}
            	//2.3.2 同步接口处理（AuditId=0200）
             	case 502321://		OFCS接口（AuditId=0201）	性能指标（AuditId=0201,type=40）
            	{	
            		CheckStatInfo oCheckStatInfo;
            		oCheckStatInfo.checkOfcsCap();
            		break;
            	}
             	case 502322://			OCS接口（AuditId=0202）		性能指标（AuditId=0202,type=40）
            	{	
            		CheckStatInfo oCheckStatInfo;
            		oCheckStatInfo.checkOcsCap();
            		break;
            	}           	      	
            	//2.3.4	系统状态（AuditId=0400）
            	case 502341://2.3.4.1	事件信息点（AuditId=0400,type=60）
            	{
            		CheckStatInfo oCheckHSSEventInfo;
            		oCheckHSSEventInfo.checkOcsCap();
            		break;
            	}	
            	
            	case 502342://2.3.4.2	告警信息点（AuditId=0400,type=20）
            	{
            		CheckStatInfo oCheckHSSSystemCaution;
            		oCheckHSSSystemCaution.checkHSSSystemCaution();
            		break;
            	}
            	         	
            	///////////////////////一下为计费信息点，对HSS无用处//////////////////////
              //数据信息点数据提取
               case 100001:  //##2.2.1	计费接收（AuditId=0001，Type=50）
                {
                	CheckStatInfo oCheckStatInfo(MODULE_CHECK_NORMAL_FILE);
                	oCheckStatInfo.checkNormalFile();
                  break;                	     
                }
               case 100002:  //##2.2.2	计费接收异常文件（AuditId=0002，Type=50）
                {
                	CheckStatInfo oCheckStatInfo(MODULE_CHECK_ERR_FILE);
                  oCheckStatInfo.checkErrFile();
                  break;                	     
                }
               case 100003:  //##2.2.3	预处理平衡（AuditId=0003，Type=50）
                {
                	CheckStatInfo oCheckStatInfo(MODULE_CHECK_FILE_PREP_BALANCE);
                  oCheckStatInfo.checkPrepBalance();
                  break;                	     
                }
               case 100004:  //##2.2.4	批价平衡（AuditId=0004，Type=50）
                {
                	CheckStatInfo oCheckStatInfo(MODULE_CHECK_FILE_PRICING_BALANCE);
                  oCheckStatInfo.checkPricingBalance();
                  break;                	     
                }
               case 100005:  //##2.2.5	入库平衡（AuditId=0005，Type=50）
                {
                	CheckStatInfo oCheckStatInfo(MODULE_CHECK_FILE_INSTORE_BALANCE);
                  oCheckStatInfo.checkInStoreBalance();
                  break;                	     
                }
               case 100006:  //##2.2.6	话单日志（AuditId=0006，Type=50）
                {
                	CheckStatInfo oCheckStatInfo;
                  oCheckStatInfo.checkTicketLog();
                  break;                	     
                }
               case 100007:  //##2.2.7	IDEP文件传输（AuditId=0007，Type=50）
                {
                	CheckStatInfo oCheckStatInfo;
                  oCheckStatInfo.checkTransLog();
                  break;                	     
                }
               case 100008:  //##2.2.8	累帐平衡（AuditId=0008，Type=50）
                {
                  //CheckDailyAcctItemAggr oCheckItem;
                  //oCheckItem.checkDailyAcctItemAggrNew();
                  break;                	     
                }                                                                                                                
                   
               case 100009:  //##2.2.9	日帐日志（AuditId=0009，Type=50）
                {
                 
                  break;                	     
                }
               case 100010:  //##2.2.10	月帐日志（AuditId=0010，Type=50）
                {
                  //CheckDailyAcctItemAggr oCheckItem;
                  //oCheckItem.checkMonthAcctItemAggrNew();                	
                  break;                	     
                }
               case 100011:  //##2.2.11	高额签到（AuditId=0011，Type=50）
                {
                	//CheckStatInfo oCheckStatInfo;
                 // oCheckStatInfo.checkSignHighFee();
                  break;                	     
                }
               case 100012:  //##2.2.12	省级高额（AuditId=0012，Type=50）
                {
                	//CheckStatInfo oCheckStatInfo;
                	//oCheckStatInfo.checkHighFeeServ();
                	/*
                	char sValue1[3+1]={0};
                	ParamDefineMgr::getParam("HIGH_FEE_ALARM", "LOG_OR_DB", sValue1);
                	int iValue=atoi(sValue1);
                	if(iValue >= 2)
                     oCheckStatInfo.checkHighFeeServ();
                  else 
                     oCheckStatInfo.checkHighFeeLog();
                   */
                  break;                	     
                }
               case 100013:  //##2.2.13	进程日志（AuditId=0013，Type=50）
                {
                	CheckStatInfo oCheckStatInfo;
                	oCheckStatInfo.checkAlertLog();
                	/*
                	char sValue1[3+1]={0};
                	ParamDefineMgr::getParam("INFO_DATA", "PROCESSLOG", sValue1);
                	int iValue=atoi(sValue1);
                	if(iValue == 1)                	
                    oCheckStatInfo.checkAlertLog();
                   else 
                    oCheckStatInfo.checkProcessLog();
                    */
                  break;                	     
                }
               case 100014:  //##2.2.14	计费处理话单延迟（AuditId=0014，Type=50）
                {
                	CheckStatInfo oCheckStatInfo(MODULE_CHECK_DELAY_FILE);
                  oCheckStatInfo.checkDelayFile();
                  break;                	     
                }
               case 100015:  //##2.2.15	模块处理性能指标（AuditId=0015，Type=50）
                {
                	CheckStatInfo oCheckStatInfo;
                	oCheckStatInfo.prepMouldeCapability();
                	oCheckStatInfo.pricingMouldeCapability();
                  oCheckStatInfo.checkMouldeCapabilityKpi();
                  break;                	     
                }
               case 100016:  //##2.2.16	系统处理损耗指标（AuditId=0016，Type=50）
                {
                	CheckStatInfo oCheckStatInfo;
                  oCheckStatInfo.checkSystemLossKpi()	;		
                  break;                	     
                }
                
                case 100017:  //##JS_20101215_001（AuditId=0060  Type=50）
                {
                	CheckStatInfo oCheckStatInfo;
                  oCheckStatInfo.checkDayAcctFee();		
                  break;                	     
                }
 
            // 信息点数据提取
            // 20000x 性能指标信息点
                case 200001:  //2.3.1.1	计费接收性能指标（AuditId=0100，Type=40）
                {
                	Information oCheckInfor;
                  oCheckInfor.checkGather()	;		
                  break;                	     
                }  
                case 200002:  //2.3.2.1.1	预处理性能指标（AuditId=0201，Type=40）
                {
                	Information oCheckInfor;
                	//oCheckInfor.checkBalance(MODULE_PREP);  //预处理告警中间数据生成
                  oCheckInfor.checkPerp()	;		
                  break;                	     
                }   
                case 200003:  //2.3.2.2.1	批价性能指标（AuditId=0202，Type=40）
                {
                	Information oCheckInfor;
                	//oCheckInfor.checkBalance(MODULE_PRICING) ;//批价告警中间数据生成  
                  oCheckInfor.checkPricing()	;		
                  break;                	     
                }   
                case 200004:  //2.3.2.3.1	入库性能指标（AuditId=0203，Type=40）
                {
                	Information oCheckInfor;
                	//oCheckInfor.checkBalance(MODULE_INSTORE) ;//入库告警中间数据生成
                  oCheckInfor.checkIndb()	;		
                  break;                	     
                }   
                case 200005:  //2.3.3.1.1	销账性能指标（AuditId=0301，Type=40）
                {
	                CheckStatInfo oCheckAddItem ;
                  oCheckAddItem.checkAcctCap("0301",40,MIDL_FREG)	;
                  break;                	     
                }
                case 200006:  //2.3.3.2.1	累帐性能指标（AuditId=0302，Type=40）
                {
                	CheckStatInfo oCheckAddItem ;
                  oCheckAddItem.checkAddItem()	;		
                  break;                	     
                }  
                case 200007:  //2.3.3.4.1	月帐性能指标（AuditId=0304，Type=40）
                {
                	CheckStatInfo oCheckMonthCap ;
                  oCheckMonthCap.checkMonthCap()	;		
                  break;                	     
                }     
                 
                case 200008:  //2.3.4.1.1	银行性能指标（AuditId=0401，Type=40）
                {
	                CheckStatInfo oCheckAddItem ;
                  oCheckAddItem.checkAcctCap("0401",40,MIDL_FREG)	;
                  break;                	     
                }
                case 200009:  //2.3.4.2.1	充值卡性能指标（AuditId=0402，Type=40）
                {
	                CheckStatInfo oCheckAddItem ;
                  oCheckAddItem.checkAcctCap("0402",40,MIDL_FREG)	;
                  break;                	     
                }      
                 case 200010:  //.3.3.5.1 欠费账龄性能指标（AuditId=0305，Type=40）
                {
                CheckStatInfo oCheckAddItem ;
                  oCheckAddItem.checkAcctCap("0305",40,HIGH_FREG) ;
                  break;                      
                } 
                                                                                          
               //20001x  告警指标信息点
               case 200012:  //2.3.2.1.2       ?
                {
                  Information oCheckInfor;
                  //oCheckInfor.checkBalance(MODULE_PREP);  //?¤′|àí???ˉ?D??êy?Yéú3é
                  oCheckInfor.checkPrepAlarm();
                  oCheckInfor.writePrepAlarm()  ;
                  break;
                }
                case 200013:  //2.3.2.2.2       
                {
                  Information oCheckInfor;
                  //oCheckInfor.checkBalance(MODULE_PRICING) ;//?ú?????ˉ?D??êy?Yéú3é
                  oCheckInfor.checkPricingAlarm();
                  oCheckInfor.writePricingAlarm()       ;
                  break;
                }
                case 200014:  //2.3.2.3.2      
                {
                  Information oCheckInfor;
                  //oCheckInfor.checkBalance(MODULE_INSTORE) ;//è??a???ˉ?D??êy?Yéú3é
                  oCheckInfor.checkIndbAlarm()    ;
                  oCheckInfor.writeIndbAlarm()  ;
                  break;
                }        
                
                case 200015:  //2.3.3.2.2	累帐告警指标（AuditId=0302，Type=20）
                {
                	CheckStatInfo oCheckInfor;
                  oCheckInfor.checkAddItemCaution()	;		
                  break;                	     
                }         
                        
                case 200020:  //2.3.3.5.2 欠费账龄告警指标（AuditId=0305，Type=20
                {
                CheckStatInfo oCheckAddItem ;
                  oCheckAddItem.checkAcctCaution("0305",20,HIGH_FREG) ;
                  break;                      
                } 
                /* 核心参数告警信息点*/
                case 300001:  //检查每日话单参数,话单总数，总时长，总流量
                {
                	CheckStatInfo oCheckInfo;
                  oCheckInfo.checkParamBalance()	;		
                  break;                	     
                }  
                
                case 300002:  //检查每日漫入漫出用户数
                {
                	CheckStatInfo oCheckInfo;
                  oCheckInfo.checkRoamServs()	;		
                  break;                	     
                }   
               /*
                case 400001:  //2.3.5.1	事件信息点（AuditId=0900，Type=60）
                {
                	CheckStatInfo checkEventInfo;
                  checkEventInfo.checkEventInfo()	;		
                  break;                	     
                }    
                */
    
                case 400002:  //2.3.4.3.1	产品配置指标（AuditId=0403，Type=30）
                {
                	Information checkInfo;
                  checkInfo.checkAllProductInfo()	;		
                  break;                	     
                }   
                case 400003:  //2.3.4.4.1	商品配置指标（AuditId=0404，Type=30）
                {
                	Information checkInfo;
                	  checkInfo.checkAllProductOfferInfo()	;		
                 	 break;                	     
                }  
                case 400004:  //2.3.4.5.1	定价计划配置指标（AuditId=0405，Type=30）
                {
                	Information checkInfo;
                  	checkInfo.checkAllPricingPlanInfo()	;		
                  break;                	     
                }  
                
                case 900001:  //2.3.5.1	系统状态性能指标（AuditId=0900，Type=40）
                {
	                CheckStatInfo oCheckAddItem ;
                  oCheckAddItem.checkAcctCap("0900",40,MIDL_FREG)	;
                  break;                	     
                }    
                               
                case 900002:  //2.3.5.2	系统状态事件信息点（AuditId=0900，Type=60）
                {
                	CheckStatInfo checkEventInfo;
                  checkEventInfo.checkHSSEventInfo()	;		
                  break;                	     
                }  
                
                case 900003:  //	2.3.5.3	告警信息点（AuditId=0900，Type=20）
                {
                	CheckStatInfo checkEventInfo;
                  	checkEventInfo.checkSystemCaution()	;		
                  break;                	     
                }
                case 900004 ://    2.3.5.4 VC接口性能指标 (AuditId=0203，Type=40)
                {
               		CheckStatInfo oCheckVCinter ;
                	oCheckVCinter.checkVCCable("0203",40,MIDL_FREG)	;
                	break;
                	
            	}
            	case 900005 ://    2.3.5.5 查询接口性能 (AuditId=0300，Type=40)
                {
               		CheckStatInfo oCheckSeekInter ;
                 	oCheckSeekInter.checkSeekInterCap("0300",40,MIDL_FREG);
                 	break;
                	
            	}
            	case 900006 ://    2.3.5.5 查询接口告警 (AuditId=0300，Type=20)
                {
               		CheckStatInfo oCheckSeekInter ;
                 	oCheckSeekInter.checkSeekInterAlarm("0300",20,MIDL_FREG); 
                 	break;            	
            	}
                                                                                                                                                                                                                                                                                                                                                                  
               default:
                {   //## 置未知统计项ID 错误
                   updateState (oPlan.m_iPlanID, "NUL", "<Error> 未知统计项(stat_name),解析错误", true);
                   return;
                }
            }
        }
        else if (oPlan.m_iAppType == 1) //## PLSQL块执行
        {
            DEFINE_QUERY (qry);
            char sql[4096];
            sprintf (sql,"%s", oPlan.m_sStatName);
            qry.setSQL (sql);
            qry.execute();
            qry.commit ();
        }
        else { //## 未知应用类型(app_type)
            updateState (oPlan.m_iPlanID, "NUL", "<Error> 未知应用类型(app_type),解析错误", true);
            return;
        }
    }
    catch (TOCIException &e)
    {
        DB_LINK->rollback ();
        char sMsg[2048];
        sprintf (sMsg, "<Error> %s \n %s", e.getErrMsg(), e.getErrSrc());
        updateState (oPlan.m_iPlanID, "ERR", sMsg, true);
        return;
    }
    //# TException
    catch (TException &e) 
    {
        DB_LINK->rollback ();
        char sMsg[2048];
        sprintf (sMsg, "<Error> %s ", e.GetErrMsg());
        updateState (oPlan.m_iPlanID, "ERR", sMsg, true);
        return;
    }
    catch (Exception &e) 
    {
        DB_LINK->rollback ();
        char sMsg[2048];
        sprintf (sMsg, "<Error> %s \n", e.descript());
        updateState (oPlan.m_iPlanID, "ERR", sMsg, true);
        return;
    }
    //# Other Exception
    catch (...) 
    {
        DB_LINK->rollback ();
        char sMsg[2048];
        sprintf (sMsg, "<Error> 程序异常退出！ \n");
        updateState (oPlan.m_iPlanID, "ERR", sMsg, true);
        return;
    }
        
    updateState (oPlan.m_iPlanID, "END", "<Message> 执行成功!", true);
}


//## ture:需要立即执行, false:不执行
bool CheckStatPlan::analyzePlan (PlanItem &oPlan)
{
    //##检查状态
    if (!strcmp(oPlan.m_sState,"RUN")) {
        
        //检查进程运行状态
        checkRun (oPlan.m_iPlanID);
        
        return false;
    }
    
    //##非运行状态的, 检查是否到达运行时间
    Date d;
    if (strcmp(d.toString(), oPlan.m_sNextRunTime) >= 0) {
        #ifdef _debug
            Log::log (0,"当前时间:%s, 计划运行时间:%s",d.toString(), oPlan.m_sNextRunTime);
        #endif
        return true;        
    }
    
    return false;
}

bool CheckStatPlan::checkRun (int iPlanID)
{
    int  iLineCnt = 0;
    
    lockPlan ();
    
    DEFINE_QUERY (qry);
    
    qry.setSQL ("select sys_pid, exec_name, state from B_Check_Stat_Plan "
        " where nvl(sys_pid,0)>0 and plan_id = :vPlanID " 
    );
    qry.setParameter ("vPlanID", iPlanID);
    qry.open ();
    
    while (qry.next ()) {
        
        if (strcmp(qry.field("state").asString(),"RUN"))
            break;
        
        char sCommand[100];
        FILE *fp;
        sprintf (sCommand, "ps -p %ld -f|grep %s|awk '{print $2}'", 
                qry.field(0).asLong(), qry.field(1).asString());        
        fp = popen (sCommand, "r");
        if (fp == NULL) {
            Log::log (0, "popen error!");
            break;
        }        
        while (!feof (fp)) {
            char sPID[10];
            if (fscanf (fp, "%s\n", sPID) == EOF)
                break;
            iLineCnt ++;
        }
        pclose (fp);
        
        if (iPlanID == 0)
            break;
        
        if (iLineCnt == 0) { //进程不存在
            Log::log (0,"子进程(pid=%ld) %s 不存在",
                    qry.field(0).asLong(), qry.field(1).asString());
            updateState (iPlanID, "ERR", "<Alarm> 主进程检测到子进程未能正常退出");
        }
        
        break;
    }
    unlockPlan ();
    qry.close();
    
    if (iLineCnt)
        return true;
    else
        return false;
}

void CheckStatPlan::lockPlan ()
{
    if (m_pLockQry) 
        delete m_pLockQry;
    m_pLockQry = new TOCIQuery (DB_LINK);
    
    m_pLockQry->setSQL ("Lock table B_Check_Stat_Plan in exclusive mode");
    m_pLockQry->execute ();
}

void CheckStatPlan::unlockPlan ()
{
    if (!m_pLockQry) {
        DB_LINK->commit();
        return;
    }
    
    m_pLockQry->commit();
    
    delete m_pLockQry;
    m_pLockQry = NULL;
}

void CheckStatPlan::openPlan ()
{
    DEFINE_QUERY (qry);
    
    qry.setSQL (" SELECT "
        "  Plan_id,"
        "  app_type,"
        "  stat_name,"
        "  interval_type,"
        "  time_interval,"
        "  nvl(next_run_time, sysdate) next_run_time,"
        "  nvl(run_parameter,'') run_parameter, "
        "  nvl(state,'END') state,"
        "  nvl(debug_flag,0) debug_flag, "
        "  nvl(data_time_offset, 0) offset, "
        "  file_directory "
        " From B_Check_Stat_Plan"
        " where open_state=1 and nvl(state,'END')<>'NUL' and Plan_id>0 "
        " order by plan_id desc"
    );
    
    qry.open();

    if (m_pPlan)
        delete m_pPlan;
    
    while (qry.next ())
    {
        PlanItem *p = new PlanItem ();
        
        p->m_iPlanID = qry.field("Plan_id").asInteger();
        p->m_iAppType = qry.field("app_type").asInteger();
        strcpy (p->m_sStatName, qry.field("stat_name").asString());
        p->m_iIntervalType = qry.field("interval_type").asInteger();
        strcpy (p->m_sTimeInterval, qry.field("time_interval").asString());
        strcpy (p->m_sNextRunTime, qry.field("next_run_time").asString());
        strcpy (p->m_sParameter, qry.field("run_parameter").asString());
        strcpy (p->m_sState, qry.field("state").asString());
        p->m_iDebugFlag = qry.field("debug_flag").asInteger();
        
        p->m_iDataTimeOffset = qry.field("offset").asInteger();
        if (p->m_iDataTimeOffset < 0)
            p->m_iDataTimeOffset = 0;
            
        strncpy(p->m_sDirectory, qry.field("file_directory").asString(), 1000);
        
        parsePlan (p);
        
        p->m_pNext = m_pPlan;
        m_pPlan = p;
    }
    
    qry.close ();
}

void CheckStatPlan::parsePlan (PlanItem *p)
{
    Date d;
    d.getTimeString (p->m_sDataBeginTime);
    d.getTimeString (p->m_sDataEndTime);    
    
   // p->m_iDataType = 0; //默认 提取动态刷新指标
     p->m_iDataType = 1 ;
    
    if (p->m_iDataTimeOffset>0/*数据偏移时间*/  
        && p->m_iIntervalType==1/*时间间隔类型*/) 
    {
        Date d (p->m_sNextRunTime);
        
        d.addSec (0 - p->m_iDataTimeOffset);
        strcpy (p->m_sDataEndTime, d.toString());
        
        d.addSec (0 - atoi(p->m_sTimeInterval));
        strcpy (p->m_sDataBeginTime, d.toString()); 
        
        p->m_iDataType = 1; //判定为增量指标
    }
    else if (p->m_iDataTimeOffset == 0 //针对动态刷新指标
        && p->m_iIntervalType==1)
    {
        strcpy (p->m_sDataEndTime, p->m_sNextRunTime);
        Date d (p->m_sNextRunTime);
        d.addSec (0 - atoi(p->m_sTimeInterval));
        strcpy (p->m_sDataBeginTime, d.toString());
    }
    else if (p->m_iIntervalType==2)
    {
        strcpy (p->m_sDataEndTime, p->m_sNextRunTime);
        Date d (p->m_sNextRunTime);
    	d.addMonth(-1);
        strcpy (p->m_sDataBeginTime, d.toString());   
        p->m_iDataType = 2; //每月固定值 	
    }	
    
}

bool CheckStatPlan::getPlan (PlanItem &oPlan)
{
    if (!m_pPlan)
        return false;
    
    PlanItem *p = m_pPlan;
    m_pPlan = m_pPlan->m_pNext;
    
    p->m_pNext = NULL;
    oPlan = *p;
    
    delete p;

    return true;
}

void CheckStatPlan::closePlan ()
{
    if (m_pPlan)
        delete m_pPlan;
}

void CheckStatPlan::setNextRunTime (PlanItem &oPlan)
{
    Date d (oPlan.m_sNextRunTime);
    Date dNow;
    char sDate[15];
    char sMsg[1024] = {0};
    
    if (oPlan.m_iIntervalType == 1) //时间间隔(秒)
    {
        int iAddSecs = atoi (oPlan.m_sTimeInterval);
        if (iAddSecs <= 0) {
            strcpy (sDate, "30000101000000");
        }
        else {            
            d.addSec (iAddSecs);
            
            if (oPlan.m_iDataType == 0) {
                while (strcmp(d.toString(), dNow.toString()) <= 0)
                    d.addSec (iAddSecs);
                
                //修正动态刷新指标的数据时间
                Date dData (d.toString());
                dData.addSec (0 - atoi(oPlan.m_sTimeInterval));
                strcpy (oPlan.m_sDataEndTime, dData.toString());
                dData.addSec (0 - atoi(oPlan.m_sTimeInterval));
                strcpy (oPlan.m_sDataBeginTime, dData.toString());
            }
            
            strcpy (sDate, d.toString());
        }
    }
    else if (oPlan.m_iIntervalType == 2) //每个月固定时间
    {
        strcpy (sDate, oPlan.m_sNextRunTime);
        while (strcmp(sDate, dNow.toString()) <= 0)
            getNextMonth (sDate, oPlan.m_sTimeInterval);
    }
    else {
        //##异常, 置无穷大
        strcpy (sDate, "30000101000000");
        strcpy (sMsg, "interval_type 字段值非法, 请检查修改");
    }
    
    //update table    
    DEFINE_QUERY (qry);
    if (sMsg[0])
    {
        qry.setSQL ("update B_Check_Stat_Plan "
            " set next_run_time = to_date(:vDate,'yyyymmddhh24miss'),"
            " Message = :vMsg "
            " where plan_id = :vPlanID "
        );
        qry.setParameter ("vMsg", sMsg);
    }
    else {
        qry.setSQL ("update B_Check_Stat_Plan "
            " set next_run_time = to_date(:vDate,'yyyymmddhh24miss')"
            " where plan_id = :vPlanID "
        );
    }
    qry.setParameter ("vPlanID", oPlan.m_iPlanID);
    qry.setParameter ("vDate", sDate);
    qry.execute();
    qry.close();
    
    if (sMsg[0]) {
        qry.setSQL ("Insert into b_check_stat_log ( "
                " log_id, plan_id, msg_time, message ) "
                " values (:vLogID, :vPlanID, sysdate, '[RUN] '||:vMsg )"
        );
        qry.setParameter ("vLogID", getNextStatLogID());
        qry.setParameter ("vPlanID", oPlan.m_iPlanID);
        qry.setParameter ("vMsg", sMsg);
        qry.execute();
    }
    qry.commit ();
    
    #ifdef _debug
        Log::log (0, "设置下一次运行时间:%s", sDate);
    #endif
}

void CheckStatPlan::getNextMonth (char sDate[], const char sTimeInterval[])
{
    char sTemp[15];
    
    DEFINE_QUERY (qry);
    qry.setSQL ("select to_char(Add_months(to_date(:vDate,'YYYYMMDDHH24MISS'),1),'YYYYMMDDHH24MISS') from dual");
    qry.setParameter ("vDate", sDate);
    qry.open();
    qry.next();
    strcpy (sDate, qry.field(0).asString());     
    
    strcpy (sTemp, sDate);
    strncpy(sTemp+6, sTimeInterval, 8);
    sTemp[14] = 0;
    
    try {
        DEFINE_QUERY (qry);
        qry.setSQL ("Select to_char(to_date(:vDate,'YYYYMMDDHH24MISS'),'YYYYMMDDHH24MISS') from dual");
        qry.setParameter ("vDate", sTemp);
        qry.open();
        qry.next();
        strcpy (sDate, qry.field(0).asString()); //使用修正后的时间
    }
    catch (TOCIException &e) {
        return;
    }
}
    

void CheckStatPlan::waitDebug (PlanItem &oPlan)
{
    DEFINE_QUERY (qry);
    
    static bool bFirst(true);
    
    if (bFirst) {
        qry.setSQL ("update B_Check_Stat_Plan set message = :vMsg where plan_id = :vPlanID");
        qry.setParameter ("vMsg", "[RUN] 等待调试...");
        qry.setParameter ("vPlanID", oPlan.m_iPlanID);
        qry.execute();
        qry.commit();
        bFirst = false;
    }
    
    qry.close();
    qry.setSQL ("select debug_flag from B_Check_Stat_Plan where plan_id=:vPlanID");
    qry.setParameter ("vPlanID", oPlan.m_iPlanID);
    qry.open();
    if (qry.next())
        oPlan.m_iDebugFlag = qry.field(0).asInteger();
    
    sleep (3);
}


bool CheckStatPlan::updateState (const int iPlanID, const char *sState,
            const char *sMsg, const bool bLock)
{
    int iFailedAdd = 0, iSucessAdd=0;
    
    char sExeName[256], sMessage[1024];
    strcpy (sExeName, m_argv[0]);
    trimExeName (sExeName);
    
    sprintf (sMessage, "[%s] %s", sState, sMsg);
    
    if (!strcmp(sState,"ERR"))      //执行失败
        iFailedAdd ++;
    else if (!strcmp(sState,"END")) //执行成功
        iSucessAdd ++;
    
    if (bLock)
        lockPlan ();
    
    try 
    {
        DEFINE_QUERY (qry);
        qry.setSQL ("update B_Check_Stat_Plan "
            " set state = :vState, "
            " failed_times = failed_times + :vFailedAdd, "
            " sucess_times = sucess_times + :vSucessAdd, "
            " state_date = sysdate ,"
            " message = :vMsg "
            " where plan_id = :vPlanID"
        );
        qry.setParameter ("vState",sState);
        qry.setParameter ("vFailedAdd", iFailedAdd);
        qry.setParameter ("vSucessAdd", iSucessAdd);
        qry.setParameter ("vMsg", sMessage);
        qry.setParameter ("vPlanID", iPlanID);
        qry.execute ();
        
        if (!strcmp (sState,"RUN")) {
            qry.close();
            qry.setSQL ("update B_Check_Stat_Plan "
                " set sys_pid = :vPid, exec_name = :vExecName "
                " where plan_id = :vPlanID " );
            qry.setParameter ("vPid", getpid());
            qry.setParameter ("vExecName", sExeName);
            qry.setParameter ("vPlanID", iPlanID);
            qry.execute ();
        }
        
        qry.close();
        qry.setSQL ("Insert into b_check_stat_log ( "
                " log_id, plan_id, msg_time, message ) "
                " values (:vLogID, :vPlanID, sysdate, :vMsg )"
        );
        qry.setParameter ("vLogID", getNextStatLogID());
        qry.setParameter ("vMsg", sMessage);
        qry.setParameter ("vPlanID", iPlanID);
        qry.execute ();
        qry.commit ();        
        if (bLock)  unlockPlan ();
        
        if (!strcmp (sState,"ERR")) 
        {
            cout << endl << sMsg <<endl;
            writeErrIntoInterface (iPlanID, sMsg);
        }
        
        return true;
    }
    catch (TOCIException &e) 
    {
        DB_LINK->rollback ();
        if (bLock)  unlockPlan ();
        
        Log::log (0, "%s\n %s", e.getErrMsg(), e.getErrSrc());
        return false;
    }
}

void CheckStatPlan::writeErrIntoInterface (int iPlanID, const char *sMsg)
{
    //MNT_SELF_WARN_01: 任务ID; 告警时间; 告警信息
    StatInterface oWarn ("MNT_SELF_WARN_01");
    
    Date d;
    
    sprintf (oWarn.sDimensions, "%d;%s;%s", iPlanID, d.toString(), sMsg);
    
    //oWarn.insert (true);
}
