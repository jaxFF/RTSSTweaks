// PtrListLite.cpp: implementation of the CPtrListLite class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "PtrListLite.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CPtrListLite::CPtrListLite()
{
	m_pHead	= NULL;
	m_pTail = NULL;
}
//////////////////////////////////////////////////////////////////////
CPtrListLite::~CPtrListLite()
{
	RemoveAll();
}
//////////////////////////////////////////////////////////////////////
LPPTR_LIST_NODE CPtrListLite::GetHead()
{
	return m_pHead;
}
//////////////////////////////////////////////////////////////////////
LPPTR_LIST_NODE CPtrListLite::GetTail()
{
	return m_pTail;
}
//////////////////////////////////////////////////////////////////////
LPPTR_LIST_NODE CPtrListLite::AddTail(LPVOID pData)
{
	LPPTR_LIST_NODE pNode = new PTR_LIST_NODE;

	pNode->pNext = NULL;
	pNode->pPrev = m_pTail;
	pNode->pData = pData;
	
	if (m_pTail)
		m_pTail->pNext = pNode;

	m_pTail		= pNode;

	if (!m_pHead)
		m_pHead = pNode;

	return pNode;
}
//////////////////////////////////////////////////////////////////////
LPPTR_LIST_NODE CPtrListLite::AddHead(LPVOID pData)
{
	LPPTR_LIST_NODE pNode = new PTR_LIST_NODE;

	pNode->pNext = m_pHead;
	pNode->pPrev = NULL;
	pNode->pData = pData;
	
	if (m_pHead)
		m_pHead->pPrev = pNode;

	m_pHead		= pNode;

	if (!m_pTail)
		m_pTail = pNode;

	return pNode;
}
//////////////////////////////////////////////////////////////////////
LPVOID CPtrListLite::GetNext(LPPTR_LIST_NODE &pNode)
{
	if (pNode)
	{
		LPVOID pData = pNode->pData;

		pNode = pNode->pNext;

		return pData;
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////
LPVOID CPtrListLite::GetPrev(LPPTR_LIST_NODE &pNode)
{
	if (pNode)
	{
		LPVOID pData = pNode->pData;

		pNode = pNode->pPrev;

		return pData;
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////
void CPtrListLite::RemoveAll()
{
	LPPTR_LIST_NODE pNode = m_pHead;

	while (pNode)
	{
		LPVOID pUnusedNode = pNode;

		pNode = pNode->pNext;

		delete pUnusedNode;
	}

	m_pHead = NULL;
	m_pTail = NULL;
}
//////////////////////////////////////////////////////////////////////
void CPtrListLite::RemoveAt(LPPTR_LIST_NODE pNode)
{
	if (m_pTail == pNode)
		m_pTail = pNode->pPrev;

	if (m_pHead == pNode)
		m_pHead = pNode->pNext;

	if (pNode->pPrev)
		pNode->pPrev->pNext = pNode->pNext;
	if (pNode->pNext)
		pNode->pNext->pPrev = pNode->pPrev;

	delete pNode;
}
//////////////////////////////////////////////////////////////////////
LPPTR_LIST_NODE CPtrListLite::Find(LPVOID pData)
{
	LPPTR_LIST_NODE pNode = m_pHead;

	while (pNode)
	{
		if (pNode->pData == pData)
			return pNode;

		pNode = pNode->pNext;
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////
LPPTR_LIST_NODE CPtrListLite::Find(DWORD dwIndex)
{
	DWORD dwIndexCur = 0;

	LPPTR_LIST_NODE pNode = m_pHead;

	while (pNode && (dwIndexCur <= dwIndex))
	{
		if (dwIndexCur == dwIndex)
			return pNode;
		
		dwIndexCur++;

		pNode = pNode->pNext;
	}

	return NULL;
}
//////////////////////////////////////////////////////////////////////
DWORD CPtrListLite::GetCount()
{
	DWORD dwCount = 0;

	LPPTR_LIST_NODE pNode = m_pHead;

	while (pNode)
	{
		dwCount++;

		pNode = pNode->pNext;
	}

	return dwCount;
}
//////////////////////////////////////////////////////////////////////
LPVOID CPtrListLite::GetAt(LPPTR_LIST_NODE pNode)
{
	if (pNode)
		return pNode->pData;

	return FALSE;
}
//////////////////////////////////////////////////////////////////////
void CPtrListLite::SwapWithPrev(LPPTR_LIST_NODE pNode)
{
	LPPTR_LIST_NODE pPrev = pNode->pPrev;

	if (pPrev)
	{
		LPVOID pTemp = pNode->pData;
		pNode->pData = pPrev->pData;
		pPrev->pData = pTemp;
	}
}
//////////////////////////////////////////////////////////////////////
void CPtrListLite::SwapWithNext(LPPTR_LIST_NODE pNode)
{
	LPPTR_LIST_NODE pNext = pNode->pNext;

	if (pNext)
	{
		LPVOID pTemp = pNode->pData;
		pNode->pData = pNext->pData;
		pNext->pData = pTemp;
	}
}
//////////////////////////////////////////////////////////////////////
void CPtrListLite::Sort(BOOL bAscending, COMPARE_NODES_PROC pCompareProc)
{
	if (bAscending)
	{
		LPPTR_LIST_NODE pNode0 = GetTail();

		while (pNode0)
		{
			LPPTR_LIST_NODE pNode1 = GetHead();

			while (pNode1)
			{
				if (pNode1 && pNode1->pNext && (pCompareProc(pNode1, pNode1->pNext) > 0))
					SwapWithNext(pNode1);

				pNode1 = pNode1->pNext;

				if (pNode0 == pNode1)
					break;
			}

			pNode0 = pNode0->pPrev;
		}
	}
	else
	{
		LPPTR_LIST_NODE pNode0 = GetHead();

		while (pNode0)
		{
			LPPTR_LIST_NODE pNode1 = GetTail();

			while (pNode1)
			{
				if (pNode1 && pNode1->pPrev && (pCompareProc(pNode1, pNode1->pPrev) > 0))
					SwapWithPrev(pNode1);

				pNode1 = pNode1->pPrev;

				if (pNode0 == pNode1)
					break;
			}

			pNode0 = pNode0->pNext;
		}
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CPtrListLite::IsEmpty()
{
	return (m_pHead == NULL);
}
//////////////////////////////////////////////////////////////////////
