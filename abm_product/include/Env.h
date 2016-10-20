#ifndef _ENV_H___
#define _ENV_H___

#define	ENV_HOME	"BILL_HOME"
#ifdef ABM_ENV
#undef ENV_HOME
#define ENV_HOME	"ABM_PRO_DIR"
#define CONFIG_FILE	"abmconfig"
#define SYSNAME "ABM"
#endif

#ifdef SGW_ENV
#undef ENV_HOME
#define ENV_HOME	"SGW_HOME"
#define CONFIG_FILE	"sgwconfig"
#endif

#endif
