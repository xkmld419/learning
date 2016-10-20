#include "stdio.h"
#include "Xml.h"
#include "stdlib.h"

int main(int argc, char* argv[])
{
	//加载新数据
	Xml newXml;	
	if(!newXml.loadFile(argv[1]))
	{
		cout<<"请检查"<<argv[1]<<"文件格式!"<<endl;
		return false;
	}
	
	newXml.m_xml.FindElem(); //"help"节点
	string option=newXml.m_xml.GetAttrib("option");
	
	/*
	newXml.m_xmlHelp->m_versionHelp.m_sVersion="";
	newXml.m_xmlHelp->m_versionHelp.m_sEnvironment="";
	*/
	newXml.loadCommand();
	newXml.loadParam();
	
	//加载目标文件
  char filename[200];
  const char* file=getenv("BILLDIR");
  sprintf(filename,"%s/etc/help.xml", file);
	CMarkup oldXml;
  if(!oldXml.Load(filename))
  {
		cout<<"加载目标文件错误!"<<endl;
		return false;
	}
	
	if(strcmp(option.c_str(),"add")==0)
	{
		/******************************添加命令****************************/
		oldXml.FindChildElem("commandHelp");
		oldXml.IntoElem();
			
		vector <struct command *>::iterator iter=newXml.m_xmlHelp->m_pCommandHelp.begin();
  	for(;iter!=newXml.m_xmlHelp->m_pCommandHelp.end();++iter)
  	{
  		//新加一个命令			
			bool bFind=false;
			while(oldXml.FindChildElem("command"))
			{
				oldXml.IntoElem();
				string cmd_value=CMarkup::UTF8ToA(oldXml.GetAttrib("code"));
				if(strcmp(cmd_value.c_str(),(*iter)->m_sCode.c_str())==0)
				{
					bFind=true;
					break;
				}
				oldXml.OutOfElem();
			}
			
			int size=(*iter)->m_pSubcommand.size();
			if(!bFind&&size>0)
			{
				oldXml.AddChildElem("command");
				oldXml.IntoElem();
				oldXml.AddAttrib("code",CMarkup::AToUTF8((*iter)->m_sCode.c_str()));
			}
  		
  		vector <struct subcommand *>::iterator subiter=(*iter)->m_pSubcommand.begin();
  		for(;subiter!=(*iter)->m_pSubcommand.end();++subiter)
  		{
  			bool bSubFind=false;
				while(oldXml.FindChildElem("subcommand"))
				{
					oldXml.IntoElem();
					string subcmd_value=CMarkup::UTF8ToA(oldXml.GetAttrib("subcode"));
					if(strcmp(subcmd_value.c_str(),(*subiter)->m_sSubCode.c_str())==0)
					{
						bSubFind=true;
						break;
					}
					oldXml.OutOfElem();
				}
				
				if(!bSubFind)
				{
					//新加子命令
					oldXml.AddChildElem("subcommand");
					oldXml.IntoElem();
					oldXml.AddAttrib("subcode",CMarkup::AToUTF8((*subiter)->m_sSubCode.c_str()));
					
					oldXml.AddChildElem("function",CMarkup::AToUTF8((*subiter)->m_sFunction.c_str()));
					oldXml.AddChildElem("format",CMarkup::AToUTF8((*subiter)->m_sFormat.c_str()));
					oldXml.AddChildElem("description",CMarkup::AToUTF8((*subiter)->m_sDescription.c_str()));
						
					oldXml.OutOfElem();
					cout<<(*iter)->m_sCode.c_str()<<" "<<(*subiter)->m_sSubCode.c_str()<<" 子命令添加成功!"<<endl;
				}
				else
				{
					oldXml.OutOfElem();
					cout<<(*iter)->m_sCode.c_str()<<" "<<(*subiter)->m_sSubCode.c_str()<<" 子命令已经存在!"<<endl;
				}
				
				oldXml.ResetChildPos();
  		}
  		
  		if(bFind||(!bFind&&size>0))
  			oldXml.OutOfElem();
  			
  		oldXml.ResetChildPos();
  	}
  	
  	oldXml.OutOfElem();
  	
  	/******************************添加参数****************************/
  	oldXml.FindChildElem("paramHelp");
		oldXml.IntoElem();
			
  	vector <struct param *>::iterator paramiter=newXml.m_xmlHelp->m_pParamHelp.begin();
  	for(;paramiter!=newXml.m_xmlHelp->m_pParamHelp.end();++paramiter)
  	{
			//新加一个参数			
			bool bFind=false;
			while(oldXml.FindChildElem("param"))
			{
				oldXml.IntoElem();
				string param_value=CMarkup::UTF8ToA(oldXml.GetAttrib("code"));
				if(strcmp(param_value.c_str(),(*paramiter)->m_sCode.c_str())==0)
				{
					bFind=true;
					break;
				}
				oldXml.OutOfElem();
			}
			
			if(!bFind)
			{
				oldXml.AddChildElem("param");
				oldXml.IntoElem();
				oldXml.AddAttrib("code",CMarkup::AToUTF8((*paramiter)->m_sCode.c_str()));
				
				oldXml.AddChildElem("name",CMarkup::AToUTF8((*paramiter)->m_sName.c_str()));
				oldXml.AddChildElem("description",CMarkup::AToUTF8((*paramiter)->m_sDescription.c_str()));
				oldXml.AddChildElem("attributes");
				
				oldXml.IntoElem();
				oldXml.AddChildElem("rule",CMarkup::AToUTF8((*paramiter)->m_attributes.m_sRule.c_str()));
				oldXml.AddChildElem("range",CMarkup::AToUTF8((*paramiter)->m_attributes.m_sRange.c_str()));
				oldXml.AddChildElem("value",CMarkup::AToUTF8((*paramiter)->m_attributes.m_sValue.c_str()));
				
				oldXml.OutOfElem();
				oldXml.OutOfElem();
				
				cout<<(*paramiter)->m_sCode.c_str()<<" 参数添加成功!"<<endl;
			}
			else
			{
				oldXml.OutOfElem();
				cout<<(*paramiter)->m_sCode.c_str()<<" 参数已经存在!"<<endl;
			}
			
			oldXml.ResetChildPos();
  	}
  	
  	oldXml.OutOfElem();
	}
	else if(strcmp(option.c_str(),"delete")==0)
	{
		/******************************删除命令****************************/
		oldXml.FindChildElem("commandHelp");
		oldXml.IntoElem();
			
		vector <struct command *>::iterator iter=newXml.m_xmlHelp->m_pCommandHelp.begin();
  	for(;iter!=newXml.m_xmlHelp->m_pCommandHelp.end();++iter)
  	{
			//删除命令
  		bool bFind=false;
			while(oldXml.FindChildElem("command"))
			{
				oldXml.IntoElem();
				string cmd_value=CMarkup::UTF8ToA(oldXml.GetAttrib("code"));
				if(strcmp(cmd_value.c_str(),(*iter)->m_sCode.c_str())==0)
				{
					bFind=true;
					break;
				}
				oldXml.OutOfElem();
			}
			
			if(bFind)
			{
				int size=(*iter)->m_pSubcommand.size();
				if(size>0)
  			{
  				//删除子命令
  				vector <struct subcommand *>::iterator subiter=(*iter)->m_pSubcommand.begin();
  				for(;subiter!=(*iter)->m_pSubcommand.end();++subiter)
  				{
  					bool bSubFind=false;
						while(oldXml.FindChildElem("subcommand"))
						{
							oldXml.IntoElem();
							string subcmd_value=CMarkup::UTF8ToA(oldXml.GetAttrib("subcode"));
							if(strcmp(subcmd_value.c_str(),(*subiter)->m_sSubCode.c_str())==0)
							{
								bSubFind=true;
								break;
							}
							oldXml.OutOfElem();
						}
						
						if(bSubFind)
						{
							oldXml.RemoveElem();
							oldXml.OutOfElem();
							cout<<"成功删除 "<<(*iter)->m_sCode.c_str()<<" "<<(*subiter)->m_sSubCode.c_str()<<" 子命令!"<<endl;
						}
						else
						{
							cout<<"删除失败 没有 "<<(*iter)->m_sCode.c_str()<<" "<<(*subiter)->m_sSubCode.c_str()<<" 子命令!"<<endl;
						}
						oldXml.ResetChildPos();
  				}
  				
  				//子命令全部删除后,删除父命令节点
  				bool isTrue=oldXml.FindChildElem("subcommand");
  				if(!isTrue)
  				{
  					oldXml.RemoveElem();
  				}
  				
  				oldXml.OutOfElem();
  			}
  			else
  			{
					oldXml.RemoveElem();
					oldXml.OutOfElem();
					cout<<"成功删除 "<<(*iter)->m_sCode.c_str()<<" 命令!"<<endl;
  			}
			}
			else
			{
				cout<<"删除失败 没有 "<<(*iter)->m_sCode.c_str()<<" 命令!"<<endl;
			}
  		
  		oldXml.ResetChildPos();
		}
		
  	oldXml.OutOfElem();
		
		/******************************删除参数****************************/
		oldXml.FindChildElem("paramHelp");
		oldXml.IntoElem();
			
  	vector <struct param *>::iterator paramiter=newXml.m_xmlHelp->m_pParamHelp.begin();
  	for(;paramiter!=newXml.m_xmlHelp->m_pParamHelp.end();++paramiter)
  	{
			//删除参数
			bool bFind=false;
			while(oldXml.FindChildElem("param"))
			{
				oldXml.IntoElem();
				string param_value=CMarkup::UTF8ToA(oldXml.GetAttrib("code"));
				if(strcmp(param_value.c_str(),(*paramiter)->m_sCode.c_str())==0)
				{
					bFind=true;
					break;
				}
				oldXml.OutOfElem();
			}
			
			if(bFind)
			{
				oldXml.RemoveElem();
				oldXml.OutOfElem();
				cout<<"成功删除 "<<(*paramiter)->m_sCode.c_str()<<" 参数!"<<endl;
			}
			else
			{
				cout<<"删除失败 没有 "<<(*paramiter)->m_sCode.c_str()<<" 参数!"<<endl;
			}
			
			oldXml.ResetChildPos();
  	}
  	
  	oldXml.OutOfElem();
	}
	else if(strcmp(option.c_str(),"modify")==0)
	{
		cout<<"修改成功!"<<endl;
	}
	else
	{
		cout<<argv[1]<<"文件格式错误!"<<endl;
		return false;
	}
	
	oldXml.Save("./help.xml");
	
	return 1;
}
