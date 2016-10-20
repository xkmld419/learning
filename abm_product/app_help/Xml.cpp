#include "Xml.h"

Xml::Xml()
{
	bAlarmLog=true;
	Log::init(MBC_COMMAND_hbhelp);
	m_xmlHelp=new help;
	//memset(m_xmlHelp,0,sizeof(help));
	
	m_pCommandComm =new CommandCom();
	if(!m_pCommandComm->InitClient())
	{
		Log::log(0,"核心参数初始化失败。");
		bAlarmLog=false;
	}
	/*
	if(bAlarmLog)
	{
		m_pCommandComm->GetInitParam("LOG");
  	m_pCommandComm->SubscribeCmd("LOG",MSG_PARAM_CHANGE);
		Log::m_iAlarmLevel = m_pCommandComm->readIniInteger("LOG","log_alarm_level",-1);
		Log::m_iAlarmDbLevel = m_pCommandComm->readIniInteger("LOG","log_alarmdb_level",-1);
	}
	*/
}

Xml::~Xml()
{
	if(m_xmlHelp)
	{
		/*
		if(m_xmlHelp->m_pCommandHelp)
		{
			if(m_xmlHelp->m_pCommandHelp.m_pSubcommand)
			{
				for_each(m_xmlHelp->m_pCommandHelp.m_pSubcommand.begin(),
				m_xmlHelp->m_pCommandHelp.m_pSubcommand.end(),DeleteElem());
			}
			
			for_each(m_xmlHelp->m_pCommandHelp.begin(),m_xmlHelp->m_pCommandHelp.end(),DeleteElem());
		}
		if(m_xmlHelp->m_pParamHelp)
		{
			for_each(m_xmlHelp->m_pParamHelp.begin(),m_xmlHelp->m_pParamHelp.end(),DeleteElem());
		}
		*/
		if(m_xmlHelp->m_pCommandHelp.size()>0)
		{
			vector <command *>::iterator iter=m_xmlHelp->m_pCommandHelp.begin();
			for(;iter!=m_xmlHelp->m_pCommandHelp.end();iter++)
			{
				if((*iter)&&(*iter)->m_pSubcommand.size()>0)
				{
					vector <subcommand *>::iterator subiter=(*iter)->m_pSubcommand.begin();
					for(;subiter!=(*iter)->m_pSubcommand.end();subiter++)
					{
						DeleteElem(*subiter);
					}
					
					DeleteElem(*iter);
				}
			}
		}
		if(m_xmlHelp->m_pParamHelp.size()>0)
		{
			vector <param *>::iterator iter=m_xmlHelp->m_pParamHelp.begin();
			for(;iter!=m_xmlHelp->m_pParamHelp.end();iter++)
			{
				DeleteElem(*iter);
			}
		}
		
		delete m_xmlHelp;
		m_xmlHelp=NULL;
	}
}

bool Xml::load(const char* fileName)
{
  if(loadFile(fileName))
  {
		loadVersion();
		loadCommand();
		loadParam();
		
		return true;
	}
	
	return false;
}

bool Xml::loadFile(const char* file)
{
#ifdef DEF_HP	
    setlocale(LC_ALL,"zh_CN.gb18030");
#else
    setlocale(LC_ALL,"zh_CN");
#endif
	bool bResult = m_xml.Load(file);
	if(!bResult)
	{
		if(bAlarmLog)
		{
			ALARMLOG28(0,MBC_CLASS_File,101,"打开文件:%s失败",file);
		}
		else
		{
			cout<<"加载help.xml帮助文件错误!"<<endl;
		}
		return false;
	}
	
	return true;
}

bool Xml::loadVersion()
{
	//加载版本
	string ver_value;
	m_xml.FindChildElem("versionHelp");
	m_xml.IntoElem();
	
	m_xml.FindChildElem("version");
	ver_value = m_xml.GetChildData();
	m_xmlHelp->m_versionHelp.m_sVersion=CMarkup::UTF8ToA(ver_value);
	
	m_xml.FindChildElem("environment");
	ver_value = m_xml.GetChildData();
	m_xmlHelp->m_versionHelp.m_sEnvironment=CMarkup::UTF8ToA(ver_value);

	m_xml.OutOfElem();
	
	return true;
}

