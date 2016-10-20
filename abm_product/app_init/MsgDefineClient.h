#define		MSG_PARAM_S_N				0x04000004		//获取用户所需的参数信息，命令如 hbparam -list memeroy.*，显示memeroy参数的一切子参数的信息
#define		MSG_PARAM_CHANGE			0x04001002		//修改参数值，命令如 hbparam -set memory.key=90006,memory.size=500
#define		MSG_PARAM_ACTIVE_MOUDLE_M	0x03001003		//修改系统参数，根据参数生效方式生效,非立即生效的提示生效方式 -refresh -
#define     MSG_PARAM_ACTIVE_MOUDLE_S	0x03001001      //修改系统参数，根据参数生效方式生效,非立即生效的提示生效方式 -refresh -s
#define     MSG_PARAM_S_C               0x03001005      //创建参数;命令如 hbparam -create memory.key
#define     MSG_PARAM_S_D               0x03001006      //删除参数;命令如 hbparam -delete memory.key
#define     MSG_PARAM_S_ALL             0x03001007      //显示所有参数:hbparam -listAll
#define		MSG_GAME_OVER				0x03001008		//系统停止
#define     MSG_STATE_CHANGE            0x03001009      //系统状态切换
#define     MSG_STATE_CHANGE_RETURN	0x03001010	//系统切换返回
#define	    MSG_PARAM_INIT_ERR		0x03001011	//参数格式
#define		MSG_PARAM_S_TIMING 	0x03001012
#define		MSG__RETURN_ERROR			0x04000100		//返回处理结果:失败
#define		MSG_NEW_VALUE_EFFECT	0x03001013	
#define     MSG_LIST_SECTION        0x03001014          //返回所有section节点名字

