#include "net.h"

#include <MacTypes.h>
#include <AppleTalk.h>
#include <Devices.h>
#include <Sound.h>

#include "util.h"

void sendConfiguration(Str255 ssid, Str255 pwd) {
	// To configure the AirTalk, we send a broadcast ATP packet out
	// on the localtalk segment.  The AirTalk intercepts this and loads the
	// configuration out from it.  We intentionally subvert an Apple-reserved
	// socket number, because it has well-defined semantics and it's unlikely to
	// collide with user software.
	//
	// We use ATP here rather than just a DDP packet because using DDP 
	// directly requires the use of assembler, and I'm too tired for that.

	Str255 driverName;
	Str255 errorMessage;
	OSErr err;
	short mpp;
	Str255 cmd;

	ATPParamBlock atp = { 0 };
	BDSType bds;
	char respBuffer[578];
	int nBufs;
	char payload[255] = { 0 };
	int cursor;
	
	// Todo: validate ssid and pwd
	
	// First, open the AppleTalk driver
	getRsrcStr(kMPPDriverName, driverName);
	err = OpenDriver(driverName,&mpp);
	if (err) {
		SysBeep(1);
		getRsrcStr(kMPPDriverError, errorMessage);

		InternalErrorDialog(errorMessage, err);
	}
	
	// Our packet payload starts with a defined string: this is partly
	// for future expansion and partly so that the AirTalk can reliably tell
	// the difference between this and an echo packet.
	getRsrcStr(kAirTalkCmdPrefix, cmd);
	
	// Set up the payload, consisting of the command prefix, then the ssid,
	// then the password, each as a Pascal string.
	cursor = 0;
	BlockMove(cmd, &payload[cursor], cmd[0]+1);
	cursor += cmd[0]+1;
	BlockMove(ssid, &payload[cursor], ssid[0]+1);
	cursor += ssid[0]+1;
	BlockMove(pwd, &payload[cursor], pwd[0]+1);
	cursor += pwd[0]+1;
	
	// Create a BDS.  A BDS is a data structure that will hold any response.
	// Since in this case we're not expecting a response at all, this is a bit
	// academic but we have to do it anyway.
	nBufs = BuildBDS((Ptr)respBuffer, (Ptr)bds, sizeof(respBuffer));
	
	// Now we set up the parameter block to send our ATP request.
	// It turns out ATP can in fact construct broadcast packets (why?), which
	// is useful for us.
	atp.SREQ.atpFlags = atpXOvalue;
	atp.SREQ.addrBlock.aNet = 0;
	atp.SREQ.addrBlock.aNode = 255;
	atp.SREQ.addrBlock.aSocket = 4;
	atp.SREQ.reqLength = cursor;
	atp.SREQ.reqPointer = payload;
	atp.SREQ.numOfBuffs = nBufs;
	atp.SREQ.bdsPointer = (Ptr)bds;
	atp.SREQ.timeOutVal = 1;
	atp.SREQ.retryCount = 1;
	
	err = PSendRequest(&atp, 0);
	if (err && err != -1096) {
		// and if it fails we just display an error and hope for the best
		SysBeep(1);
		getRsrcStr(kATPSendError, errorMessage);

		InternalErrorDialog(errorMessage, err);
	}
}
