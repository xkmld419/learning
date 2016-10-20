#include "SHMCmdSet.h"
#include <iostream>
using namespace std;

void showvalue(vector<SHMInfo> _vShmInfo)
{
	cout<<endl;
	for (int i =0; i<_vShmInfo.size(); i++) {
		cout<<_vShmInfo[i].m_sKey<<"(";
		cout<<_vShmInfo[i].m_lKey<<")"<<"   ";
		cout<<_vShmInfo[i].m_sOwner<<"   ";
		cout<<_vShmInfo[i].m_sIPCType<<"   ";
		cout<<_vShmInfo[i].m_sDataFrom<<"   ";
		cout<<_vShmInfo[i].m_sDataNbr<<"   ";
		cout<<_vShmInfo[i].m_iUsed<<"   ";
		cout<<_vShmInfo[i].m_iAllSize<<"   ";
		cout<<_vShmInfo[i].m_sUseRate<<"   ";
		cout<<_vShmInfo[i].m_iNattch<<"   ";
		cout<<_vShmInfo[i].m_iTotal<<"   ";
		cout<<_vShmInfo[i].m_sLoadTime<<"   "<<endl;
	}
	cout<<endl;
}

int run_onlyone(const char *filename)
{
	int  fd;
	char buf[15];
	memset(buf,'\0',sizeof(buf));
	if ((fd = open(filename, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0)
		return -1;
	// try and set a write lock on the entire file
	struct flock lock;
	lock.l_type = F_WRLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;

	if (fcntl(fd, F_SETLK, &lock) < 0) {
		if (errno == EACCES || errno == EAGAIN) {
			cout<<"使用相同环境变量PROCESS_ID的进程不能多次运行！"<<endl;
			exit(0);     // gracefully exit, daemon is already running
		} else {
			cout<<"特殊异常，进程退出！"<<endl;
			close(fd);
			return -1;
		}
	}
	// truncate to zero length, now that we have the lock
	if (ftruncate(fd, 0) < 0) {
		close(fd);
		return -1;
	}
	// and write our process ID
	sprintf(buf, "%ld\n", (long)getpid());
	if (write(fd, buf, strlen(buf)) != (int)strlen(buf)){
		close(fd);
		return -1;
	}

	// leave file open until we terminate: lock will be held
	return 0;
}


//内存管理进程
int run()
{
	int pid = fork();
	if (pid == 0) {
		char *p =getenv("SGW_HOME");
		char sFile[128] = {0};
		snprintf(sFile,sizeof(sFile)-1,"%s/src/app_petri/sgwshmadmin_n",p);
		run_onlyone(sFile);

		// 		SHMCmdSet::unloadSHMALL();
		// 		SHMCmdSet::createSHMALL();
		// 		SHMCmdSet::clearSHMALL();
		// 		SHMCmdSet::loadSHMALL();

		sleep(2);
		cout<<"共享内存管理进程启动成功……"<<endl;
		while (true) {
			sleep(30);
		}
		exit(0);
	} else if (pid == -1) {
		cout<<"共享内存管理进程启动失败……" <<endl;
	} else {
		exit(0);
	}
	return 0;
}

void help()
{
	printf("\n---------------------------------------------------\n");
	printf("    sgwshmtools -c				[创建所有共享内存的数据区和索引区]\n");
	printf("    sgwshmtools -c 文件名			[创建某文件的共享内存数据区和索引区]\n");
	printf("    sgwshmtools -p 文件名			[锁定共享内存全量数据]\n");
	printf("    sgwshmtools -v 文件名			[解锁共享内存全量数据]\n");
	printf("    sgwshmtools -l				[上载所有共享内存的数据区和索引区]\n");
	printf("    sgwshmtools -l 文件名			[上载某张文件的共享内存数据区和索引区]\n");
	printf("    sgwshmtools -q				[查询所有共享内存信息]\n");
	printf("    sgwshmtools -q 文件名			[查询某张文件的共享内存]\n");
	printf("    sgwshmtools -d				[卸载所有共享内存]\n");
	printf("    sgwshmtools -d 文件名			[卸载某张文件的共享内存]\n");
	printf("    sgwshmtools -k				[清空所有共享内存数据]\n");
	printf("    sgwshmtools -k 文件名			[清空某张文件的共享内存数据]\n");
	printf("    sgwshmtools -r 文件名			[重新分配某张文件共享内存空间大小]\n");
	printf("    sgwshmtools -m 				[验证核心参数配置是否正确]\n");
	printf("    sgwshmtools -n 				[启动共享内存管理进程]\n");
	printf("    sgwshmtools -e table			[数据导出到表table中]\n");
	printf("    sgwshmtools -show FimeName (IndexName=Value)[显示某张表共享内存数据]\n");
	printf("---------------------------------------------------\n\n");
	printf("目前支持的文件名有:\n    context_filter            tab_msg_map               service_context_list\
		   \n    stream_ctrl_info          stream_ctrl_data          userinfo\
		   \n    sgw_sla_que_rel           netInfodata               sessiondata\
		   \n    shmcongestleve            service_context_list_base loadbalancedata\
		   \n    base_method               service_package_route     s_packet_session\
		   \n    service_package_node      service_package_route_ccr service_package_judge\
		   \n    service_package_cca       service_package_func      service_package_variable\
		   \n    statisticscongestmsg      loadbalancedata           wf_process_mq\
		   \n    portinfo                  session_clear             sgw_org_route\
		   \n    sgw_org_head              channelInfo               sgw_global_map\
		   \n    sgw_user_staff            sgw_route_rule            sgw_area_org\
		   \n    sgw_tux_rela_in           sgw_tux_rela_out          sgw_svr_list\
		   \n    sgw_svrs_param_rela       sgw_svrs_param_list       sgw_svrs_register\
		   \n    sgw_param_ins_list        sgw_enum_area\n");
	printf("注意:\n    sgw_org_route               sgw_org_head\
		   \n    支持导出操作，其他暂不支持\n\n");
}

int main(int argc,char *argv[])
{
	if (argc<2||argv[1][0] !='-') {
		printf("\n[ERRNO]: 输入参数出错!\n\n");
		printf("[备注]：可通过 sgwshmtools -h  查询共享内存命令集的使用方式\n\n");
		return -1;
	}
	cout<<endl;

	try {
		switch(argv[1][1])
		{
		case 'c'://创建共享内存
			{
				if (argc==2) {
					SHMCmdSet::createSHMALL();
				} else if (argc == 3) {
					SHMCmdSet::createoneSHM(argv[2]);
				}
				break;
			}
		case 'r':
			if (argc == 3||argc == 4) {
				if (!SHMCmdSet::ralloconeSHM(argv[2])) {
					SHMCmdSet::loadoneSHM(argv[2],argv[3]);
				}
			} else if (argc == 2){
				cout<<"错误！重新分配参数共享内存空间大小不支持全量操作"<<endl;
			} else {
				cout<<"参数错误，命令启动方式如：sgwshmtools -r userinfo filename 形式"<<endl;
			}
			break;
		case 'q'://查询
			{
				vector<SHMInfo> _vShmInfo;
				if (argc==2) {
					SHMCmdSet::querySHMALL(_vShmInfo);
					showvalue(_vShmInfo);
				} else if (argc == 3) {
					SHMCmdSet::queryoneSHM(argv[2],_vShmInfo);
					showvalue(_vShmInfo);
				}
				break;
			}
		case 'd'://卸载
			{
				if (argc==2) {
					SHMCmdSet::unloadSHMALL();
				} else if (argc == 3) {
					SHMCmdSet::unloadoneSHM(argv[2]);
				}
			}
			break;
		case 'k'://清空
			{
				if (argc==2) {
					SHMCmdSet::clearSHMALL();
				} else if (argc == 3) {
					SHMCmdSet::clearoneSHM(argv[2]);
				}
			}
			break;
		case 'f'://断开连接
			{
				if (argc==2) {
					SHMCmdSet::detachSHMALL();
				} else if (argc == 3) {
					SHMCmdSet::detachoneSHM(argv[2]);
				}
			}
			break;
		case 'l'://上载
			if (argc==2) {
				SHMCmdSet::loadSHMALL();
			} else if (argc == 3) {
				SHMCmdSet::loadoneSHM(argv[2]);
			}
			break;
		case 'h'://帮助
			{
				help();
				break;
			}
		case 'p'://锁定共享内存
			{
				if (argc==2) {
					cout<<"参数错误，共享内存暂不支持全量解锁定共享内存"<<endl;
				} else if (argc == 3) {
					SHMCmdSet::lockoneSHM(argv[2]);
				}
			}
			break;
		case 'v': //解锁
			{
				if (argc==2) {
					cout<<"参数错误，共享内存暂不支持全量解锁定共享内存"<<endl;
				} else if (argc == 3) {
					char _sVale[32] = {0};
					char _sCmd[BUFF_MAX_SIZE] = {0};
					FILE* fp  = NULL;
					sprintf(_sCmd,"ps -ef|grep -w \"sgwshmtools -p %s\" |grep -v grep |awk '{print $2}'",argv[2]);
					fp = popen (_sCmd, "r");

					if (fp == NULL) {
						cout<<"该共享内存未被绑定"<<endl;
						return 0;
					}
					if (feof(fp)) {
						pclose (fp);
						cout<<"该共享内存未被绑定"<<endl;
						return false;
					}
					if (fscanf (fp, "%s\n",_sVale) == EOF) {
						pclose (fp);
						cout<<"该共享内存未被绑定"<<endl;
						return false;
					} else {
						pclose (fp);
						memset(_sCmd,0,sizeof(_sCmd));
						snprintf(_sCmd,sizeof(_sCmd),"kill -9 %d",atoi(_sVale));
						system(_sCmd);
						cout<<"共享内存解锁成功"<<endl;
					}
				}
			}
			break;
		case 'n':
			{
				cout<<"启动共享内存管理进程……"<<endl;
				run();
				break;
			}
		case 'm':
			{
				cout<<"开始验证共享内存参数管理核心参数配置信息是否正确……"<<endl;
				if (!SHMCmdSet::checkParam()) {
					cout<<"共享内存参数管理核心参数配置信息验证成功……"<<endl;
				} else {
					ThreeLogGroup _oLogObj;
					_oLogObj.Product_log(MBC_PARAM_ERROR,-1,-1,-1,2,2,-1,NULL,"共享内存参数管理核心参数配置信息不成功，请检查核心参数配置文件");
				}
				break;
			}
		case 'e'://数据导出
			{
				if (argc==2) {
					//SHMCmdSet::exportSHMALL();
				} else if (argc == 4) {
					SHMCmdSet::exportOneSHMALL(argv[2],argv[3]);
				}
			}
			break;
		default:
			break;
		}

		if (strcmp(argv[1],"-show") == 0) {
			if (argc==2) {
				cout<<"参数错误，命令启动方式如：sgwshmtools -show context_filter 形式"<<endl;
				return -1;
			} else if (argc == 3) {
				SHMCmdSet::querySHMData(argv[2],argv[3]);
			} else {
				cout<<"参数错误，命令启动方式如：sgwshmtools -show context_filter 形式"<<endl;
				return -1;
			}
		}
	}catch (TOCIException & toe){
		cout<<toe.getErrCode()<<endl;
		cout<<toe.getErrMsg()<<endl;
		cout<<toe.getErrSrc()<<endl;
		return -1;
	} catch (Exception &e) {
		cout<<e.descript()<<endl;
		cout<<"errno="<<errno<<endl;
		return -1;
	} catch (SITEException &se) {
		cout<<se.getErrMsg()<<endl;
		cout<<se.getErrSrc()<<endl;
		cout<<se.getErrCode()<<endl;
		return -1;
	}
}
