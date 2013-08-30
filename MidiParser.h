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
	char cEventCode;	// See Constants
	char cChannel;		// Between 0 and 15
	char cParam1;
	char cParam2;
} Event;

typedef struct EventList
{
	Event *pEvent;
	struct EventList *pNext;
} EventList;

typedef struct MidiTrack
{
	/* some Metadata in the future... */

	EventList *pEvents;
	struct MidiTrack *pmtNextTrack; //Pointer to Next track...
} MidiTrack;

typedef struct 
{
	int iType;		// Type of Midi File, either MID_TYPE0 or MID_TYPE1
	int nTracks;	// Number of tracks
	int nTicks; 	// Number of ticks (msecs) per quarter note

	MidiTrack *pmtTrackList;
} MidiHeader;

typedef struct 
{
	char* pcFilePath; 	// Path of MIDI File
	FILE* pfFile; 		// File Handle of MIDI File

	MidiHeader *pMidiHeader; 
} ParsingInformation;


/**
	Initiates the parser and sets the file to pcMidiPath;
	A Pointer to the parser Struct is createt at pMidiParser
**/
MIDIError initParser(char *pcMidiPath, ParsingInformation **pMidiParser);