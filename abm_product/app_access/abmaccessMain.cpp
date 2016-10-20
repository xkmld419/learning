/* add by  zhangyz */
/* date:2010 09 15 */

#include "abmaccessAdmin.h"
#include "abmmainmenu.h"
#include "abmlogin.h"
#include "Log.h"
//#include "HbAdminMgr.h"
void showAdminHelp()
{
    printf("\n ****************************************************************   \n") ;
	  printf("  1:进入交互式模式 \n") ;
	  printf("     ex:abmaccess \n");
	  printf("  2:进入命令行模式 \n");
    printf("     ex1:abmaccess 任意参数 \n");
    printf("  3:使用命令说明 \n");
    printf("     ex:abmaccess help \n");
    printf(" *****************************************************************   \n") ;
}

int main(int argc, char* argv[])
{
	
	if(argc > 1){
		if(argc == 2 && strcmp(argv[1],"help")==0){
			showAdminHelp();
			return 0;
		}
	//	if(bIsLogin==-1)
	//	{
       	//	Log::log (0, "请运行统一认证程序进行身份认证!谢谢!");
	//		return 0;
	//	}
	//	else if (bIsLogin == 0)
	//	{
       	//	Log::log (0, "用户未登陆，请登录后重试!谢谢!");
	//		return 0;			
	//	}
		hbaccessAdmin *phbAdmin = new hbaccessAdmin();
		phbAdmin->run();
		delete phbAdmin;
		phbAdmin = 0;
		return 0;
		
	}	
	else if(argc == 1) 
	{
	/*增加判断用户是否已经登陆*/
		//if(bIsLogin==-1)
		//{
       		//Log::log (0, "请运行统一认证程序进行身份认证!谢谢!");
		//	return 0;
	//	}	
	//	else if (bIsLogin == 0)
	//	{
       	//	Log::log (0, "用户未登陆，请登录后重试!谢谢!");
	//		return 0;			
	//	}
  //进入交互模式    
	   try {
	        signal (SIGINT, SIG_IGN);
	        signal (SIGQUIT, SIG_IGN);
	                
	        startwin();
	        //hblogin lg;
	        //lg.run();
	        hbmainmenu mm;
				  mm.run();
	        endwin();
	    }
    //# OCI Exception
    catch (TOCIException &e) {
        cout<<"[EXCEPTION]: TOCIException catched"<<endl;
        cout<<"  *Err: "<<e.getErrMsg()<<endl;
        cout<<"  *SQL: "<<e.getErrSrc()<<endl;
        Log::log (0, "收到TOCIException退出\n[TOCIEXCEPTION]: %s", e.getErrMsg());
    }

    //# Exception
    catch (Exception &e) {
        cout << "[EXCEPTION]: " << e.descript () << endl;
        Log::log (0, "收到Exception退出\n[EXCEPTION]: %s", e.descript());
    }

    //# Other Exception
    catch (...) {
        cout << "[EXCEPTION]: Exception catched" <<endl;
        Log::log (0, "收到其他Exception退出");
    }
	}
	//if(cHbLogin)
	//{
	//	delete cHbLogin; 
	//	cHbLogin = NULL;
	//}
    return 0;
}
