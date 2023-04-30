// CRC32.cpp: implementation of the CCRC32 class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "CRC32.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CCRC32::CCRC32()
{
	m_pTable = NULL;
}
//////////////////////////////////////////////////////////////////////
CCRC32::~CCRC32()
{
	KillTable();
}
//////////////////////////////////////////////////////////////////////
void CCRC32::KillTable()
{
	if (m_pTable)
		delete [] m_pTable;
	m_pTable = NULL;
}
//////////////////////////////////////////////////////////////////////
void CCRC32::InitTable()
{
	if (!m_pTable)
	{
		m_pTable = new DWORD[256];

		DWORD c;
		int n, k;

		DWORD poly;            
		static const BYTE p[] = {0,1,2,4,5,7,8,10,11,12,16,22,23,26};

		poly = 0L;

		for (n = 0; n < sizeof(p); n++)
			poly |= 1L << (31 - p[n]);

		for (n = 0; n < 256; n++)
		{
			c = n;
			for (k = 0; k < 8; k++)
				c = c & 1 ? poly ^ (c >> 1) : c >> 1;
			m_pTable[n] = c;
		}

	}
}
//////////////////////////////////////////////////////////////////////
#define DO1(buf) crc = m_pTable[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);
//////////////////////////////////////////////////////////////////////
DWORD CCRC32::Calc(DWORD crc, LPBYTE lpData, DWORD dwSize)
{
	if (!m_pTable)
		InitTable();

    crc = crc ^ 0xffffffff;

    while (dwSize >= 8)
    {
      DO8(lpData);
      dwSize -= 8;
    }

    if (dwSize) 
	do 
	{
		DO1(lpData);
    } while (--dwSize);

    return crc ^ 0xffffffff;
}
//////////////////////////////////////////////////////////////////////
DWORD CCRC32::Calc(DWORD crc, LPCSTR lpFilename)
{
	DWORD dwCRC = 0;

	HFILE		hFile;
	OFSTRUCT	ofs;

	hFile = OpenFile(lpFilename, &ofs, OF_READ | OF_SHARE_DENY_WRITE);

	if (hFile != HFILE_ERROR)
	{
		DWORD dwSizeHigh	= 0;
		DWORD dwSize		= GetFileSize((HANDLE)hFile, &dwSizeHigh);

		if (dwSize)
		{
			LPBYTE lpBuf = new BYTE[dwSize];

			if (lpBuf)
			{
				DWORD dwActualSize = 0;

				if (ReadFile((HANDLE)hFile, lpBuf, dwSize, &dwActualSize, NULL))
				{
					if (dwSize == dwActualSize)
						dwCRC = Calc(crc, lpBuf, dwSize);
				}

				delete [] lpBuf;
			}
		}

		_lclose(hFile);
	}

	return dwCRC;
}
//////////////////////////////////////////////////////////////////////
