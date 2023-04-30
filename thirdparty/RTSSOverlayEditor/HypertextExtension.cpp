// HypertextExtension.cpp: implementation of the CHypertextExtension class.
//
// created by Unwinder
//////////////////////////////////////////////////////////////////////
#include "StdAfx.h"
#include "HypertextExtension.h"
#include "RTSSSharedMemoryInterface.h"
#include "Overlay.h"
#include "OverlayDataSourceInternal.h"

#include <float.h>
//////////////////////////////////////////////////////////////////////
CHypertextExtension::CHypertextExtension()
{
	m_nPos			= -1;
	m_nObj			= OBJ_UNKNOWN;
	m_nObjFrom		= -1;
	m_nObjTo		= -1;
	m_nObjParams	= 0;
} 
//////////////////////////////////////////////////////////////////////
CHypertextExtension::~CHypertextExtension()
{
}
//////////////////////////////////////////////////////////////////////
CString	CHypertextExtension::TranslateExtensionString(COverlay* lpOverlay, LPCSTR lpString, BOOL bOptimize, BOOL bEmbed, LPBYTE lpEmbeddedObjectBuffer, DWORD dwEmbeddedObjectBufferSize, DWORD& dwEmbeddedObjectOffset, DWORD dwColor, int nSize, BOOL bContextHighlighting)
{
	CString strResult;

	DWORD	dwGraphWidth						= DEFAULT_GRAPH_WIDTH;
	DWORD	dwGraphHeight						= DEFAULT_GRAPH_HEIGHT;
	DWORD	dwGraphMargin						= DEFAULT_GRAPH_MARGIN;
	float	fltGraphMin							= DEFAULT_GRAPH_MIN;
	float	fltGraphMax							= DEFAULT_GRAPH_MAX;
	DWORD	dwGraphFlags						= DEFAULT_GRAPH_FLAGS;

	DWORD	dwAnimatedImageWidth				= DEFAULT_ANIMATED_IMAGE_WIDTH;
	DWORD	dwAnimatedImageHeight				= DEFAULT_ANIMATED_IMAGE_HEIGHT;
	float	fltAnimatedImageMin					= DEFAULT_ANIMATED_IMAGE_MIN;
	float	fltAnimatedImageMax					= DEFAULT_ANIMATED_IMAGE_MAX;
	DWORD	dwAnimatedImageSpriteX				= DEFAULT_ANIMATED_IMAGE_SPRITE_X;
	DWORD	dwAnimatedImageSpriteY				= DEFAULT_ANIMATED_IMAGE_SPRITE_Y;
	DWORD	dwAnimatedImageSpriteWidth			= DEFAULT_ANIMATED_IMAGE_SPRITE_WIDTH;
	DWORD	dwAnimatedImageSpriteHeight			= DEFAULT_ANIMATED_IMAGE_SPRITE_HEIGHT;
	DWORD	dwAnimatedImageSpritesNum			= DEFAULT_ANIMATED_IMAGE_SPRITES_NUM;
	DWORD	dwAnimatedImageSpritesPerLine		= DEFAULT_ANIMATED_IMAGE_SPRITES_PER_LINE;
	float	fltAnimatedImageBias				= DEFAULT_ANIMATED_IMAGE_BIAS;
	int		nAnimatedImageRotationAngleMin		= DEFAULT_ANIMATED_IMAGE_ROTATION_ANGLE;
	int		nAnimatedImageRotationAngleMax		= DEFAULT_ANIMATED_IMAGE_ROTATION_ANGLE;

	DWORD	dwImageWidth						= DEFAULT_ANIMATED_IMAGE_WIDTH;
	DWORD	dwImageHeight						= DEFAULT_ANIMATED_IMAGE_HEIGHT;
	DWORD	dwImageSpriteX						= DEFAULT_ANIMATED_IMAGE_SPRITE_X;
	DWORD	dwImageSpriteY						= DEFAULT_ANIMATED_IMAGE_SPRITE_Y;
	DWORD	dwImageSpriteWidth					= DEFAULT_ANIMATED_IMAGE_SPRITE_WIDTH;
	DWORD	dwImageSpriteHeight					= DEFAULT_ANIMATED_IMAGE_SPRITE_HEIGHT;
	int		nImageRotationAngle					= DEFAULT_ANIMATED_IMAGE_ROTATION_ANGLE;

	LPCSTR	lpStart		= lpString;

	if (bContextHighlighting)
	{
		m_nObj				= OBJ_UNKNOWN;
		m_nObjFrom			= -1;
		m_nObjTo			= -1;
		m_nObjParams		= 0;
	}

	while (*lpString)
	{
		BOOL bAppendChar = FALSE;

		int nObjFrom	= -1;
		int nObjTo		= -1;
		int nObjParams	= 0;

		if (*lpString == '%')
		{
			COverlayMacroList* lpMacroList = lpOverlay->GetMacroList();

			BOOL bMacro = FALSE;

			POSITION pos = lpMacroList->GetHeadPosition();

			while (pos)
			{
				CString strSrc = lpMacroList->GetNext(pos);
				CString strDst = lpMacroList->GetNext(pos);

				int nLen = strlen(strSrc);

				if (!strncmp(lpString, strSrc, nLen))
				{
					nObjFrom	= lpString - lpStart;
					nObjTo		= nObjFrom + nLen;

					if (bContextHighlighting)
					{
						if ((m_nPos > nObjFrom	) &&
							(m_nPos < nObjTo	))
						{
							m_nObj		= OBJ_MACRO;
							m_nObjFrom	= nObjFrom;
							m_nObjTo	= nObjTo;
						}
					}

					strResult	+= strDst;
					lpString	+= nLen;

					bMacro = TRUE;

					break;
				}
			}

			if (!bMacro)
				bAppendChar = TRUE;
		}
		else
		if (*lpString == '<')
			//tags
		{
			//native hypertext tags

			if (!strncmp(lpString, GRAPH_MIN_TAG, GRAPH_MIN_TAG_LEN))
			{
				nObjFrom	= lpString - lpStart;
				nObjTo		= nObjFrom + GRAPH_MIN_TAG_LEN;

				if (bContextHighlighting)
				{
					if ((m_nPos > nObjFrom	) &&
						(m_nPos < nObjTo	))
					{
						m_nObj			= OBJ_TEXT_TAG;
						m_nObjFrom		= nObjFrom;
						m_nObjTo		= nObjTo;
						m_nObjParams	= 0;
					}
				}

				bAppendChar = TRUE;
			}
			else
			if (!strncmp(lpString, GRAPH_MAX_TAG, GRAPH_MAX_TAG_LEN))
			{
				nObjFrom	= lpString - lpStart;
				nObjTo		= nObjFrom + GRAPH_MAX_TAG_LEN;

				if (bContextHighlighting)
				{
					if ((m_nPos > nObjFrom	) &&
						(m_nPos < nObjTo	))
					{
						m_nObj			= OBJ_TEXT_TAG;
						m_nObjFrom		= nObjFrom;
						m_nObjTo		= nObjTo;
						m_nObjParams	= 0;
					}
				}

				bAppendChar = TRUE;
			}
			else
			if (!strncmp(lpString, FRAMERATE_MIN_TAG, FRAMERATE_MIN_TAG_LEN))
			{
				nObjFrom	= lpString - lpStart;
				nObjTo		= nObjFrom + FRAMERATE_MIN_TAG_LEN;

				if (bContextHighlighting)
				{
					if ((m_nPos > nObjFrom	) &&
						(m_nPos < nObjTo	))
					{
						m_nObj			= OBJ_TEXT_TAG;
						m_nObjFrom		= nObjFrom;
						m_nObjTo		= nObjTo;
						m_nObjParams	= 0;
					}
				}

				bAppendChar = TRUE;
			}
			else
			if (!strncmp(lpString, FRAMERATE_AVG_TAG, FRAMERATE_AVG_TAG_LEN))
			{
				nObjFrom	= lpString - lpStart;
				nObjTo		= nObjFrom + FRAMERATE_AVG_TAG_LEN;

				if (bContextHighlighting)
				{
					if ((m_nPos > nObjFrom	) &&
						(m_nPos < nObjTo	))
					{
						m_nObj			= OBJ_TEXT_TAG;
						m_nObjFrom		= nObjFrom;
						m_nObjTo		= nObjTo;
						m_nObjParams	= 0;
					}
				}

				bAppendChar = TRUE;
			}
			else
			if (!strncmp(lpString, FRAMERATE_MAX_TAG, FRAMERATE_MAX_TAG_LEN))
			{
				nObjFrom	= lpString - lpStart;
				nObjTo		= nObjFrom + FRAMERATE_MAX_TAG_LEN;

				if (bContextHighlighting)
				{
					if ((m_nPos > nObjFrom	) &&
						(m_nPos < nObjTo	))
					{
						m_nObj			= OBJ_TEXT_TAG;
						m_nObjFrom		= nObjFrom;
						m_nObjTo		= nObjTo;
						m_nObjParams	= 0;
					}
				}

				bAppendChar = TRUE;
			}
			else
			if (!strncmp(lpString, FRAMERATE_10L_TAG, FRAMERATE_10L_TAG_LEN))
			{
				nObjFrom	= lpString - lpStart;
				nObjTo		= nObjFrom + FRAMERATE_10L_TAG_LEN;

				if (bContextHighlighting)
				{
					if ((m_nPos > nObjFrom	) &&
						(m_nPos < nObjTo	))
					{
						m_nObj			= OBJ_TEXT_TAG;
						m_nObjFrom		= nObjFrom;
						m_nObjTo		= nObjTo;
						m_nObjParams	= 0;
					}
				}

				bAppendChar = TRUE;
			}
			else
			if (!strncmp(lpString, FRAMERATE_01L_TAG, FRAMERATE_01L_TAG_LEN))
			{
				nObjFrom	= lpString - lpStart;
				nObjTo		= nObjFrom + FRAMERATE_01L_TAG_LEN;

				if (bContextHighlighting)
				{
					if ((m_nPos > nObjFrom	) &&
						(m_nPos < nObjTo	))
					{
						m_nObj			= OBJ_TEXT_TAG;
						m_nObjFrom		= nObjFrom;
						m_nObjTo		= nObjTo;
						m_nObjParams	= 0;
					}
				}

				bAppendChar = TRUE;
			}
			else
			if (!strncmp(lpString, FRAMERATE_TAG, FRAMERATE_TAG_LEN))
			{
				nObjFrom	= lpString - lpStart;
				nObjTo		= nObjFrom + FRAMERATE_TAG_LEN;

				if (bContextHighlighting)
				{
					if ((m_nPos > nObjFrom	) &&
						(m_nPos < nObjTo	))
					{
						m_nObj			= OBJ_TEXT_TAG;
						m_nObjFrom		= nObjFrom;
						m_nObjTo		= nObjTo;
						m_nObjParams	= 0;
					}
				}

				bAppendChar = TRUE;
			}
			else
			if (!strncmp(lpString, FRAME_COUNT_TAG, FRAME_COUNT_TAG_LEN))
			{
				nObjFrom	= lpString - lpStart;
				nObjTo		= nObjFrom + FRAME_COUNT_TAG_LEN;

				if (bContextHighlighting)
				{
					if ((m_nPos > nObjFrom	) &&
						(m_nPos < nObjTo	))
					{
						m_nObj			= OBJ_TEXT_TAG;
						m_nObjFrom		= nObjFrom;
						m_nObjTo		= nObjTo;
						m_nObjParams	= 0;
					}
				}

				bAppendChar = TRUE;
			}
			else
			if (!strncmp(lpString, FRAMETIME_TAG, FRAMETIME_TAG_LEN))
			{
				nObjFrom	= lpString - lpStart;
				nObjTo		= nObjFrom + FRAMETIME_TAG_LEN;

				if (bContextHighlighting)
				{
					if ((m_nPos > nObjFrom	) &&
						(m_nPos < nObjTo	))
					{
						m_nObj			= OBJ_TEXT_TAG;
						m_nObjFrom		= nObjFrom;
						m_nObjTo		= nObjTo;
						m_nObjParams	= 0;
					}
				}

				bAppendChar = TRUE;
			}
			else
			if (!strncmp(lpString, APP_TAG, APP_TAG_LEN))
			{
				nObjFrom	= lpString - lpStart;
				nObjTo		= nObjFrom + APP_TAG_LEN;

				if (bContextHighlighting)
				{
					if ((m_nPos > nObjFrom	) &&
						(m_nPos < nObjTo	))
					{
						m_nObj			= OBJ_TEXT_TAG;
						m_nObjFrom		= nObjFrom;
						m_nObjTo		= nObjTo;
						m_nObjParams	= 0;
					}
				}

				bAppendChar = TRUE;
			}
			else
			if (!strncmp(lpString, API_TAG, API_TAG_LEN))
			{
				nObjFrom	= lpString - lpStart;
				nObjTo		= nObjFrom + API_TAG_LEN;

				if (bContextHighlighting)
				{
					if ((m_nPos > nObjFrom	) &&
						(m_nPos < nObjTo	))
					{
						m_nObj			= OBJ_TEXT_TAG;
						m_nObjFrom		= nObjFrom;
						m_nObjTo		= nObjTo;
						m_nObjParams	= 0;
					}
				}

				bAppendChar = TRUE;
			}
			else
			if (!strncmp(lpString, EXE_TAG, EXE_TAG_LEN))
			{
				nObjFrom	= lpString - lpStart;
				nObjTo		= nObjFrom + EXE_TAG_LEN;

				if (bContextHighlighting)
				{
					if ((m_nPos > nObjFrom	) &&
						(m_nPos < nObjTo	))
					{
						m_nObj			= OBJ_TEXT_TAG;
						m_nObjFrom		= nObjFrom;
						m_nObjTo		= nObjTo;
						m_nObjParams	= 0;
					}
				}

				bAppendChar = TRUE;
			}
			else
			if (!strncmp(lpString, RESOLUTION_TAG, RESOLUTION_TAG_LEN))
			{
				nObjFrom	= lpString - lpStart;
				nObjTo		= nObjFrom + RESOLUTION_TAG_LEN;

				if (bContextHighlighting)
				{
					if ((m_nPos > nObjFrom	) &&
						(m_nPos < nObjTo	))
					{
						m_nObj			= OBJ_TEXT_TAG;
						m_nObjFrom		= nObjFrom;
						m_nObjTo		= nObjTo;
						m_nObjParams	= 0;
					}
				}

				bAppendChar = TRUE;
			}
			else
			if (!strncmp(lpString, ARCHITECTURE_TAG, ARCHITECTURE_TAG_LEN))
			{
				nObjFrom	= lpString - lpStart;
				nObjTo		= nObjFrom + ARCHITECTURE_TAG_LEN;

				if (bContextHighlighting)
				{
					if ((m_nPos > nObjFrom	) &&
						(m_nPos < nObjTo	))
					{
						m_nObj			= OBJ_TEXT_TAG;
						m_nObjFrom		= nObjFrom;
						m_nObjTo		= nObjTo;
						m_nObjParams	= 0;
					}
				}

				bAppendChar = TRUE;
			}
			else
			if (!strncmp(lpString, TIME_TAG, TIME_TAG_LEN))
			{
				CString strTimeFormat;

				LPCSTR lpNext = ScanTimeTag(lpString + TIME_TAG_LEN, strTimeFormat);

				if (lpNext && (*lpNext == '>'))
				{
					nObjFrom	= lpString - lpStart;
					nObjTo		= lpNext   - lpStart + 1;

					if (bContextHighlighting)
					{
						if ((m_nPos > nObjFrom	) &&
							(m_nPos < nObjTo	))
						{
							m_nObj			= OBJ_TEXT_TAG;
							m_nObjFrom		= nObjFrom;
							m_nObjTo		= nObjTo;
							m_nObjParams	= ValidateTimeFormat(strTimeFormat) ? 1 : -1;
						}
					}
				}

				bAppendChar = TRUE;
			}
			else
			if (!strncmp(lpString, BENCHMARK_TIME_TAG, BENCHMARK_TIME_TAG_LEN))
			{
				nObjFrom	= lpString - lpStart;
				nObjTo		= nObjFrom + BENCHMARK_TIME_TAG_LEN;

				if (bContextHighlighting)
				{
					if ((m_nPos > nObjFrom	) &&
						(m_nPos < nObjTo	))
					{
						m_nObj			= OBJ_TEXT_TAG;
						m_nObjFrom		= nObjFrom;
						m_nObjTo		= nObjTo;
						m_nObjParams	= 0;
					}
				}

				bAppendChar = TRUE;
			}
			else
			
				//hypertext extenstion tags

			if (!strncmp(lpString, TEXT_TABLE_TAG, TEXT_TABLE_TAG_LEN))
			{
				nObjFrom = lpString - lpStart;

				CString strTable;

				LPCSTR lpNext = ScanTextTableTag(lpString + TEXT_TABLE_TAG_LEN, strTable);

				if (lpNext && (*lpNext == '>'))
				{
					nObjTo = lpNext - lpStart + 1;

					if (bContextHighlighting)
					{
						if ((m_nPos > nObjFrom	) &&
							(m_nPos < nObjTo	))
						{
							m_nObj			= OBJ_TEXT_TABLE;
							m_nObjFrom		= nObjFrom;
							m_nObjTo		= nObjTo;
							m_nObjParams	= 1;
						}
					}

					CTextTable* lpTable = lpOverlay->FindTextTable(strTable);

					if (lpTable)
						strResult += TranslateExtensionString(lpOverlay, lpTable->GetHypertext(bOptimize, dwColor, nSize), bOptimize, bEmbed, lpEmbeddedObjectBuffer, dwEmbeddedObjectBufferSize, dwEmbeddedObjectOffset, dwColor, nSize, FALSE);

					lpString = lpNext + 1;
				}
				else
					bAppendChar = TRUE;

			}
			else
			if (!strncmp(lpString, FILE_TAG, FILE_TAG_LEN))
			{
				CString strFile;

				LPCSTR lpNext = ScanFileTag(lpString + FILE_TAG_LEN, strFile);

				if (lpNext && (*lpNext == '>'))
				{
					strResult += TranslateExtensionString(lpOverlay, GetTextFromFile(strFile), bOptimize, bEmbed, lpEmbeddedObjectBuffer, dwEmbeddedObjectBufferSize, dwEmbeddedObjectOffset, dwColor, nSize, FALSE);

					lpString = lpNext + 1;
				}
				else
					bAppendChar = TRUE;

			}
			else
			if (!strncmp(lpString, IMAGE_TAG, IMAGE_TAG_LEN))
			{
				nObjFrom = lpString - lpStart;

				CString strSource;

				LPCSTR lpNext = ScanImageTag(lpString + IMAGE_TAG_LEN, dwImageWidth, dwImageHeight, dwImageSpriteX, dwImageSpriteY, dwImageSpriteWidth, dwImageSpriteHeight, nImageRotationAngle);

				if (lpNext && (*lpNext == '>'))
				{
					nObjTo = lpNext - lpStart + 1;

					if (bContextHighlighting)
					{
						if ((m_nPos > nObjFrom	) &&
							(m_nPos < nObjTo	))
						{
							m_nObj			= OBJ_IMAGE;
							m_nObjFrom		= nObjFrom;
							m_nObjTo		= nObjTo;
							m_nObjParams	= nObjParams;
						}
					}
				}

				bAppendChar = TRUE;
			}
			else
			if (!strncmp(lpString, ANIMATED_IMAGE_TAG, ANIMATED_IMAGE_TAG_LEN))
			{
				nObjFrom = lpString - lpStart;

				CString strSource;

				LPCSTR lpNext = ScanAnimatedImageTag(lpString + ANIMATED_IMAGE_TAG_LEN, strSource, dwAnimatedImageWidth, dwAnimatedImageHeight, fltAnimatedImageMin, fltAnimatedImageMax, dwAnimatedImageSpriteX, dwAnimatedImageSpriteY, dwAnimatedImageSpriteWidth, dwAnimatedImageSpriteHeight, dwAnimatedImageSpritesNum, dwAnimatedImageSpritesPerLine, fltAnimatedImageBias, nAnimatedImageRotationAngleMin, nAnimatedImageRotationAngleMax, nObjParams);

				if (lpNext && (*lpNext == '>'))
				{
					nObjTo = lpNext - lpStart + 1;

					if (bContextHighlighting)
					{
						if ((m_nPos > nObjFrom	) &&
							(m_nPos < nObjTo	))
						{
							m_nObj			= OBJ_ANIMATED_IMAGE;
							m_nObjFrom		= nObjFrom;
							m_nObjTo		= nObjTo;
							m_nObjParams	= nObjParams;
						}
					}

					COverlayDataSource* lpSource = lpOverlay->FindDataSource(strSource);

					if (lpSource)
					{
						CString strDataTag = lpSource->GetDataTag();

						if (strlen(strDataTag))
							//data for internal sources (e.g. framerate) is not displayed explicitly because it is application specific, we use
							//special data tags (e.g. <FR>) to let hypertext parser to translate and display correct data in context of each 
							//running 3D application

							//due to this reason we cannot explicitly format animation tags for such data sources, so we use general purpose
							//hypertext variables (<V> tag) and _SPR/_MAP hypertext functions to calculate sprite coordinates and rotation angle
							//in context of each running 3D application
						{
							strDataTag.Replace("<","");
							strDataTag.Replace(">","");
								//data tags are wrapped with <>, we need to unwrap them first prior to passing them as function parameters

							CString strSpriteX;
							CString strSpriteY;

							if (dwAnimatedImageSpritesNum > 1)
								//sprite based animation is in use, we need to define hypertext variable for selecting the sprite in context of
								//3D application by selected source
							{
								strResult += FormatVarSpriteFnTag(0, strDataTag, fltAnimatedImageMin, fltAnimatedImageMax, fltAnimatedImageBias, dwAnimatedImageSpriteX, dwAnimatedImageSpriteY, dwAnimatedImageSpriteWidth, dwAnimatedImageSpriteHeight, dwAnimatedImageSpritesNum, dwAnimatedImageSpritesPerLine);
									//assign V0 variable to _SPR function call result

								strSpriteX = "V0L";	//sprite X is packed in the lowest 16 bits of V0 after _SPR function call
								strSpriteY = "V0H";	//sprite Y is packed in the highest 16 bits of V0 after _SPR function call 
							}
							else
							{
								strSpriteX.Format("%d", dwAnimatedImageSpriteX);
								strSpriteY.Format("%d", dwAnimatedImageSpriteY);
									//format coordinates explicitly, we do not animate them
							}

							CString strRotationAngle;

							if (nAnimatedImageRotationAngleMin != nAnimatedImageRotationAngleMax)
								//rotation based animation is in use, we need to define hypertext variable for calculating rotation angle in context of
								//3D application by selected source
							{
								strRotationAngle = FormatMapFn(strDataTag, fltAnimatedImageMin, fltAnimatedImageMax, (float)nAnimatedImageRotationAngleMin, (float)nAnimatedImageRotationAngleMax);
									//use inplace _MAP function call to calculate rotation angle
							}
							else
							{
								if (nAnimatedImageRotationAngleMin)
									strRotationAngle.Format("%d", nAnimatedImageRotationAngleMin);
								else
									strRotationAngle = "";
							}


							strResult += FormatImageTag(dwAnimatedImageWidth, dwAnimatedImageHeight, strSpriteX, strSpriteY, dwAnimatedImageSpriteWidth, dwAnimatedImageSpriteHeight, strRotationAngle);
						}
						else
						{
							FLOAT fltData			= lpSource->GetData();
							DWORD dwSprite			= 0;
							DWORD dwSpriteIndexX	= 0;
							DWORD dwSpriteIndexY	= 0;

							if ((fltData != FLT_MAX) && (fltAnimatedImageMax > fltAnimatedImageMin))
							{
								if (fltData > fltAnimatedImageMax)
									fltData = fltAnimatedImageMax;

								if (fltData < fltAnimatedImageMin)
									fltData = fltAnimatedImageMin;

								dwSprite = (DWORD)(dwAnimatedImageSpritesNum * (fltData - fltAnimatedImageMin) / (fltAnimatedImageMax - fltAnimatedImageMin) + fltAnimatedImageBias);

								if ((LONG)dwSprite < 0)
									dwSprite = 0;

								if (dwSprite > dwAnimatedImageSpritesNum - 1)
									dwSprite = dwAnimatedImageSpritesNum - 1;
							}
							
							if (!dwAnimatedImageSpritesPerLine)
								dwAnimatedImageSpritesPerLine = dwAnimatedImageSpritesNum;

							if (dwAnimatedImageSpritesNum)
							{
								dwSpriteIndexX	= dwSprite % dwAnimatedImageSpritesPerLine;
								dwSpriteIndexY	= dwSprite / dwAnimatedImageSpritesPerLine;
							}

							int nRotationAngle = 0;

							if (nAnimatedImageRotationAngleMin != nAnimatedImageRotationAngleMax)
								nRotationAngle = (int)(nAnimatedImageRotationAngleMin + (nAnimatedImageRotationAngleMax - nAnimatedImageRotationAngleMin) * (fltData - fltAnimatedImageMin) / (fltAnimatedImageMax - fltAnimatedImageMin));
							else
								nRotationAngle = nAnimatedImageRotationAngleMin;

							strResult += FormatImageTag(dwAnimatedImageWidth, dwAnimatedImageHeight, dwAnimatedImageSpriteX + dwSpriteIndexX * dwAnimatedImageSpriteWidth, dwAnimatedImageSpriteY + dwSpriteIndexY * dwAnimatedImageSpriteHeight, dwAnimatedImageSpriteWidth, dwAnimatedImageSpriteHeight, nRotationAngle);
						}
					}

					lpString = lpNext + 1;
				}
				else
					bAppendChar = TRUE;
			}
			else
			if (!strncmp(lpString, GRAPH_TAG, GRAPH_TAG_LEN))
			{
				nObjFrom = lpString - lpStart;

				CString strSource;

				LPCSTR lpNext = ScanGraphTag(lpString + GRAPH_TAG_LEN, strSource, dwGraphWidth, dwGraphHeight, dwGraphMargin, fltGraphMin, fltGraphMax, dwGraphFlags, nObjParams);

				if (lpNext && (*lpNext == '>'))
				{
					nObjTo = lpNext - lpStart + 1;

					if (bContextHighlighting)
					{
						if ((m_nPos > nObjFrom	) &&
							(m_nPos < nObjTo	))
						{
							m_nObj			= OBJ_GRAPH;
							m_nObjFrom		= nObjFrom;
							m_nObjTo		= nObjTo;
							m_nObjParams	= nObjParams;
						}
					}

					BOOL bFramerate					= !_stricmp(strSource, INTERNAL_SOURCE_FRAMERATE);
					BOOL bFrametime					= !_stricmp(strSource, INTERNAL_SOURCE_FRAMETIME);

					BOOL bFramerateMin				= !_stricmp(strSource, INTERNAL_SOURCE_FRAMERATE_MIN);
					BOOL bFramerateAvg				= !_stricmp(strSource, INTERNAL_SOURCE_FRAMERATE_AVG);
					BOOL bFramerateMax				= !_stricmp(strSource, INTERNAL_SOURCE_FRAMERATE_MAX);
					BOOL bFramerate1Dot0PercentLow	= !_stricmp(strSource, INTERNAL_SOURCE_FRAMERATE_1DOT0_PERCENT_LOW);
					BOOL bFramerate0Dot1PercentLow	= !_stricmp(strSource, INTERNAL_SOURCE_FRAMERATE_0DOT1_PERCENT_LOW);

					if (bFramerate || bFrametime || bFramerateMin || bFramerateAvg || bFramerateMax || bFramerate1Dot0PercentLow || bFramerate0Dot1PercentLow)
					{
						CString strObj;
						if (bOptimize)
							strObj.Format("<OBJ>", dwEmbeddedObjectOffset);
						else
							strObj.Format("<OBJ=%X>", dwEmbeddedObjectOffset);

						strResult += strObj;

						if (bEmbed)
						{
							CRTSSSharedMemoryInterface sharedMemoryInterface;
							if (bFramerate)
								dwEmbeddedObjectOffset += sharedMemoryInterface.EmbedGraphInObjBuffer(lpEmbeddedObjectBuffer, dwEmbeddedObjectBufferSize, dwEmbeddedObjectOffset, NULL, 0, 0, dwGraphWidth, dwGraphHeight, dwGraphMargin, fltGraphMin, fltGraphMax, dwGraphFlags | RTSS_EMBEDDED_OBJECT_GRAPH_FLAG_FRAMERATE); 
							else
							if (bFrametime)
								dwEmbeddedObjectOffset += sharedMemoryInterface.EmbedGraphInObjBuffer(lpEmbeddedObjectBuffer, dwEmbeddedObjectBufferSize, dwEmbeddedObjectOffset, NULL, 0, 0, dwGraphWidth, dwGraphHeight, dwGraphMargin, fltGraphMin * 1000, fltGraphMax * 1000, dwGraphFlags | RTSS_EMBEDDED_OBJECT_GRAPH_FLAG_FRAMETIME); 
							else
							if (bFramerateMin)
								dwEmbeddedObjectOffset += sharedMemoryInterface.EmbedGraphInObjBuffer(lpEmbeddedObjectBuffer, dwEmbeddedObjectBufferSize, dwEmbeddedObjectOffset, NULL, 0, 0, dwGraphWidth, dwGraphHeight, dwGraphMargin, fltGraphMin, fltGraphMax, dwGraphFlags | RTSS_EMBEDDED_OBJECT_GRAPH_FLAG_FRAMERATE_MIN); 
							else
							if (bFramerateAvg)
								dwEmbeddedObjectOffset += sharedMemoryInterface.EmbedGraphInObjBuffer(lpEmbeddedObjectBuffer, dwEmbeddedObjectBufferSize, dwEmbeddedObjectOffset, NULL, 0, 0, dwGraphWidth, dwGraphHeight, dwGraphMargin, fltGraphMin, fltGraphMax, dwGraphFlags | RTSS_EMBEDDED_OBJECT_GRAPH_FLAG_FRAMERATE_AVG); 
							else
							if (bFramerateMax)
								dwEmbeddedObjectOffset += sharedMemoryInterface.EmbedGraphInObjBuffer(lpEmbeddedObjectBuffer, dwEmbeddedObjectBufferSize, dwEmbeddedObjectOffset, NULL, 0, 0, dwGraphWidth, dwGraphHeight, dwGraphMargin, fltGraphMin, fltGraphMax, dwGraphFlags | RTSS_EMBEDDED_OBJECT_GRAPH_FLAG_FRAMERATE_MAX); 
							else
							if (bFramerate1Dot0PercentLow)
								dwEmbeddedObjectOffset += sharedMemoryInterface.EmbedGraphInObjBuffer(lpEmbeddedObjectBuffer, dwEmbeddedObjectBufferSize, dwEmbeddedObjectOffset, NULL, 0, 0, dwGraphWidth, dwGraphHeight, dwGraphMargin, fltGraphMin, fltGraphMax, dwGraphFlags | RTSS_EMBEDDED_OBJECT_GRAPH_FLAG_FRAMERATE_1DOT0_PERCENT_LOW); 
							else
							if (bFramerate0Dot1PercentLow)
								dwEmbeddedObjectOffset += sharedMemoryInterface.EmbedGraphInObjBuffer(lpEmbeddedObjectBuffer, dwEmbeddedObjectBufferSize, dwEmbeddedObjectOffset, NULL, 0, 0, dwGraphWidth, dwGraphHeight, dwGraphMargin, fltGraphMin, fltGraphMax, dwGraphFlags | RTSS_EMBEDDED_OBJECT_GRAPH_FLAG_FRAMERATE_0DOT1_PERCENT_LOW); 
						}

					}
					else
					{
						COverlayDataSource* lpSource = lpOverlay->FindDataSource(strSource);

						if (lpSource)
						{
							CString strObj;
							if (bOptimize)
								strObj.Format("<OBJ>", dwEmbeddedObjectOffset);
							else
								strObj.Format("<OBJ=%X>", dwEmbeddedObjectOffset);

							strResult += strObj;

							if (bEmbed)
							{
								CRTSSSharedMemoryInterface sharedMemoryInterface;
								dwEmbeddedObjectOffset += sharedMemoryInterface.EmbedGraphInObjBuffer(lpEmbeddedObjectBuffer, dwEmbeddedObjectBufferSize, dwEmbeddedObjectOffset, lpSource->GetBuffer(), lpSource->GetBufferPos(), RING_BUFFER_SIZE, dwGraphWidth, dwGraphHeight, dwGraphMargin, fltGraphMin, fltGraphMax, dwGraphFlags); 
							}
						}
					}

					lpString = lpNext + 1;
				}
				else
					bAppendChar = TRUE;
			}
			else
				bAppendChar = TRUE;
		}
		else
			bAppendChar = TRUE;

		if (bAppendChar)
			strResult += *lpString++;
	}

	return strResult;
}
/////////////////////////////////////////////////////////////////////////////
LPCSTR CHypertextExtension::ScanFileTag(LPCSTR lpsz, CString& strFile)
{
	if (*lpsz != '=')
		return NULL;

	return ScanStr(lpsz + 1, strFile);
}
/////////////////////////////////////////////////////////////////////////////
LPCSTR CHypertextExtension::ScanTimeTag(LPCSTR lpsz, CString& strTimeFormat)
{
	if (*lpsz != '=')
		return NULL;

	return ScanStr(lpsz + 1, strTimeFormat);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CHypertextExtension::ValidateTimeFormat(LPCSTR lpFormat)
{
	BOOL bParse = FALSE;

	while (*lpFormat)
	{
		char c = *lpFormat;

		if (bParse)
		{
			if (!strchr("aAbBcdHIjmMpSUwWxXyYZ%", c))
				return FALSE;

			bParse = FALSE;
		}
		else
		{
			if (c == '%')
				bParse = TRUE;
		}

		lpFormat++;
	}

	if (bParse)
		return FALSE;

	return TRUE;
}
//////////////////////////////////////////////////////////////////////
LPCSTR CHypertextExtension::ScanTextTableTag(LPCSTR lpsz, CString& strTable)
{
	if (*lpsz != '=')
		return NULL;

	return ScanStr(lpsz + 1, strTable);
}
/////////////////////////////////////////////////////////////////////////////
LPCSTR CHypertextExtension::ScanGraphTag(LPCSTR lpsz, CString& strSource, DWORD& dwWidth, DWORD& dwHeight, DWORD& dwMargin, float& fltMin, float& fltMax, DWORD& dwFlags, int& nParams)
{
	if (*lpsz != '=')
		return NULL;

	nParams = 0;

	lpsz = ScanStr(lpsz + 1, strSource);
	if (!lpsz)
		return NULL;
	if (*lpsz == '>')
	{
		nParams = 1;
		return lpsz;
	}
	if (*lpsz != ',')
		return NULL;

	lpsz = ScanDec(lpsz + 1, dwWidth);
	if (!lpsz)
		return NULL;
	if (*lpsz == '>')
	{
		nParams = 2;
		return lpsz;
	}
	if (*lpsz != ',')
		return NULL;

	lpsz = ScanDec(lpsz + 1, dwHeight);
	if (!lpsz)
		return NULL;
	if (*lpsz == '>')
	{
		nParams = 3;
		return lpsz;
	}
	if (*lpsz != ',')
		return NULL;

	lpsz = ScanDec(lpsz + 1, dwMargin);
	if (!lpsz)
		return NULL;
	if (*lpsz == '>')
	{
		nParams = 4;
		return lpsz;
	}
	if (*lpsz != ',')
		return NULL;

	lpsz = ScanFlt(lpsz + 1, fltMin);
	if (!lpsz)
		return NULL;
	if (*lpsz == '>')
	{
		nParams = 5;
		return lpsz;
	}
	if (*lpsz != ',')
		return NULL;

	lpsz = ScanFlt(lpsz + 1, fltMax);
	if (!lpsz)
		return NULL;
	if (*lpsz == '>')
	{
		nParams = 6;
		return lpsz;
	}
	if (*lpsz != ',')
		return NULL;

	nParams = 7;

	return ScanHex(lpsz + 1, dwFlags);
}
/////////////////////////////////////////////////////////////////////////////
LPCSTR CHypertextExtension::ScanImageTag(LPCSTR lpsz, DWORD& dwWidth, DWORD& dwHeight, DWORD& dwSpriteX, DWORD& dwSpriteY, DWORD& dwSpriteWidth, DWORD& dwSpriteHeight, int& nRotationAngle)
{
	if (*lpsz != '=')
		return NULL;

	lpsz = ScanDec(lpsz + 1, dwWidth);
	if (!lpsz || (*lpsz != ','))
		return NULL;

	lpsz = ScanDec(lpsz + 1, dwHeight);
	if (!lpsz || (*lpsz != ','))
		return NULL;

	lpsz = ScanDec(lpsz + 1, dwSpriteX);
	if (!lpsz || (*lpsz != ','))
		return NULL;

	lpsz = ScanDec(lpsz + 1, dwSpriteY);
	if (!lpsz || (*lpsz != ','))
		return NULL;

	lpsz = ScanDec(lpsz + 1, dwSpriteWidth);
	if (!lpsz || (*lpsz != ','))
		return NULL;

	lpsz = ScanDec(lpsz + 1, dwSpriteHeight);
	if (!lpsz)
		return NULL;
	if (*lpsz == '>')
		return lpsz;
	if (*lpsz != ',')
		return NULL;

	return ScanDec(lpsz + 1, (DWORD&)nRotationAngle);
}
/////////////////////////////////////////////////////////////////////////////
LPCSTR	CHypertextExtension::ScanAnimatedImageTag(LPCSTR lpsz, CString& strSource, DWORD& dwWidth, DWORD& dwHeight, float& fltMin, float& fltMax, DWORD& dwSpriteX, DWORD& dwSpriteY, DWORD& dwSpriteWidth, DWORD& dwSpriteHeight, DWORD& dwSpritesNum, DWORD& dwSpritesPerLine, float& fltBias, int& nRotationAngleMin, int& nRotationAngleMax, int& nParams)
{
	if (*lpsz != '=')
		return NULL;

	nParams = 0;

	lpsz = ScanStr(lpsz + 1, strSource);
	if (!lpsz)
		return NULL;
	if (*lpsz == '>')
	{
		nParams = 1;
		return lpsz;
	}
	if (*lpsz != ',')
		return NULL;

	lpsz = ScanDec(lpsz + 1, dwWidth);
	if (!lpsz)
		return NULL;
	if (*lpsz == '>')
	{
		nParams = 2;
		return lpsz;
	}
	if (*lpsz != ',')
		return NULL;

	lpsz = ScanDec(lpsz + 1, dwHeight);
	if (!lpsz)
		return NULL;
	if (*lpsz == '>')
	{
		nParams = 3;
		return lpsz;
	}
	if (*lpsz != ',')
		return NULL;

	lpsz = ScanFlt(lpsz + 1, fltMin);
	if (!lpsz)
		return NULL;
	if (*lpsz == '>')
	{
		nParams = 4;
		return lpsz;
	}
	if (*lpsz != ',')
		return NULL;

	lpsz = ScanFlt(lpsz + 1, fltMax);
	if (!lpsz)
		return NULL;
	if (*lpsz == '>')
	{
		nParams = 5;
		return lpsz;
	}
	if (*lpsz != ',')
		return NULL;

	lpsz = ScanDec(lpsz + 1, dwSpriteX);
	if (!lpsz)
		return NULL;
	if (*lpsz == '>')
	{
		nParams = 6;
		return lpsz;
	}
	if (*lpsz != ',')
		return NULL;

	lpsz = ScanDec(lpsz + 1, dwSpriteY);
	if (!lpsz)
		return NULL;
	if (*lpsz == '>')
	{
		nParams = 7;
		return lpsz;
	}
	if (*lpsz != ',')
		return NULL;

	lpsz = ScanDec(lpsz + 1, dwSpriteWidth);
	if (!lpsz)
		return NULL;
	if (*lpsz == '>')
	{
		nParams = 8;
		return lpsz;
	}
	if (*lpsz != ',')
		return NULL;

	lpsz = ScanDec(lpsz + 1, dwSpriteHeight);
	if (!lpsz)
		return NULL;
	if (*lpsz == '>')
	{
		nParams = 9;
		return lpsz;
	}
	if (*lpsz != ',')
		return NULL;

	lpsz = ScanDec(lpsz + 1, dwSpritesNum);
	if (!lpsz)
		return NULL;
	if (*lpsz == '>')
	{
		nParams = 10;
		return lpsz;
	}
	if (*lpsz != ',')
		return NULL;

	lpsz = ScanDec(lpsz + 1, dwSpritesPerLine);
	if (!lpsz)
		return NULL;
	if (*lpsz == '>')
	{
		nParams = 11;
		return lpsz;
	}
	if (*lpsz != ',')
		return NULL;

	lpsz = ScanFlt(lpsz + 1, fltBias);
	if (!lpsz)
		return NULL;
	if (*lpsz == '>')
	{
		nParams = 12;
		return lpsz;
	}
	if (*lpsz != ',')
		return NULL;

	lpsz = ScanDec(lpsz + 1, (DWORD&)nRotationAngleMin);
	if (!lpsz)
		return NULL;
	if (*lpsz == '>')
	{
		nParams = 13;
		return lpsz;
	}
	if (*lpsz != ',')
		return NULL;

	nParams = 14;

	return ScanDec(lpsz + 1, (DWORD&)nRotationAngleMax);
}
/////////////////////////////////////////////////////////////////////////////
void CHypertextExtension::SetPos(int nPos)
{
	m_nPos = nPos;
}
/////////////////////////////////////////////////////////////////////////////
int CHypertextExtension::GetPos()
{
	return m_nPos;
}
/////////////////////////////////////////////////////////////////////////////
int CHypertextExtension::GetObj()
{
	return m_nObj;
}
/////////////////////////////////////////////////////////////////////////////
int CHypertextExtension::GetObjFrom()
{
	return m_nObjFrom;
}
/////////////////////////////////////////////////////////////////////////////
int CHypertextExtension::GetObjTo()
{
	return m_nObjTo;
}
/////////////////////////////////////////////////////////////////////////////
int CHypertextExtension::GetObjParams()
{
	return m_nObjParams;
}
/////////////////////////////////////////////////////////////////////////////
void CHypertextExtension::SetObj(int nObjFrom, int nObjTo)
{
	m_nObjFrom	= nObjFrom;
	m_nObjTo	= nObjTo;
}
/////////////////////////////////////////////////////////////////////////////
CString CHypertextExtension::FormatGraphTag(BOOL bCustomTemplate, LPCSTR lpSource, DWORD dwWidth, DWORD dwHeight, DWORD dwMargin, float fltMin, float fltMax, DWORD dwFlags)
{
	CString strTag;
	if (bCustomTemplate)
	{
		CString strMin = COverlayDataSource::FormatFloat(fltMin);
		CString strMax = COverlayDataSource::FormatFloat(fltMax);

		strTag.Format("<G=%s,%d,%d,%d,%s,%s,%X>", lpSource, dwWidth, dwHeight, dwMargin, strMin, strMax, dwFlags);
	}
	else
		strTag.Format("<G=%s>", lpSource);

	return strTag;
}
/////////////////////////////////////////////////////////////////////////////
CString	CHypertextExtension::FormatImageTag(DWORD dwWidth, DWORD dwHeight, DWORD dwSpriteX, DWORD dwSpriteY, DWORD dwSpriteWidth, DWORD dwSpriteHeight, int nRotationAngle)
{
	CString strTag;

	if (nRotationAngle)
		strTag.Format("<I=%d,%d,%d,%d,%d,%d,%d>", dwWidth, dwHeight, dwSpriteX, dwSpriteY, dwSpriteWidth, dwSpriteHeight, nRotationAngle);
	else
		strTag.Format("<I=%d,%d,%d,%d,%d,%d>", dwWidth, dwHeight, dwSpriteX, dwSpriteY, dwSpriteWidth, dwSpriteHeight);

	return strTag;
}
/////////////////////////////////////////////////////////////////////////////
CString	CHypertextExtension::FormatImageTag(DWORD dwWidth, DWORD dwHeight, LPCSTR lpSpriteX, LPCSTR lpSpriteY, DWORD dwSpriteWidth, DWORD dwSpriteHeight, LPCSTR lpRotationAngle)
{
	CString strTag;

	if (strlen(lpRotationAngle))
		strTag.Format("<I=%d,%d,%s,%s,%d,%d,%s>", dwWidth, dwHeight, lpSpriteX, lpSpriteY, dwSpriteWidth, dwSpriteHeight, lpRotationAngle);
	else
		strTag.Format("<I=%d,%d,%s,%s,%d,%d>", dwWidth, dwHeight, lpSpriteX, lpSpriteY, dwSpriteWidth, dwSpriteHeight);

	return strTag;
}
/////////////////////////////////////////////////////////////////////////////
CString	CHypertextExtension::FormatAnimatedImageTag(BOOL bCustomTemplate, LPCSTR lpSource, DWORD dwWidth, DWORD dwHeight, float fltMin, float fltMax, DWORD dwSpriteX, DWORD dwSpriteY, DWORD dwSpriteWidth, DWORD dwSpriteHeight, DWORD dwSpritesNum, DWORD dwSpritesPerLine, float fltBias, int nRotationAngleMin, int nRotationAngleMax)
{
	CString strTag;
	if (bCustomTemplate)
	{
		CString strMin	= COverlayDataSource::FormatFloat(fltMin);
		CString strMax	= COverlayDataSource::FormatFloat(fltMax);
		CString strBias	= COverlayDataSource::FormatFloat(fltBias);

		strTag.Format("<AI=%s,%d,%d,%s,%s,%d,%d,%d,%d,%d,%d,%s,%d,%d>", lpSource, dwWidth, dwHeight, strMin, strMax, dwSpriteX, dwSpriteY, dwSpriteWidth, dwSpriteHeight, dwSpritesNum, dwSpritesPerLine, strBias, nRotationAngleMin, nRotationAngleMax);
	}
	else
		strTag.Format("<AI=%s>", lpSource);

	return strTag;

}
/////////////////////////////////////////////////////////////////////////////
CString CHypertextExtension::GetTextFromFile(LPCSTR lpFile)
{
	return "";
}
/////////////////////////////////////////////////////////////////////////////
CString CHypertextExtension::FormatVarSpriteFnTag(DWORD dwVariable, LPCSTR lpSource, float fltMin, float fltMax, float fltBias, DWORD dwSpriteX, DWORD dwSpriteY, DWORD dwSpriteWidth, DWORD dwSpriteHeight, DWORD dwSpritesNum, DWORD dwSpritesPerLine)
{
	CString strMin	= COverlayDataSource::FormatFloat(fltMin);
	CString strMax	= COverlayDataSource::FormatFloat(fltMax);
	CString strBias	= COverlayDataSource::FormatFloat(fltBias);

	CString strTag;
	strTag.Format("<V%d=$SPR(%s,%s,%s,%s,%d,%d,%d,%d,%d,%d)>", dwVariable, lpSource, strMin, strMax, strBias, dwSpriteX, dwSpriteY, dwSpriteWidth, dwSpriteHeight, dwSpritesNum, dwSpritesPerLine);

	return strTag;
}
/////////////////////////////////////////////////////////////////////////////
CString	CHypertextExtension::FormatMapFn(LPCSTR lpSource, float fltMin, float fltMax, float fltDstMin, float fltDstMax)
{
	CString strMin		= COverlayDataSource::FormatFloat(fltMin);
	CString strMax		= COverlayDataSource::FormatFloat(fltMax);
	CString strDstMin	= COverlayDataSource::FormatFloat(fltDstMin);
	CString strDstMax	= COverlayDataSource::FormatFloat(fltDstMax);

	CString strTag;
	strTag.Format("$MAP(%s,%s,%s,%s,%s)", lpSource, strMin, strMax, strDstMin, strDstMax);

	return strTag;
}
/////////////////////////////////////////////////////////////////////////////
