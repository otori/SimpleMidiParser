/****************************************************************/
/*																*/
/*				Project:	Simple Midi Parser					*/
/*																*/
/*				File: MidiParser.c								*/
/*																*/
/*				Author: Eugen Füchsle							*/
/*																*/
/*																*/
/*				Task: Should parse a MIDI File					*/
/*																*/
/*																*/
/****************************************************************/

#include "MidiParser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

MIDIError MDI_initParser(char * pcMidiPath, MDI_ParsingInformation **pParserStruct)
{
	MIDIError error = MID_ERR_OK;
	int iPathlen = 0;

	MDI_ParsingInformation* pMidiParser = NULL; 
	pMidiParser = (MDI_ParsingInformation*) malloc(sizeof(MDI_ParsingInformation));
	if(pMidiParser == NULL)
	{
		error = MID_ERR_MALLOC;
		goto cleanup_err;
	}

	memset ( pMidiParser, 0, sizeof(MDI_ParsingInformation) );
	iPathlen = strlen(pcMidiPath);
	pMidiParser->pcFilePath = (char *)malloc(iPathlen);
	if(!pMidiParser->pcFilePath)
	{
		error = MID_ERR_MALLOC;
		goto cleanup_err;
	}

	pMidiParser->pfFile = fopen(pMidiParser->pcFilePath , "rb");

	if(!pMidiParser->pfFile)
	{
		error = MID_ERR_FILEOPEN;
		goto cleanup_err;
	}

	//Get file length
	fseek(pMidiParser->pfFile, 0, SEEK_END);
	pMidiParser->iBufLen = ftell(pMidiParser->pfFile);
	fseek(pMidiParser->pfFile, 0, SEEK_SET);

	fread(pMidiParser->pucMidiBuffer, 1, pMidiParser->iBufLen, pMidiParser->pfFile);
	
	fclose(pMidiParser->pfFile);
	pMidiParser->pfFile = NULL;

	*pParserStruct = pMidiParser;
	return MID_ERR_OK;

cleanup_err:
	/*Free the already allocated stuff*/
	if(pMidiParser)
	{
		if(pMidiParser->pcFilePath)
			free(pMidiParser->pcFilePath);
		if(pMidiParser->pfFile)
			fclose(pMidiParser->pfFile);
		if(pMidiParser->pucMidiBuffer)
			free(pMidiParser->pucMidiBuffer);
		free(pMidiParser);
	}

	return error;
}

MIDIError MDI_parseHeader(MDI_ParsingInformation *pMidiParser)
{
	MDI_ParsingInformation *mp = pMidiParser;
	MIDIError error = MID_ERR_OK;
	char strMidiHeader[] = MID_HEADER_START;
	int i;
	size_t headerSize;
	bool bType = true; 

	if(!mp)
	{
		error = MID_ERR_INVALIDPARAM;
		goto cleanup_err;
	}

	mp->pMidiHeader = (MDI_MidiHeader*) malloc(sizeof(MDI_MidiHeader));
	if(!mp->pMidiHeader)
	{
		error = MID_ERR_MALLOC;
		goto cleanup_err;
	}

	//Checking for Header Marker
	for(i = 0; i < sizeof(strMidiHeader); i++)
	{
		if(i > mp->iBufLen)
		{
			error = MID_ERR_FILEERR;
			goto cleanup_err;
		}

		if(mp->pucMidiBuffer[mp->curPos] != strMidiHeader[i])
		{
			error = MID_ERR_NOMIDIHEADER;
			goto cleanup_err;
		}
		mp->curPos++;
	}

	if(mp->curPos + 4 > mp->iBufLen)
	{
		error = MID_ERR_FILEERR;
		goto cleanup_err;
	}

	//Reading size of Header
	headerSize = 6;
	if(	mp->pucMidiBuffer[mp->curPos] != 0	 ||
		mp->pucMidiBuffer[mp->curPos + 1] != 0 ||
		mp->pucMidiBuffer[mp->curPos + 2] != 0 ||
		mp->pucMidiBuffer[mp->curPos + 3] != 6)
	{
		error = MID_ERR_INVHEADER;
		goto cleanup_err;
	}
	mp->curPos += 4;

	if(mp->curPos + 2 > mp->iBufLen)
	{
		error = MID_ERR_FILEERR;
		goto cleanup_err;
	}
	//Reading type of MIDI File
	if(mp->pucMidiBuffer[mp->curPos] == 0)
	{
		if(mp->pucMidiBuffer[mp->curPos + 1] == 0)
			mp->pMidiHeader->iType = MID_TYPE0;
		else if (mp->pucMidiBuffer[mp->curPos + 1] == 1)
			mp->pMidiHeader->iType = MID_TYPE1;
		else
			bType = false;
	}
	else bType = false;
	mp->curPos += 2;

	if(mp->curPos + 2 > mp->iBufLen)
	{
		error = MID_ERR_FILEERR;
		goto cleanup_err;
	}
	//Reading number of tracks
	mp->pMidiHeader->nTracks = 0;
	mp->pMidiHeader->nTracks += mp->pucMidiBuffer[mp->curPos] << 0x8; //Shifting by one byte
	mp->pMidiHeader->nTracks += mp->pucMidiBuffer[mp->curPos + 1];

	mp->curPos += 2;

	if(mp->curPos + 2 > mp->iBufLen)
	{
		error = MID_ERR_FILEERR;
		goto cleanup_err;
	}
	//Reading ticks per quarter
	mp->pMidiHeader->nTicks = 0;
	mp->pMidiHeader->nTicks += mp->pucMidiBuffer[mp->curPos] << 0x8; //Shifting by one byte
	mp->pMidiHeader->nTicks += mp->pucMidiBuffer[mp->curPos + 1];

	mp->curPos += 2;

	if(mp->pMidiHeader->iType == MID_TYPE0 && mp->pMidiHeader->nTracks != 1)
	{
		error = MID_ERR_FILEERR; // We could also forgive this kind of file syntax error...
		goto cleanup_err;
	}

	if(!bType)
	{
		error = MID_ERR_INVMIDTYPE;
		goto cleanup_err;
	}
		
	return error;

cleanup_err:
	if(mp)
	{
		if(mp->pMidiHeader)
			free(mp->pMidiHeader);
	}
	return error;
}

