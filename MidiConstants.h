/****************************************************************/
/*																*/
/*				Project:	Simple Midi Parser					*/
/*																*/
/*				File: MidiParser.h								*/
/*																*/
/*				Author: Eugen Füchsle							*/
/*																*/
/*																*/
/*				Task: Defines Constants used in Project			*/
/*																*/
/*																*/
/****************************************************************/

#ifndef _MDI_MIDICONSTANTS_H
#define _MDI_MIDICONSTANTS_H

typedef char MIDIError;

#ifndef NULL 
#define NULL 0
#endif

#define MID_HEADER_START "MThd"
#define MID_TRACK_START  "MTrk"

#define MID_FILE_HEADERSIZE_BYTES 4

#define MID_TYPE0 0
#define MID_TYPE1 1

#define MID_EVENT_NOTEOFF			0x8
#define MID_EVENT_NOTEON			0x9
#define MID_EVENT_AFTERTOUCH		0xA
#define MID_EVENT_CONTROLCHANGE		0xB
#define MID_EVENT_PROGRAMCHANGE		0xC
#define MID_EVENT_CHANNELPRESSURE	0xD
#define MID_EVENT_PITCHWHEEL		0xE

#define MID_ERR_OK				 0
#define MID_ERR_MALLOC			-1
#define MID_ERR_FILEOPEN		-2
#define MID_ERR_INVALIDPARAM	-3
#define MID_ERR_FILEERR			-4
#define MID_ERR_NOMIDIHEADER	-5
#define MID_ERR_INVHEADER		-6
#define MID_ERR_INVMIDTYPE		-7

#define MID_ERR_INVTRACKHEADER	-8

#endif