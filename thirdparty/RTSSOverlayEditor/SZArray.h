// SZArray.h: interface for the CSZArray class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#ifndef _SZARRAY_H_INCLUDED_
#define _SZARRAY_H_INCLUDED_
//////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//////////////////////////////////////////////////////////////////////
typedef struct SZARRAY_HEADER
{
	DWORD dwSign;
	DWORD dwSize;
	DWORD dwNumEntries;
} SZARRAY_HEADER, *LPSZARRAY_HEADER;
//////////////////////////////////////////////////////////////////////
typedef struct SZARRAY_ENTRY_HEADER
{
	DWORD dwSize;
	DWORD dwSizeData;
} SZARRAY_ENTRY_HEADER, *LPSZARRAY_ENTRY_HEADER;
//////////////////////////////////////////////////////////////////////
class CSZArray  
{
public:
	void	SetReservedSize(DWORD dwReservedSize);
	DWORD	GetReservedSize();

	BOOL	AddEntryFromFile(LPCSTR lpsz, LPCSTR lpFilename);
	BOOL	Save(LPCSTR lpFilename, BOOL bCompress = FALSE);
	BOOL	Load(LPCSTR lpFilename);
	BOOL	Copy(LPVOID lpData);
	DWORD	GetNumEntries();

	DWORD	GetSize();
	LPVOID	GetData();

	LPSZARRAY_ENTRY_HEADER	FindEntryHeaderByIndex(DWORD	dwIndex);
	LPSZARRAY_ENTRY_HEADER	FindEntryHeaderByString(LPCSTR lpString);

	LPVOID	GetEntryDataByIndex(DWORD dwIndex);
	DWORD	GetEntryDataSizeByIndex(DWORD dwIndex);
	LPCSTR	GetEntryStringByIndex(DWORD dwIndex);

	LPVOID	GetEntryDataByHeader(LPSZARRAY_ENTRY_HEADER pEntryHdr);
	DWORD	GetEntryDataSizeByHeader(LPSZARRAY_ENTRY_HEADER pEntryHdr);
	LPCSTR	GetEntryStringByHeader(LPSZARRAY_ENTRY_HEADER pEntryHdr);

	void	AddEntry(LPCSTR lpsz, LPVOID lpEntryData0 = NULL, DWORD dwDataSize0 = 0, LPVOID lpEntryData1 = NULL, DWORD dwDataSize1 = 0);
	void	Uninit();

	CSZArray();
	virtual ~CSZArray();

private:
	LPBYTE	m_lpData;
	DWORD	m_dwReservedSize;
};
//////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////
