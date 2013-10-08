/****************************************************************/
/*																*/
/*				Project:	Simple Midi Parser					*/
/*																*/
/*				File: MidiParser.h								*/
/*																*/
/*				Author: Eugen Füchsle							*/
/*																*/
/*																*/
/*				Task: Should parse a MIDI File					*/
/*																*/
/*																*/
/****************************************************************/

#ifndef _MDI_MIDIPARSER_H
#define _MDI_MIDIPARSER_H

#include "MidiConstants.h"
#include <stdio.h>

/* Nooot, to lazy :D
#pragma ToDo: Lets use Callback Methods in this lib.

typedef struct MidiParserCallbacks
{
	void * (*MIDI_malloc) ()
}
*/

typedef struct 
{
	int	iTimeStamp;		// Ticks left since last event.
	unsigned char cEventCode;	// See Constants
	unsigned char cChannel;		// Between 0 and 15
	unsigned char cParam1;
	unsigned char cParam2;
} MDI_Event;

typedef struct MDI_EventList
{
	MDI_Event *pEvent;
	struct MDI_EventList *pNext;

} MDI_EventList;

typedef struct MDI_MidiTrack
{
	/* some Metadata in the future... */

	MDI_EventList *pEvents;
	struct MDI_MidiTrack *pmtNextTrack; //Pointer to Next track...
} MDI_MidiTrack;

typedef struct 
{
	int iType;		// Type of Midi File, either MID_TYPE0 or MID_TYPE1
	int nTracks;	// Number of tracks
	int nTicks; 	// Number of ticks (msecs) per quarter note

	MDI_MidiTrack *pmtTrackList;
} MDI_MidiHeader;

typedef struct 
{	
	FILE* pfFile; 		// File Handle of MIDI File
	unsigned char *pucMidiBuffer;	// I will write the whole file into this buffer.
	unsigned int iBufLen;			// Length of buffer.
	unsigned int curPos;			// Current position in buffer.

	MDI_MidiHeader *pMidiHeader; 
} MDI_ParsingInformation;


/**
	Initiates the parser and sets the file to pcMidiPath;
	A Pointer to the parser Struct is createt at pMidiParser
**/
MIDIError MDI_initParser(char *pcMidiPath, MDI_ParsingInformation **pMidiParser);

/**
	Parses the Header of the file;
	Pass the ParsingInformation struct as argument.
**/
MIDIError MDI_parseHeader(MDI_ParsingInformation *pMidiParser);

/**
	Finds and parses the next Track from curPos of the file;
	Pass the ParsingInformation struct as argument.
**/
MIDIError MDI_parseNextTrack(MDI_ParsingInformation *pMidiParser);

#endif