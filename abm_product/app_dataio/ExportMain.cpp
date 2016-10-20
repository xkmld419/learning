#include "DataExportor.h"
#include "ThreeLogGroup.h"
#include "TimesTenAccess.h"

ThreeLogGroup logg;


int main(int argc, char **argv)
{
    try
    {

        DataExportor exporter(argc, argv);

        exporter.run();
    }
    catch(...)
    {        
        logg.log(MBC_MEM_EXPORT_FAIL,LOG_LEVEL_FATAL,LOG_TYPE_SYSTEM,"内存导出异常。请检查相关文件权限或是表");
        return -1;
    }

    return 0;
}