bool Xml::loadCommand()
{
	//加载命令
	string cmd_value;
	m_xml.FindChildElem("commandHelp");
	m_xml.IntoElem();
	
	command *cmd;
	while (m_xml.FindChildElem("command"))
	{
		cmd=new command;
		//memset(cmd,0,sizeof(command));
		
		m_xml.IntoElem();
		cmd_value=m_xml.GetAttrib("code");
		cmd->m_sCode=CMarkup::UTF8ToA(cmd_value);
		
		subcommand *sub_cmd;
		while(m_xml.FindChildElem("subcommand"))
		{
			sub_cmd=new subcommand;
			//memset(sub_cmd,0,sizeof(subcommand));
		
			m_xml.IntoElem();
			cmd_value=m_xml.GetAttrib("subcode");
			sub_cmd->m_sSubCode=CMarkup::UTF8ToA(cmd_value);
			
			m_xml.FindChildElem("function");
			cmd_value=m_xml.GetChildData();
			sub_cmd->m_sFunction=CMarkup::UTF8ToA(cmd_value);
			
			m_xml.FindChildElem("format");
			cmd_value=m_xml.GetChildData();
			sub_cmd->m_sFormat=CMarkup::UTF8ToA(cmd_value);
			
			m_xml.FindChildElem("description");
			cmd_value=m_xml.GetChildData();
			sub_cmd->m_sDescription=CMarkup::UTF8ToA(cmd_value);

			m_xml.OutOfElem();
			
			cmd->m_pSubcommand.push_back(sub_cmd);
		}

		m_xml.OutOfElem();
		
		m_xmlHelp->m_pCommandHelp.push_back(cmd);
	}
	
	m_xml.OutOfElem();
	
	return true;
}

bool Xml::loadParam()
{
	//加载参数
	string param_value;
	m_xml.FindChildElem("paramHelp");
	m_xml.IntoElem();
	
	param *parameter;
	while (m_xml.FindChildElem("param"))
	{
		parameter=new param;
		//memset(parameter,0,sizeof(param));
		
		m_xml.IntoElem();
		param_value=m_xml.GetAttrib("code");
		parameter->m_sCode=CMarkup::UTF8ToA(param_value);
		
		m_xml.FindChildElem("name");
		param_value=m_xml.GetChildData();
		parameter->m_sName=CMarkup::UTF8ToA(param_value);
		
		m_xml.FindChildElem("description");
		param_value=m_xml.GetChildData();
		parameter->m_sDescription=CMarkup::UTF8ToA(param_value);
		
		m_xml.FindChildElem("attributes");
		m_xml.IntoElem();
		
		m_xml.FindChildElem("rule");
		param_value=m_xml.GetChildData();
		parameter->m_attributes.m_sRule=CMarkup::UTF8ToA(param_value);
		
		m_xml.FindChildElem("range");
		param_value=m_xml.GetChildData();
		parameter->m_attributes.m_sRange=CMarkup::UTF8ToA(param_value);
		
		m_xml.FindChildElem("value");
		param_value=m_xml.GetChildData();
		parameter->m_attributes.m_sValue=CMarkup::UTF8ToA(param_value);

		m_xml.OutOfElem();
		
		m_xml.OutOfElem();
		
		m_xmlHelp->m_pParamHelp.push_back(parameter);
	}
	
	m_xml.OutOfElem();
	
	return true;
}

