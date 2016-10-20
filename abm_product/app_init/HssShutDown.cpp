#include "HbShutDown.h"


/* 1先注册服务端          
    2,发送停止消息    接受停止消息
	*/

DEFINE_MAIN_EX(CHbShutDown)

CHbShutDown::CHbShutDown(void)
{
}

CHbShutDown::~CHbShutDown(void)
{
}

int CHbShutDown::run()
{
	if(!InitClient())
	{
		cout<<"服务没起,退出了"<<endl;
		return 0;
	}

	CmdMsg* pMsg = CreateNewMsg(MSG_GAME_OVER,"");
	if(send(pMsg))
	{
		int j=0;
		char* c[3]={"/ ","--","\\ "};
		CmdMsg* pRecvMsg=0;
		while(1)
		{	
			j++;
			int i=j%3;		
			cout<<"等待服务退出.........................................."<<c[i];
			usleep(100);
			cout<<'\r';			
			if((pRecvMsg = GetMsg())!=0)
			{
				//检查返回消息
				variant vt;
				//if(pRecvMsg->GetMsgContent(vt))
				{
					long lCmd = pRecvMsg->GetMsgCmd();
					pRecvMsg->GetMsgContent(vt);
					if(lCmd == MSG_PROCESS_ONEXIT)
					{
						cout<<vt.asStringEx()<<endl;
					}else
					{
						if(lCmd == MSG_PROCESS_EXIT)
						{
						//	cout<<vt.asStringEx()<<endl;
							exit(0);
						}
					}
					/*if(vt.isLong()||vt.isInteger())
					{
						if(vt == INIT_OP_OK)
							cout<<"服务退出..........................................Ok";
						else
						{
							cout<<"服务退出失败.................................:"<<vt.asLong()<<endl;
						}
						break;
					}*/
				}
			}			
		}
	}
	delete pMsg;

	return 0;
}

