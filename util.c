#include "util.h"

#include <MacTypes.h>
#include <Resources.h>
#include <Sound.h>
#include <TextUtils.h>
#include <Traps.h>

void getRsrcStr(short id, Str255 dest) {
	Handle h;
	
	h = GetResource('STR ', id);
	HLock(h);
	BlockMove(*h, dest, 256);
	HUnlock(h);
	ReleaseResource(h);
}

typedef struct {
	OSErr err;
	Str255 string;
} errDictEntry;

void getErrorString(OSErr error, Str255 dest) {
	Handle h;
	Boolean found = 0;
	short count;
	char* ptr;
	char len;
	int i;
	
	h = GetResource('ERST', kErrorDictionaryID);
	HLock(h);
	ptr = *h;
	count = *(short*)ptr;
	ptr += 2;
	
	for (i = 0; i < count; i++) {
		if (((errDictEntry*)ptr)->err == error) {
			BlockMove(((errDictEntry*)ptr)->string, dest, ((errDictEntry*)ptr)->string[0]+1);
			found = 1;
		} else {
			len = ((errDictEntry*)ptr)->string[0];
			ptr += 3;
			ptr += len;
		}
	}
	HUnlock(h);
	ReleaseResource(h);
	
	if (!found) {
		NumToString(error, dest);
	}
	
	return;
}

Boolean trapAvailable(short theTrap) {
	TrapType theTrapType;
	short numToolboxTraps;
	
	if ((theTrap & 0x0800) > 0) {
		theTrapType = ToolTrap;
	} else {
		theTrapType = OSTrap;
	}

	if (theTrapType == ToolTrap) {
		theTrap = theTrap & 0x07ff;
		
		// See IM: OS Utilities.  The SE and Plus mask tool traps
		// differently (wrongly?) so we compensate for that here.
		if (NGetTrapAddress(_InitGraf, ToolTrap) == NGetTrapAddress(0xaa6e, ToolTrap)) {
			numToolboxTraps = 0x0200;
		} else {
			numToolboxTraps = 0x0400;
		}
		
		if (theTrap >= numToolboxTraps) {
			theTrap = _Unimplemented;
		}
	}

	return (NGetTrapAddress(theTrap, theTrapType) != NGetTrapAddress(_Unimplemented, ToolTrap));
}
