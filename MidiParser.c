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

MIDIError initParser(char * pcMidiPath, ParsingInformation **pMidiParser)
{
	MIDIError error = MIDI_ERR_OK;

	ParsingInformation* pMidiParser = NULL; 
	pMidiParser = (ParsingInformation*) malloc(sizeof(ParsingInformation));
	if(pMidiParser == NULL)
	{
		error = MID_ERR_MALLOC;
		goto cleanup;
	}

	memset ( pMidiParser, 0, sizeof(ParsingInformation) );
	int iPathlen = strlen(pcMidiPath);
	pMidiParser->pcFilePath = (char *)malloc(iPathlen);
	if(!pMidiParser->pcFilePath)
		return MID_ERR_MALLOC;

	pMidiO fopen();

cleanup:
	/*Free the already allocated stuff*/
	
	return error;
}

