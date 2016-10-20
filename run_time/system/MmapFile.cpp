#include "MmapFile.h"
#include "ABMException.h"
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

MmapFile::MmapFile(char *sFile, int iSize)
{
    snprintf(m_sFile, sizeof(m_sFile), "%s", sFile);
    m_iSize = iSize;
    m_pvAddr = NULL;
}

MmapFile::~MmapFile()
{
    if (m_pvAddr != NULL)
        closeMap();
}

int MmapFile::openMap(ABMException &oExp)
{
    int fd;
    if ((fd=open(m_sFile, O_RDWR|O_CREAT, 0666)) < 0) {
        ADD_EXCEPT1(oExp, "MmapFile::openMap open failed, errno=%d", errno);
        return -1;
    }
    
    struct stat flstat;
    if (fstat(fd, &flstat) < 0) {
        ADD_EXCEPT1(oExp, "MmapFile::openMap fstat failed, errno=%d", errno);
        close(fd);
        return -1;
    }
    if (m_iSize == 0)
    	m_iSize = flstat.st_size;
    if (flstat.st_size < m_iSize) {
        if (ftruncate(fd, m_iSize) < 0) {
            close(fd);
            return -1;
        }
    }

    if ((m_pvAddr=mmap(NULL, m_iSize, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED) {
        ADD_EXCEPT1(oExp, "MmapFile::openMap mmap failed, errno=%d", errno);
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}

int MmapFile::synFile(void *pAddr, int iSize)
{
    return msync(pAddr, iSize, MS_SYNC);
}

int MmapFile::closeMap()
{
    if (munmap(m_pvAddr, m_iSize) == 0) {
        m_pvAddr = NULL;
        return 0;
    }
    return -1;
}
