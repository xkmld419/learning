#include "DataImport.h"
#include "ThreeLogGroup.h"

ThreeLogGroup logg;


int main(int argc, char **argv)
{
    try
    {

        DataImport importer(argc, argv);

        importer.run();
    }
    catch(...)
    {        
        logg.log(MBC_MEM_IMPORT_FAIL,LOG_LEVEL_FATAL,LOG_TYPE_SYSTEM,"内存导入异常。请检查相关内存是否足够");
        return -1;
    }

    return 0;
}
