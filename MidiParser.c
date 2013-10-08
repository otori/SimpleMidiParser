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
#include "MidiConstants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _NO_BOOL //Fuck some c-compiler
#define bool	char
#define true	1
#define false	0
#endif

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
	
	pMidiParser->pfFile = fopen(pcMidiPath , "rb");

	if(!pMidiParser->pfFile)
	{
		error = MID_ERR_FILEOPEN;
		goto cleanup_err;
	}

	//Get file length
	fseek(pMidiParser->pfFile, 0, SEEK_END);
	pMidiParser->iBufLen = ftell(pMidiParser->pfFile);
	fseek(pMidiParser->pfFile, 0, SEEK_SET);

	pMidiParser->pucMidiBuffer = (unsigned char*) malloc(pMidiParser->iBufLen);
	if(pMidiParser->pucMidiBuffer == NULL)
	{
		error = MID_ERR_MALLOC;
		goto cleanup_err;
	}

	fread(pMidiParser->pucMidiBuffer, 1, pMidiParser->iBufLen, pMidiParser->pfFile);
	
	fclose(pMidiParser->pfFile);
	pMidiParser->pfFile = NULL;

	*pParserStruct = pMidiParser;
	return MID_ERR_OK;

cleanup_err:
	/*Free the already allocated stuff*/
	if(pMidiParser)
	{		
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
	unsigned int i;
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
	memset(mp->pMidiHeader, 0, sizeof(MDI_MidiHeader));

	//Checking for Header Marker
	for(i = 0; i < sizeof(strMidiHeader) - 1; i++)
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

MIDIError MDI_parseNextTrack(MDI_ParsingInformation *pMidiParser)
{
	MIDIError error = MID_ERR_OK;
	char strMidiTrack[] = MID_TRACK_START;
	unsigned char strMidiTrackEnd[] = {0x00, 0xFF, 0x2F, 0x00};
	MDI_ParsingInformation *mp = pMidiParser;
	MDI_MidiHeader *mh;
	MDI_MidiTrack *mTrack = NULL;
	MDI_EventList *mEventList = NULL, *mEventCur;
	MDI_Event *mEvent;
	int i,k;
	int iTimeStamp;
	unsigned char ucTmp;
	
	if(!mp)
	{
		error = MID_ERR_INVALIDPARAM;
		goto cleanup_err;
	}
	mh = mp->pMidiHeader;
	if(!mh)
	{
		error = MID_ERR_INVALIDPARAM;
		goto cleanup_err;
	}
	
	mTrack = (MDI_MidiTrack*) malloc(sizeof(MDI_MidiTrack));		
	if(!mTrack)
	{
		error = MID_ERR_MALLOC;
		goto cleanup_err;
	}
	memset(mTrack, 0, sizeof(MDI_MidiTrack));

	if(!mh->pmtTrackList)
		mh->pmtTrackList = mTrack;
	else
	{
		MDI_MidiTrack *pmtCur = mh->pmtTrackList;
		while(pmtCur->pmtNextTrack) pmtCur = pmtCur->pmtNextTrack;
		pmtCur->pmtNextTrack = mTrack;
	}

	if(mp->curPos + 4 > mp->iBufLen)
	{
		error = MID_ERR_FILEERR;
		goto cleanup_err;
	}

	for(i = 0; i < sizeof(strMidiTrack) - 1; ++i)
	{
		if(i + mp->curPos >= mp->iBufLen)
		{
			error = MID_ERR_FILEERR;
			goto cleanup_err;
		}

		if(mp->pucMidiBuffer[mp->curPos + i] != strMidiTrack[i])
		{
			error = MID_ERR_INVTRACKHEADER;
			goto cleanup_err;
		}
	}
	mp->curPos += 4;

	if(mp->curPos + 4 > mp->iBufLen)
	{
		error = MID_ERR_FILEERR;
		goto cleanup_err;
	}
	mp->curPos += 4; //Ignore size of Track data. It is incorrect in most cases, claims <http://www.ccarh.org/courses/253/assignment/midifile/>

	mEventList = (MDI_EventList*) malloc(sizeof(MDI_EventList));
	if(!mEventList)
	{
		error = MID_ERR_MALLOC;
		goto cleanup_err;
	}
	mEventList->pEvent = NULL;
	mEventList->pNext = NULL;
	mTrack->pEvents = mEventList;
	mEventCur = mEventList;

	while(1)
	{		
		//Loop Start

		if(mp->curPos + 4 > mp->iBufLen)
		{
			error = MID_ERR_FILEERR;
			goto cleanup_err;
		}

		if(mp->pucMidiBuffer[mp->curPos] == strMidiTrackEnd[0] &&
			mp->pucMidiBuffer[mp->curPos + 1] == strMidiTrackEnd[1] &&	
			mp->pucMidiBuffer[mp->curPos + 2] == strMidiTrackEnd[2] &&
			mp->pucMidiBuffer[mp->curPos + 3] == strMidiTrackEnd[3]
		)
		{
			// End of MIDI Track;
			mp->curPos += 4; 
			return MID_ERR_OK;
		}

		//Parse Event :
		iTimeStamp = 0; i = 0;	
		while(mp->pucMidiBuffer[mp->curPos + i] > 0x80)i++; // amount of bytes of timestamp
		for(k = 0; k < i; ++k)
		{
			iTimeStamp += mp->pucMidiBuffer[mp->curPos] & 0x7F;
			iTimeStamp = iTimeStamp << 7;
			mp->curPos++;
		}		
		iTimeStamp += mp->pucMidiBuffer[mp->curPos];
		mp->curPos++;

		if(mp->pucMidiBuffer[mp->curPos] == 0xFF) // MetaData;
		{
			mp->curPos += 3 + mp->pucMidiBuffer[mp->curPos + 2]; // Skip dat.
		}
		else
		{
			mEvent = (MDI_Event*) malloc(sizeof(MDI_Event));
			if(!mEvent)
			{
				error = MID_ERR_MALLOC;
				goto cleanup_err;
			}

			ucTmp = mp->pucMidiBuffer[mp->curPos];
			mEvent->cEventCode = ucTmp >>  4;
			mEvent->cChannel = ucTmp & 0xF;
			mp->curPos++;

			mEvent->cParam1 = mp->pucMidiBuffer[mp->curPos]; mp->curPos++;
			mEvent->cParam2 = mp->pucMidiBuffer[mp->curPos]; mp->curPos++;
			mEvent->iTimeStamp = iTimeStamp;
		
			// todo Here could occurr a end of file buffer... lets catch it.

			mEventCur->pEvent = mEvent;
			mEventCur->pNext = (MDI_EventList*) malloc(sizeof(MDI_EventList));
			if(!mEventCur->pNext)
			{
				error = MID_ERR_MALLOC;
				goto cleanup_err;
			}
			mEventCur = mEventCur->pNext;
			mEventCur->pEvent = NULL;
			mEventCur->pNext = NULL;
		}
	}

cleanup_err:

	if(mTrack)
	{
		free(mTrack);
	}

	return error;
}
