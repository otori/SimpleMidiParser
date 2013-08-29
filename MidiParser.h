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

/* Nooot, to lazy :D
#pragma ToDo: Lets use Callback Methods in this lib.

typedef struct MidiParserCallbacks
{
	void * (*MIDI_malloc) ()
}
*/

typedef struct ParsingInformation
{
	char* pcFilePath; 	// Path of MIDI File
	FILE* pfFile; 		// File Handle of MIDI File

	MidiHeader *pMidiHeader; 
};

typedef struct MidiHeader
{
	int iType;		// Type of Midi File, either MID_TYPE0 or MID_TYPE1
	int nTracks;	// Number of tracks
	int nTicks; 	// Number of ticks (msecs) per quarter note

	MidiTrack *pmtTrackList;
};

typedef struct MidiTrack
{
	/* some Metadata in the future... */

	MidiTrack *pmtNextTrack; //Pointer to Next track...
};

typedef struct EventList
{
	Event *pEvent;
	EventList* pNext;
};

typedef struct Event
{
	int	iTimeStamp;		// Ticks left since last event.
	char cEventCode;	// See Constants
	char cChannel;		// Between 0 and 15
	char cParam1;
	char cParam2;
};

/**
	Initiates the parser and sets the file to pcMidiPath;
	A Pointer to the parser Struct is createt at pMidiParser
**/
MIDIError initParser(char * pcMidiPath, ParsingInformation **pMidiParser);