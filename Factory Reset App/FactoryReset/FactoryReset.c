#include <stdio.h>
#include <stdlib.h>

#include <MacTypes.h>
#include <AppleTalk.h>
#include <Devices.h>
#include <Sound.h>

int clearConfiguration();

int clearConfiguration() {
	// To configure the AirTalk, we send a broadcast ATP packet out
	// on the localtalk segment.  The AirTalk intercepts this and loads the
	// configuration out from it.  We intentionally subvert an Apple-reserved
	// socket number, because it has well-defined semantics and it's unlikely to
	// collide with user software.
	//
	// We use ATP here rather than just a DDP packet because using DDP 
	// directly requires the use of assembler, and I'm too tired for that.

	OSErr err;
	short mpp;
	Str255 cmd = "\pairtalk setap";

	ATPParamBlock atp = { 0 };
	BDSType bds;
	char respBuffer[578];
	int nBufs;
	char payload[255] = { 0 };
	
	// First, open the AppleTalk driver
	err = OpenDriver("\p.MPP",&mpp);
	if (err) {
		SysBeep(1);
		printf("Couldn't load AppleTalk driver; something's gone badly wrong.  Sorry.\n");
		return 0;
	};
		
	// Set up the payload, consisting of the command prefix, then the ssid,
	// then the password, each as a Pascal string.  Here the AP and password are null
	// so we can just use the initialisation above.
	BlockMove(cmd, &payload, cmd[0]+1);
	
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
	atp.SREQ.reqLength = cmd[0]+1;
	atp.SREQ.reqPointer = payload;
	atp.SREQ.numOfBuffs = nBufs;
	atp.SREQ.bdsPointer = (Ptr)bds;
	atp.SREQ.timeOutVal = 1;
	atp.SREQ.retryCount = 1;
	
	err = PSendRequest(&atp, 0);
	if (err) {
		// and if it fails we just display an error and hope for the best
		SysBeep(1);

		printf("An error occurred talking to AirTalk: %d\n", err);
		return 0;
	}
	
	return 1;
}


int main(void)
{
	char inbuf[256];

	printf("This tool will factory reset any AirTalk that this computer is directly "
	       "plugged into by a serial cable or LocalTalk network.\n\n");
	printf("This means that it will be reset not to connect to any wireless network "
	       "and you will have to set it up again using the AirTalk Chooser extension"
	       " or HTTP-based setup.\n\n");
	       
	printf("Are you sure you want to continue? [y/n]\n");
	gets(inbuf);
	printf("\n");
	
	if (inbuf[0] != 121) {
		printf("Bye!\n\n");
		return 0;
	}

	return 0;
}

