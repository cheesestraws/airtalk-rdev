#include <MacTypes.h>
#include <Dialogs.h>
#include <Resources.h>
#include <Lists.h>

#include "dlg.h"
#include "util.h"
#include "net.h"

extern void __Startup__(void);		// code resource/DRVR startup code
extern pascal OSErr main (short message, short caller,
	StringPtr objName, StringPtr zoneName,
	long p1, long p2);
	
void aboutBox(void);
void doPasswordDlg(ListHandle lst);
	
asm void __Startup__(void)			// Note: this must be the first code in this file !
{
	bra.s	start					// 0	standard macintosh code resource header
	dc.w	89						// 2	Device ID
	dc.l	'PACK'					// 4	code resource type
	dc.w	0xF000					// 8	code resource id
	dc.w	1						// 10	code resource version number
	dc.l	0x8E000000				// 12	flags (IM: Devices p. 1-46)

start:
	jmp		main					// direct jump to main (no special setup is needed)
}


#define kButtonMsg 19

pascal OSErr main (short message, short caller,
	StringPtr objName, StringPtr zoneName,
	long p1, long p2) {
	
	// Did user press a button?
	if (message == kButtonMsg) {
		if ((p2 & 0xff) == 1) {
			// left button
			doPasswordDlg((ListHandle)p1);
		}

		if ((p2 & 0xff) == 2) {
			// right button
			aboutBox();
			doPasswordDlg((ListHandle)p1);
		}
	}
		
	return 0;
}

void aboutBox(void) {
	Alert(-4096, NULL);
}

void doPasswordDlg(ListHandle lst) {
	Cell listCell;
	Str255 pwd;
	Str255 ssid;
	short ssidLen = 255;
	ssid[0] = 4;
	ssid[1] = 's';
	ssid[2] = 's';
	ssid[3] = 'i';
	ssid[4] = 'd';
	
	if (lst) {
		SetPt(&listCell, 0, 0);
		if (LGetSelect(1, &listCell, lst)) {
			LGetCell(ssid+1, &ssidLen, listCell, lst);
			ssid[0] = ssidLen;
		}
	}
	
	ParamText(ssid, NULL, NULL, NULL);
	
	if(DoLoginDialog(pwd)) {
		sendConfiguration(ssid, pwd);
	}
}