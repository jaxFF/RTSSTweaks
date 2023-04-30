// AIDA64Parser.h: interface for the CAIDA64Parser class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#pragma once
//////////////////////////////////////////////////////////////////////
class CAIDA64Parser
{
public:
	BOOL ParseXML(LPCSTR lpText, DWORD dwContext = 0);
		//parses AIDA64 XML-styled shared memory
	void ParseXMLElement(LPCSTR lpTag, LPCSTR lpVal, int nLevel, DWORD dwContext);
		//called by parser for each XML element of AIDA64 XML-styled shared memory

	virtual void ParseSensor(LPCSTR lpType, LPCSTR lpID, LPCSTR lpLabel, LPCSTR lpValue, DWORD dwContext) = 0;
		///pure virtual function, called by parser for each sensor of AIDA64 XML-styled shared memory

	CAIDA64Parser();
	virtual ~CAIDA64Parser();

protected:
	CString	m_strID;
	CString	m_strLabel;
	CString m_strValue;
};
//////////////////////////////////////////////////////////////////////
