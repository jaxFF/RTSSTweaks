// DataCompressor.cpp: implementation of the CDataCompressor class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "DataCompressor.h"
#include "CRC32.h"
//////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
//#define FIXED_BIT_COUNT
//////////////////////////////////////////////////////////////////////
#pragma warning (disable : 4244)
//////////////////////////////////////////////////////////////////////
CDataCompressor::CDataCompressor()
{
	m_lpData			= NULL;
	m_dwSize			= 0;
	m_dwOffset			= 0;

	m_lpInputData		= NULL;
	m_dwInputSize		= 0;
	m_dwInputOffset		= 0;

	m_lpStack			= NULL;

	m_dwSizeAllocated	= 0;
}
//////////////////////////////////////////////////////////////////////
CDataCompressor::~CDataCompressor()
{
	Uninit();
}
//////////////////////////////////////////////////////////////////////
BOOL CDataCompressor::Compress(LPBYTE lpData, DWORD dwSize, DWORD dwCompressor)
{
	switch (dwCompressor)
	{
	case COMPRESSOR_SIGNATURE_LZW0:
		return LZW_Compress(lpData, dwSize);
	}

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CDataCompressor::Decompress(LPBYTE lpData, DWORD /*dwSize*/)
{
	Uninit();

	LPCOMPRESSED_DATA_HEADER lpHeader = (LPCOMPRESSED_DATA_HEADER)lpData;

	if (lpHeader->dwSign != 'UDC0')
		return FALSE;

	CCRC32 crc;
	if (lpHeader->dwCRC32Compressed != crc.Calc(0, lpData + sizeof(COMPRESSED_DATA_HEADER), lpHeader->dwSizeCompressed))
		return FALSE;

	m_lpData			= new BYTE[lpHeader->dwSizeOriginal];
	m_dwSizeAllocated	= lpHeader->dwSizeOriginal;
	m_dwSize			= 0;

	switch (lpHeader->dwSignCompressor)
	{
	case COMPRESSOR_SIGNATURE_LZW0:
		if (!LZW_Decompress(lpData + sizeof(COMPRESSED_DATA_HEADER), lpHeader->dwSizeCompressed))
			return FALSE;
		break;
	}

	if (lpHeader->dwSizeOriginal != m_dwSize)
		return FALSE;

	if (lpHeader->dwCRC32Original != crc.Calc(0, m_lpData, m_dwSize))
		return FALSE;

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CDataCompressor::LZW_Compress(LPBYTE lpData, DWORD dwSize)
{
	Uninit();

	m_dwSize							= 0;
	m_dwSizeAllocated					= sizeof(COMPRESSED_DATA_HEADER) + 4 * dwSize / 3;
		//worst scenario assumption: header + 12 bits per each byte (4/3 size increment)
	m_lpData							= new BYTE[m_dwSizeAllocated];

	LPCOMPRESSED_DATA_HEADER lpHeader	= (LPCOMPRESSED_DATA_HEADER)m_lpData;

	CCRC32 crc;

	lpHeader->dwSign					= 'UDC0';
	lpHeader->dwSizeOriginal			= dwSize;
	lpHeader->dwSignCompressor			= COMPRESSOR_SIGNATURE_LZW0;
	lpHeader->dwCRC32Original			= crc.Calc(0, lpData, dwSize);

		//init compressed data header

	m_dwSize							= sizeof(COMPRESSED_DATA_HEADER);
		//init compressed data size

	m_dwBitCount						= BITS_MIN;
	m_dwBitOffset						= 0;
	m_codeMax							= CODE_MAX(m_dwBitCount);

	register long chr;
	register long str;
	register long code;
	register long offset;
	register long index;
	register long probe;

	m_bClear = FALSE;

	m_codeFree	= CODE_FREE;	
		//init first free entry index



	DWORD dwOffset = 0;
		//init input character offset

	str = lpData[dwOffset++];
		//init string with the first input character

	ClearHash(HASH_SIZE); 
		//clear hash table
	PutBits(CODE_CLEAR);
		//write clear code to output stream

	while (dwOffset<dwSize)
	{    
		chr		= lpData[dwOffset++];
			//get input character

		code	= (chr<<BITS_MAX	) + str;
		index	= (chr<<HASH_SHIFT	) ^ str;	//xor hashing

		if (m_hash[index] == code) 
			//found match in hash table
		{
			str = m_code[index];
				//string = existing code from table
			continue;
		}
		else
			if (m_hash[index] != -1)	
				//collision detected!
				//hash table slot is not empty,	try to find an empty slot or a match in hash table using fixed offset probe
			{
				offset = HASH_SIZE - index;	

				if (!index)	
					offset = 1;

				do
				{
					index = index - offset;

					if (index < 0)
						index = index + HASH_SIZE;

					probe = m_hash[index];
				}
				while ((probe != -1) && (probe != code));
					//keep probing until either an empty slot or a match is found

				if (probe == code)
					//found match in hash table
				{
					str = m_code[index];
						//string = existing code from table
					continue;
				}
			}

		PutBits(str);
			//write sting code to output stream

		str = chr;
			//string = input character

		if (m_codeFree < (1 << BITS_MAX)) 
		{  
			m_code[index] = m_codeFree++;
			m_hash[index] = code;
		} 
		else 
		{
			ClearHash(HASH_SIZE);
				//clear hash table
			PutBits(CODE_CLEAR);
				//write clear code to output stream

			m_codeFree	= CODE_FREE;
				//reset first free entry index
			m_bClear	= TRUE;
				//set clear flag
		}
	}
	 //write last code and EOF to output stream

	PutBits(str);
	PutBits(CODE_EOF);

	lpHeader->dwSizeCompressed	= m_dwSize - sizeof(COMPRESSED_DATA_HEADER);
	lpHeader->dwCRC32Compressed	= crc.Calc(0, m_lpData + sizeof(COMPRESSED_DATA_HEADER), lpHeader->dwSizeCompressed);


	return TRUE;
}
//////////////////////////////////////////////////////////////////////
BOOL CDataCompressor::LZW_Decompress(LPBYTE lpData, DWORD dwSize)
{
	m_dwBitCount						= BITS_MIN;
	m_dwBitOffset						= 0;
	m_codeMax							= CODE_MAX(m_dwBitCount);

	m_roffset = 0;

	SetInputData(lpData, dwSize, 0);
		//init input data pointer, size and offset variables

	m_bClear = FALSE;

	InitStack();
	InitSuffixTable();

	short int	code		= 0;
	short int	inputcode	= 0;
	short int	prevcode	= 0;
	char		firstchar	= 0;

	m_codeFree	= CODE_FREE - 1;

	while (!IsInputEmpty())
	{
		code = GetInputBits();
			//get input code

		if (code == -1)
			//stop decompression if input stream is empty
			break;

		if (code == CODE_EOF)
			//process EOF code
			break;

		if (code == CODE_CLEAR)
			//process CLEAR code
		{
			InitPrefixTable();
				//init prefix table

			m_bClear	= TRUE;
				//set clear flag
			m_codeFree	= CODE_FREE - 1;
				//reset free code

			code = GetInputBits();

			if (code == -1)
				//stop decompression if input stream is empty
				break;

			if (code == CODE_EOF)
				break;

			prevcode		= code;
			firstchar		= code;
		}

		inputcode = code;
			//save input code

		if (code >= m_codeFree) 
			// special case for KwKwK string
		{
			Push(firstchar);
			code = prevcode;
		}

		// push string characters
		while (code >= 256) 
		{
			Push(m_suffix[code]);
			code = m_prefix[code];
		}

		firstchar = m_suffix[code];
			//save first char
		Push(firstchar);
			//and push it into stack

		// pop string characters in reversed order and put them to output stream

		do 
			PutByte(Pop());
		while (!IsStackEmpty());

		// generate the new table entry 

		if (m_codeFree < (1<<BITS_MAX)) 
		{
			m_prefix[m_codeFree] = prevcode;
			m_suffix[m_codeFree] = firstchar;
			m_codeFree++;
		}
		else
			return FALSE;

		// remember previous code as input code
		prevcode = inputcode;
	}

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
void CDataCompressor::Uninit()
{
	if (m_lpData)
		delete [] m_lpData;

	m_lpData			= NULL;
	m_dwSize			= 0;
	m_dwSizeAllocated	= 0;
}
//////////////////////////////////////////////////////////////////////
LPBYTE CDataCompressor::GetData()
{
	return m_lpData;
}
//////////////////////////////////////////////////////////////////////
DWORD CDataCompressor::GetSize()
{
	return m_dwSize;
}
//////////////////////////////////////////////////////////////////////
void CDataCompressor::ClearHash(long size)
{
	register long *ptr	= m_hash + size;
	register long index	= size - 16;
	register long value	= -1;

	do 
	{
		*(ptr - 16)	= value;
		*(ptr - 15)	= value;
		*(ptr - 14)	= value;
		*(ptr - 13)	= value;
		*(ptr - 12)	= value;
		*(ptr - 11)	= value;
		*(ptr - 10)	= value;
		*(ptr - 9)	= value;
		*(ptr - 8)	= value;
		*(ptr - 7)	= value;
		*(ptr - 6)	= value;
		*(ptr - 5)	= value;
		*(ptr - 4)	= value;
		*(ptr - 3)	= value;
		*(ptr - 2)	= value;
		*(ptr - 1)	= value;
		
		ptr		= ptr	- 16;
		index	= index - 16;
	} 
	while (index >=0);

	for (index = index + 16; index>0; --index)
		*--ptr=value;
}
//////////////////////////////////////////////////////////////////////
void CDataCompressor::PutBits(short int code)
{
	if (m_bClear) 
	{
		m_dwBitCount	= BITS_MIN;
		m_codeMax		= CODE_MAX(m_dwBitCount);
		m_bClear		= FALSE;
	} 

	static const BYTE mask[] = { 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };

	BOOL bEOF = (code == CODE_EOF);

	if (m_dwBitOffset)
	{
		m_lpData[m_dwSize]	&= mask[m_dwBitOffset];
		m_lpData[m_dwSize]	|= code<<m_dwBitOffset;
		code = code>>(8 - m_dwBitOffset);
	}
	else
	{
		m_lpData[m_dwSize] = code;
		code = code>>8;
	}

#ifdef FIXED_BIT_COUNT
	m_dwBitOffset += 12;
#else
	m_dwBitOffset += m_dwBitCount;
#endif

	while (m_dwBitOffset >= 8)
	{	
		m_dwSize++;
		m_dwBitOffset -= 8;

		m_lpData[m_dwSize] = (BYTE)code;

		code = code>>8;
	}

	if (m_codeFree > m_codeMax) 
	{
		m_dwBitCount++;

		if (m_dwBitCount == BITS_MAX)
			m_codeMax = 1 << BITS_MAX; 
		else
			m_codeMax = CODE_MAX(m_dwBitCount);
	}

	if (bEOF)
	{
		if (m_dwBitOffset)
			m_dwSize++;
	}
}
//////////////////////////////////////////////////////////////////////
void CDataCompressor::SetInputData(LPBYTE lpData, DWORD dwSize, DWORD dwOffset)
{
	m_lpInputData		= lpData;
	m_dwInputSize		= dwSize;
	m_dwInputOffset		= dwOffset;
}
//////////////////////////////////////////////////////////////////////
short int CDataCompressor::GetInputBits()
{
	if ((m_codeFree > m_codeMax) || m_bClear) 
	{
		if (m_bClear) 
		{
			m_dwBitCount	= BITS_MIN;
			m_codeMax		= CODE_MAX(m_dwBitCount);
			m_bClear		= FALSE;
		} 
		else 
		{
			m_dwBitCount++;

			if (m_dwBitCount == BITS_MAX)
				m_codeMax = 1 << BITS_MAX; 
			else
				m_codeMax = CODE_MAX(m_dwBitCount);
		}
	}


	short int bits;

	static const BYTE mask[] = { 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F };

	bits = m_lpInputData[m_dwInputOffset]>>m_dwBitOffset;
	int offset;

	if (m_dwBitOffset)
		offset = 8 - m_dwBitOffset;
	else
		offset = 8;

#ifdef FIXED_BIT_COUNT
	m_dwBitOffset += 12;
#else
	m_dwBitOffset += m_dwBitCount;
#endif

	while (m_dwBitOffset >= 8)
	{
		m_dwInputOffset++;
		m_dwBitOffset -= 8;

		BYTE buf = m_lpInputData[m_dwInputOffset];

		if (m_dwBitOffset < 8)
			buf = buf & mask[ m_dwBitOffset];

		bits = bits | (buf<<offset);

		offset += 8;
	}


	return bits;
}
//////////////////////////////////////////////////////////////////////
void CDataCompressor::InitSuffixTable()
{
	for (int index=0; index<256; index++) 
		m_suffix[index] = index;
}
//////////////////////////////////////////////////////////////////////
void CDataCompressor::InitPrefixTable()
{
	for (int index=0; index<256; index++) 
		m_prefix[index] = 0;
}
//////////////////////////////////////////////////////////////////////
void CDataCompressor::PutByte(BYTE data)
{
	m_lpData[m_dwSize++] = data;
}
//////////////////////////////////////////////////////////////////////
void CDataCompressor::Push(BYTE data)
{
	*m_lpStack++ = data;
}
//////////////////////////////////////////////////////////////////////
BYTE CDataCompressor::Pop()
{
	return *--m_lpStack;
}
//////////////////////////////////////////////////////////////////////
void CDataCompressor::InitStack()
{
	m_lpStack = m_stack;
}
//////////////////////////////////////////////////////////////////////
BOOL CDataCompressor::IsStackEmpty()
{
	return m_lpStack == m_stack;
}
//////////////////////////////////////////////////////////////////////
BOOL CDataCompressor::IsInputEmpty()
{
	return !m_lpInputData || (m_dwInputOffset >= m_dwInputSize);
}
//////////////////////////////////////////////////////////////////////
#pragma warning (default : 4244)
//////////////////////////////////////////////////////////////////////
