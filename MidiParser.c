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
		free(pMidiParser);
	}

	return error;
}