void Xml::parse(int argc,char argv[ARG_NUM][ARG_VAL_LEN])
{
	const char * appName="abmhelp";
	//if((argc==1)&&(strcmp(argv[0],appName)==0))
	if(argc==1)
        {
		displayCommand("abmhelp");
		/*
		displayVersion();
		displayCommand();
		displayParam();
		*/
	}
	if((argc>1)&&(strcmp(argv[0],appName)==0))
	{
		if(strcmp(argv[1],"-version")==0)
		{
			displayVersion();
		}
		else if(strcmp(argv[1],"-command")==0)
		{
			if(argc==2)
				displayCommand();
			else if(argc==3)
				displayCommand(argv[2]);
			else
				displayCommand(argv[2],argv[3]);
		}
		else if(strcmp(argv[1],"-param")==0)
		{
			if(argc==2)
				displayParam();
			else
				displayParam(argv[2]);
		}
		else
		{
			displayCommand("abmhelp");
			/*
			displayVersion();
			displayCommand();
			displayParam();
			*/
		}
	}
}

void Xml::displayVersion()
{
	cout<<"版本"<<endl<<"	"<<m_xmlHelp->m_versionHelp.m_sVersion.c_str()<<endl;
	cout<<"环境"<<endl<<"	"<<m_xmlHelp->m_versionHelp.m_sEnvironment.c_str()<<endl;
}

void Xml::displayCommand(char *cmdName/*=NULL*/,char *subcmdName/*=NULL*/)
{
	if(cmdName)
	{
		if(subcmdName)
		{
			vector <command *>::iterator iter=m_xmlHelp->m_pCommandHelp.begin();
  		for(;iter!=m_xmlHelp->m_pCommandHelp.end();++iter)
  		{
  			if(strcmp((*iter)->m_sCode.c_str(),cmdName)==0)
  			{
  				cout<<(*iter)->m_sCode.c_str()<<endl;
  				
  				vector <subcommand *>::iterator subiter=(*iter)->m_pSubcommand.begin();
  				for(;subiter!=(*iter)->m_pSubcommand.end();++subiter)
  				{
  					if(strcmp((*subiter)->m_sSubCode.c_str(),subcmdName)==0)
  					{
  						//cout<<"	"<<(*subiter)->m_sSubCode.c_str()<<endl;
  						cout<<"	"<<(*subiter)->m_sFormat.c_str()<<endl;
  						cout<<"	"<<(*subiter)->m_sFunction.c_str()<<endl;  						
  						cout<<"	"<<(*subiter)->m_sDescription.c_str()<<endl;
  					
  						return;
  					}
  				}
  				
  				cout<<"子命令错误："<<cmdName<<" 没有 "<<subcmdName<<" 子命令"<<endl;
  				return;
  			}
  		}
  		
  		cout<<"命令错误：没有 "<<cmdName<<" 命令"<<endl;
  		cout<<"abmhelp –command 可获取所有命令帮助"<<endl;
		}
		else
		{
			vector <command *>::iterator iter=m_xmlHelp->m_pCommandHelp.begin();
  		for(;iter!=m_xmlHelp->m_pCommandHelp.end();++iter)
  		{
  			if(strcmp((*iter)->m_sCode.c_str(),cmdName)==0)
  			{
  				cout<<(*iter)->m_sCode.c_str()<<endl;
  				
  				vector <subcommand *>::iterator subiter=(*iter)->m_pSubcommand.begin();
  				for(;subiter!=(*iter)->m_pSubcommand.end();++subiter)
  				{
  					//cout<<"	"<<(*subiter)->m_sSubCode.c_str()<<endl;
  					cout<<"	"<<(*subiter)->m_sFormat.c_str()<<endl;
  					cout<<"	"<<(*subiter)->m_sFunction.c_str()<<endl;  					
  					cout<<"	"<<(*subiter)->m_sDescription.c_str()<<endl;
  				}
  				
  				return;
  			}
  		}
  		
  		cout<<"命令错误：没有 "<<cmdName<<" 命令"<<endl;
  		cout<<"abmhelp –command 可获取所有命令帮助"<<endl;
		}
	}
	else
	{
		vector <command *>::iterator iter=m_xmlHelp->m_pCommandHelp.begin();
  	for(;iter!=m_xmlHelp->m_pCommandHelp.end();++iter)
  	{
  		cout<<(*iter)->m_sCode.c_str()<<endl;
  		
  		vector <subcommand *>::iterator subiter=(*iter)->m_pSubcommand.begin();
  		for(;subiter!=(*iter)->m_pSubcommand.end();++subiter)
  		{
  			//cout<<"	"<<(*subiter)->m_sSubCode.c_str()<<endl;
  			cout<<"	"<<(*subiter)->m_sFormat.c_str()<<endl;
  			cout<<"	"<<(*subiter)->m_sFunction.c_str()<<endl;
  			cout<<"	"<<(*subiter)->m_sDescription.c_str()<<endl;
  		}
  	}
	}
}

