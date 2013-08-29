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

#pragma ToDo: Lets use Callback Methods in this lib.

typedef struct ParsingInformation
{
	char* pcFilePath; 	// Path of MIDI File
	FILE* pfFile; 		// File Handle of MIDI File

	MidiHeader
};

typedef struct MidiHeader
{
	int iType;		// Type of Midi File, either MIDI_TYPE0 or MIDI_TYPE1
	int nTracks;	// Number of tracks
	int nTicks; 	// Number of ticks (msecs) per quarter note

	MidiTrack *pmtTrackList;
};

typedef struct MidiTrack
{
	/* some Metadata in the future... */
};