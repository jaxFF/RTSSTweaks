// SZArray.cpp: implementation of the CSZArray class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "SZArray.h"
#include "DataCompressor.h"
//////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CSZArray::CSZArray()
{
	m_lpData			= NULL;
	m_dwReservedSize	= 0;
}
//////////////////////////////////////////////////////////////////////
CSZArray::~CSZArray()
{
	Uninit();
}
//////////////////////////////////////////////////////////////////////
void CSZArray::Uninit()
{
	if (m_lpData)
		delete [] m_lpData;

	m_lpData			= NULL;
	m_dwReservedSize	= 0;
}
//////////////////////////////////////////////////////////////////////
void CSZArray::AddEntry(LPCSTR lpsz, LPVOID lpEntryData0, DWORD dwDataSize0, LPVOID lpEntryData1, DWORD dwDataSize1)
{
	if (lpsz || (lpEntryData0 && dwDataSize0) || (lpEntryData1 && dwDataSize1))
	{
		if (m_lpData)
			//append already existing array
		{
			DWORD dwSizeInc			= sizeof(SZARRAY_ENTRY_HEADER);
				//calculate required data size increment
		
			if (lpEntryData0 && dwDataSize0)
				dwSizeInc += dwDataSize0;
					//adjust data size increment if data0 is specified

			if (lpEntryData1 && dwDataSize1)
				dwSizeInc += dwDataSize1;
					//adjust data size increment if data1 is specified

			if (lpsz)
				dwSizeInc += strlen(lpsz) + 1;
					//adjust data size increment if string is specified

			LPSZARRAY_HEADER pHdr	= (LPSZARRAY_HEADER)m_lpData;

			DWORD dwSizeOld			= pHdr->dwSize;
				//get old data size

			pHdr->dwNumEntries		+= 1;
			pHdr->dwSize			+= dwSizeInc;

			if (dwSizeOld + dwSizeInc <= m_dwReservedSize)
			{
				LPSZARRAY_ENTRY_HEADER pEntryHdr = (LPSZARRAY_ENTRY_HEADER)(m_lpData + dwSizeOld);
			
				pEntryHdr->dwSize		= dwSizeInc - sizeof(SZARRAY_ENTRY_HEADER);
				pEntryHdr->dwSizeData	= (lpEntryData0 ? dwDataSize0 : 0) + (lpEntryData1 ? dwDataSize1 : 0);

				if (lpEntryData0 && dwDataSize0)
					memcpy(m_lpData + dwSizeOld + sizeof(SZARRAY_ENTRY_HEADER), lpEntryData0, dwDataSize0);

				if (lpEntryData1 && dwDataSize1)
					memcpy(m_lpData + dwSizeOld + sizeof(SZARRAY_ENTRY_HEADER) + dwDataSize0, lpEntryData1, dwDataSize1);

				if (lpsz)
					strcpy_s((LPSTR)(m_lpData + dwSizeOld + sizeof(SZARRAY_ENTRY_HEADER) + pEntryHdr->dwSizeData), strlen(lpsz) + 1, lpsz);
			}
			else
			{
				LPBYTE lpData			= new BYTE[dwSizeOld + dwSizeInc];
					//allocate new array

				memcpy(lpData, m_lpData, dwSizeOld);
					//move old array to new one

				LPSZARRAY_ENTRY_HEADER pEntryHdr = (LPSZARRAY_ENTRY_HEADER)(lpData + dwSizeOld);
			
				pEntryHdr->dwSize		= dwSizeInc - sizeof(SZARRAY_ENTRY_HEADER);
				pEntryHdr->dwSizeData	= (lpEntryData0 ? dwDataSize0 : 0) + (lpEntryData1 ? dwDataSize1 : 0);

				if (lpEntryData0 && dwDataSize0)
					memcpy(lpData + dwSizeOld + sizeof(SZARRAY_ENTRY_HEADER), lpEntryData0, dwDataSize0);

				if (lpEntryData1 && dwDataSize1)
					memcpy(lpData + dwSizeOld + sizeof(SZARRAY_ENTRY_HEADER) + dwDataSize0, lpEntryData1, dwDataSize1);

				if (lpsz)
					strcpy_s((LPSTR)(lpData + dwSizeOld + sizeof(SZARRAY_ENTRY_HEADER) + pEntryHdr->dwSizeData), strlen(lpsz) + 1, lpsz);

				delete [] m_lpData;
					//deallocate old array
				m_lpData = lpData;
					//save ptr to new one
			}
		}
		else
			//create new array
		{
			DWORD dwSize			= sizeof(SZARRAY_HEADER) + sizeof(SZARRAY_ENTRY_HEADER);
			
			if (lpEntryData0 && dwDataSize0)
				dwSize += dwDataSize0;

			if (lpEntryData1 && dwDataSize1)
				dwSize += dwDataSize1;

			if (lpsz)
				dwSize += strlen(lpsz) + 1;

			m_lpData				= new BYTE[max(dwSize, m_dwReservedSize)];
				//allocate new data
			LPSZARRAY_HEADER pHdr	= (LPSZARRAY_HEADER) m_lpData;
				//get ptr to array header

			pHdr->dwSign			= 'SZA0';
			pHdr->dwSize			= dwSize;
			pHdr->dwNumEntries		= 1;
				//init array header

			LPSZARRAY_ENTRY_HEADER pEntryHdr	= (LPSZARRAY_ENTRY_HEADER)(m_lpData + sizeof(SZARRAY_HEADER));
				//get ptr to array header

			pEntryHdr->dwSize		= dwSize - sizeof(SZARRAY_ENTRY_HEADER) - sizeof(SZARRAY_HEADER);
			pEntryHdr->dwSizeData	= (lpEntryData0 ? dwDataSize0 : 0) + (lpEntryData1 ? dwDataSize1 : 0);

			if (lpEntryData0 && dwDataSize0)
				memcpy(m_lpData + sizeof(SZARRAY_HEADER) + sizeof(SZARRAY_ENTRY_HEADER), lpEntryData0, dwDataSize0);

			if (lpEntryData1 && dwDataSize1)
				memcpy(m_lpData + sizeof(SZARRAY_HEADER) + sizeof(SZARRAY_ENTRY_HEADER) + dwDataSize0, lpEntryData1, dwDataSize1);

			if (lpsz)
				strcpy_s((LPSTR)(m_lpData + sizeof(SZARRAY_HEADER) + sizeof(SZARRAY_ENTRY_HEADER) + pEntryHdr->dwSizeData), strlen(lpsz) + 1, lpsz);
					//copy string to new array
		}
	}
}
//////////////////////////////////////////////////////////////////////
LPSZARRAY_ENTRY_HEADER CSZArray::FindEntryHeaderByIndex(DWORD dwIndex)
{
	if (m_lpData)
	{
		LPSZARRAY_HEADER pHdr = (LPSZARRAY_HEADER)m_lpData;

		if (dwIndex < pHdr->dwNumEntries)
		{
			LPBYTE pEntry = m_lpData + sizeof(SZARRAY_HEADER);

			while (dwIndex)
			{
				LPSZARRAY_ENTRY_HEADER pEntryHdr = (LPSZARRAY_ENTRY_HEADER)pEntry;

				pEntry += pEntryHdr->dwSize + sizeof(SZARRAY_ENTRY_HEADER);
				dwIndex--;
			}
			
			return (LPSZARRAY_ENTRY_HEADER)pEntry;
		}
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////
LPSZARRAY_ENTRY_HEADER CSZArray::FindEntryHeaderByString(LPCSTR lpString)
{
	if (m_lpData)
	{
		LPSZARRAY_HEADER pHdr = (LPSZARRAY_HEADER)m_lpData;

		DWORD dwIndex = pHdr->dwNumEntries;

		LPBYTE pEntry = m_lpData + sizeof(SZARRAY_HEADER);

		while (dwIndex)
		{
			LPSZARRAY_ENTRY_HEADER pEntryHdr	= (LPSZARRAY_ENTRY_HEADER)pEntry;
			LPCSTR lpEntryString				= GetEntryStringByHeader(pEntryHdr);

			if (lpEntryString)
				if (!_stricmp(lpString, lpEntryString))
					return pEntryHdr;

			pEntry += pEntryHdr->dwSize + sizeof(SZARRAY_ENTRY_HEADER);
			dwIndex--;
		}
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////
LPCSTR CSZArray::GetEntryStringByHeader(LPSZARRAY_ENTRY_HEADER pEntryHdr)
{
	if (pEntryHdr)
		if (pEntryHdr->dwSizeData < pEntryHdr->dwSize)
			return (LPCSTR)((LPBYTE)pEntryHdr + sizeof(SZARRAY_ENTRY_HEADER) + pEntryHdr->dwSizeData);

	return NULL;
}
//////////////////////////////////////////////////////////////////////
LPCSTR CSZArray::GetEntryStringByIndex(DWORD dwIndex)
{
	return GetEntryStringByHeader(FindEntryHeaderByIndex(dwIndex));
}
//////////////////////////////////////////////////////////////////////
LPVOID CSZArray::GetEntryDataByHeader(LPSZARRAY_ENTRY_HEADER pEntryHdr)
{
	if (pEntryHdr)
		if (pEntryHdr->dwSizeData)
			return (LPBYTE)pEntryHdr + sizeof(SZARRAY_ENTRY_HEADER);

	return NULL;
}
//////////////////////////////////////////////////////////////////////
LPVOID CSZArray::GetEntryDataByIndex(DWORD dwIndex)
{
	return GetEntryDataByHeader(FindEntryHeaderByIndex(dwIndex));
}
//////////////////////////////////////////////////////////////////////
DWORD CSZArray::GetEntryDataSizeByHeader(LPSZARRAY_ENTRY_HEADER pEntryHdr)
{
	if (pEntryHdr)
		return pEntryHdr->dwSizeData;

	return NULL;
}
//////////////////////////////////////////////////////////////////////
DWORD CSZArray::GetEntryDataSizeByIndex(DWORD dwIndex)
{
	return GetEntryDataSizeByHeader(FindEntryHeaderByIndex(dwIndex));
}
//////////////////////////////////////////////////////////////////////
LPVOID CSZArray::GetData()
{
	return m_lpData;
}
//////////////////////////////////////////////////////////////////////
DWORD CSZArray::GetSize()
{
	LPSZARRAY_HEADER pHdr	= (LPSZARRAY_HEADER) m_lpData;

	if (pHdr)
		return pHdr->dwSize;

	return 0;
}
//////////////////////////////////////////////////////////////////////
DWORD CSZArray::GetNumEntries()
{
	LPSZARRAY_HEADER pHdr	= (LPSZARRAY_HEADER) m_lpData;

	if (pHdr)
		return pHdr->dwNumEntries;

	return 0;
}
//////////////////////////////////////////////////////////////////////
BOOL CSZArray::Copy(LPVOID lpData)
{
	Uninit();

	if (lpData)
	{
		LPSZARRAY_HEADER pHdr	= (LPSZARRAY_HEADER) lpData;
			//get ptr to array header

		DWORD dwSize = pHdr->dwSize;

		if (pHdr->dwSign != 'SZA0')
			return FALSE;

		if (!pHdr->dwNumEntries || !dwSize)
			return FALSE;

		m_lpData = new BYTE[dwSize];

		memcpy(m_lpData, lpData, dwSize);
	}

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CSZArray::Load(LPCSTR lpFilename)
{
	CFile file;

    if (!file.Open(lpFilename,CFile::modeRead|CFile::shareDenyWrite))					
       return FALSE;

	DWORD dwSize = (DWORD)file.GetLength();

	if (!dwSize)
		return FALSE;

	LPBYTE pBuf = new BYTE[dwSize];

	if (file.Read(pBuf, dwSize) != dwSize)
	{
		if (pBuf)
			delete [] pBuf;

		return FALSE;
	}

	BOOL bResult = TRUE;

	LPCOMPRESSED_DATA_HEADER pHdr = (LPCOMPRESSED_DATA_HEADER)pBuf;

	if (pHdr->dwSign == 'UDC0')
	{
		CDataCompressor decompressor;

		bResult = decompressor.Decompress(pBuf, dwSize);

		if (bResult)
			bResult = Copy(decompressor.GetData()); 
	}
	else
		bResult = Copy(pBuf);

	if (pBuf)
		delete [] pBuf;

	return bResult;
}
//////////////////////////////////////////////////////////////////////
BOOL CSZArray::Save(LPCSTR lpFilename, BOOL bCompress)
{
	DWORD dwSize = GetSize();

	if (!dwSize)
		return FALSE;

	CFile file;

    if (!file.Open(lpFilename, CFile::modeCreate|CFile::modeWrite))					
       return FALSE;

	if (bCompress)
	{
		CDataCompressor compressor;

		if (compressor.Compress(m_lpData, dwSize))
			file.Write(compressor.GetData(), compressor.GetSize()); 
		else
			file.Write(m_lpData, dwSize); 
	}
	else
		file.Write(m_lpData, dwSize); 

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CSZArray::AddEntryFromFile(LPCSTR lpsz, LPCSTR lpFilename)
{
	CFile file;

    if (!file.Open(lpFilename,CFile::modeRead|CFile::shareDenyWrite))					
       return FALSE;

	DWORD dwSize = (DWORD)file.GetLength();

	if (!dwSize)
		return FALSE;

	LPBYTE pBuf = new BYTE[dwSize];

	if (file.Read(pBuf, dwSize) != dwSize)
	{
		if (pBuf)
			delete [] pBuf;

		return FALSE;
	}

	 AddEntry(lpsz, pBuf, dwSize);

	delete [] pBuf;

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void CSZArray::SetReservedSize(DWORD dwReservedSize)
{
	m_dwReservedSize = dwReservedSize;
}
//////////////////////////////////////////////////////////////////////
DWORD CSZArray::GetReservedSize()
{
	return m_dwReservedSize;
}
//////////////////////////////////////////////////////////////////////