void Xml::displayParam(char *paramName/*=NULL*/)
{
	if(paramName)
	{
		vector <param *>::iterator iter=m_xmlHelp->m_pParamHelp.begin();
  	for(;iter!=m_xmlHelp->m_pParamHelp.end();++iter)
  	{
  		if(strcmp((*iter)->m_sCode.c_str(),paramName)==0)
  		{
  			cout<<"参数编码 "<<(*iter)->m_sCode.c_str()<<endl;
				cout<<"参数名称 "<<(*iter)->m_sName.c_str()<<endl;
				cout<<"参数描述 "<<(*iter)->m_sDescription.c_str()<<endl;
				
				cout<<"生效规则 "<<(*iter)->m_attributes.m_sRule.c_str()<<endl;
				cout<<"取值范围 "<<(*iter)->m_attributes.m_sRange.c_str()<<endl;
				cout<<"建议取值 "<<(*iter)->m_attributes.m_sValue.c_str()<<endl;
				
				return;
			}
  	}
  	
  	cout<<"参数错误：没有 "<<paramName<<" 参数"<<endl;
  	cout<<"abmhelp -param 可获取所有参数帮助"<<endl;
	}
	else
	{
		char ch;
		int i=0,j=PARAM_STEP;
		int size=m_xmlHelp->m_pParamHelp.size();
		vector <param *>::iterator iter=m_xmlHelp->m_pParamHelp.begin();
		while(1)
		{
  		for(;(i<j)&&(i<size);i++,iter++)
  		{
  			cout<<"参数编码 "<<(*iter)->m_sCode.c_str()<<endl;
				cout<<"参数名称 "<<(*iter)->m_sName.c_str()<<endl;
				cout<<"参数描述 "<<(*iter)->m_sDescription.c_str()<<endl;
				
				cout<<"生效规则 "<<(*iter)->m_attributes.m_sRule.c_str()<<endl;
				cout<<"取值范围 "<<(*iter)->m_attributes.m_sRange.c_str()<<endl;
				cout<<"建议取值 "<<(*iter)->m_attributes.m_sValue.c_str()<<endl;
				
				cout<<endl;
  		}
  		
  		if(i>=size)
				return;
			
  		ch=getch();
			
			switch(ch)
			{
				case '\n':
					j+=PARAM_STEP;
					break;
				case 'q':
				case 'Q':
					return;
				default :
					break;
			}
  	}
	}
}

char Xml::getch(void)
{       
	char buf = 0;
	struct termios old = {0};
	
	if(tcgetattr(0,&old)<0)
		perror("tcsetattr()");
	
	old.c_lflag &= ~ICANON;
	old.c_lflag &= ~ECHO;
	old.c_cc[VMIN] = 1;
	old.c_cc[VTIME] = 0;
	if(tcsetattr(0,TCSANOW,&old)<0)
		perror("tcsetattr ICANON");
	
	if(read(0,&buf,1) < 0)
		perror("read()");
	
	old.c_lflag |= ICANON;
	old.c_lflag |= ECHO;
	if(tcsetattr(0,TCSADRAIN,&old)<0)
		perror("tcsetattr ~ICANON");
	
	return (buf);
}
