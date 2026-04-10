#include "file_routines.h"
#include <ace/utils/disk_file.h>
#include <ace/utils/file.h>
#include <proto/exec.h>
#include <exec/memory.h>

extern struct ExecBase *SysBase;

ULONG loadFileToChipMem(const char *filePath, void **dataOut)
{
	if (!filePath || !dataOut)
		return 0;

	*dataOut = NULL;

	// Open the file
	tFile *pFile = diskFileOpen(filePath, DISK_FILE_MODE_READ, 0);
	if (!pFile)
		return 0;

	// Get file size
	LONG fileSize = fileGetSize(pFile);
	if (fileSize <= 0)
	{
		fileClose(pFile);
		return 0;
	}

	// Allocate chip memory
	void *chipMem = AllocMem(fileSize, MEMF_CHIP | MEMF_CLEAR);
	if (!chipMem)
	{
		fileClose(pFile);
		return 0;
	}

	// Read file into chip memory
	ULONG bytesRead = fileRead(pFile, chipMem, fileSize);
	fileClose(pFile);

	if (bytesRead != (ULONG)fileSize)
	{
		FreeMem(chipMem, fileSize);
		return 0;
	}

	*dataOut = chipMem;
	return (ULONG)fileSize;
}

void freeChipMem(void *data, ULONG size)
{
	if (data && size > 0)
	{
		FreeMem(data, size);
	}
}
