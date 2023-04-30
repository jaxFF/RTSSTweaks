// DataCompressor.h: interface for the CDataCompressor class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#ifndef _DATACOMPRESSOR_H_INCLUDED_
#define _DATACOMPRESSOR_H_INCLUDED_
//////////////////////////////////////////////////////////////////////
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
//////////////////////////////////////////////////////////////////////
#define COMPRESSOR_SIGNATURE_LZW0								'LZW0'
//////////////////////////////////////////////////////////////////////
typedef struct COMPRESSED_DATA_HEADER
{
	DWORD dwSign;
	DWORD dwSizeOriginal;
	DWORD dwSizeCompressed;
	DWORD dwCRC32Original;
	DWORD dwCRC32Compressed;
	DWORD dwSignCompressor;
} COMPRESSED_DATA_HEADER, *LPCOMPRESSED_DATA_HEADER;
//////////////////////////////////////////////////////////////////////
#define BITS_MIN				9
#define BITS_MAX				12
#define CODE_CLEAR				((1 << (BITS_MIN - 1)))
#define CODE_EOF				((1 << (BITS_MIN - 1)) + 1)
#define CODE_FREE				((1 << (BITS_MIN - 1)) + 2)
#define CODE_MAX(bits)			((1 << (bits		)) - 1)
#define HASH_SIZE				5003		// 80% occupancy
											// must be prime for proper hashing!!!
#define HASH_SHIFT				4
//////////////////////////////////////////////////////////////////////
class CDataCompressor  
{
public:
	DWORD	GetSize();
	LPBYTE	GetData();
	void	Uninit();

	BOOL	Compress(LPBYTE lpData, DWORD dwSize, DWORD dwCompressor = COMPRESSOR_SIGNATURE_LZW0);
	BOOL	Decompress(LPBYTE lpData, DWORD dwSize);

	BOOL	LZW_Compress(LPBYTE lpData, DWORD dwSize);
	BOOL	LZW_Decompress(LPBYTE lpData, DWORD dwSize);

	CDataCompressor();
	virtual ~CDataCompressor();

protected:
	BOOL			IsInputEmpty();
	BOOL			IsStackEmpty();
	void			InitStack();
	BYTE			Pop();
	void			Push(BYTE data);
	void			PutByte(BYTE data);
	void			InitPrefixTable();
	void			InitSuffixTable();
	short int		GetInputBits();
	void			SetInputData(LPBYTE lpData, DWORD dwSize, DWORD dwOffset);
	void			PutBits(short int code);
	void			ClearHash(register long size);

	LPBYTE			m_lpData;
	DWORD			m_dwSize;
	DWORD			m_dwOffset;
	DWORD			m_dwSizeAllocated;

	DWORD			m_dwBitOffset;
	DWORD			m_dwBitCount;

	LPBYTE			m_lpInputData;
	DWORD			m_dwInputSize;
	DWORD			m_dwInputOffset;

	long			m_hash[HASH_SIZE];
	unsigned short	m_code[HASH_SIZE];

	long			m_prefix[1<<BITS_MAX];
	char			m_suffix[1<<BITS_MAX];
	char			m_stack [1<<BITS_MAX];
	char*			m_lpStack;

	long			m_codeFree;
	long			m_codeMax;
	BOOL			m_bClear;

	int				m_roffset;
};
//////////////////////////////////////////////////////////////////////
#endif
//////////////////////////////////////////////////////////////////////
