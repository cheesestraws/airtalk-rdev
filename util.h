#include <MacTypes.h>

/* util.{h,c} contain a set of utility functions for dealing with resources
   and traps, mostly. */

// Resource constants
#define kMPPDriverError -4065
#define kMPPDriverName -4066
#define kATPOpenError -4067
#define kAirTalkCmdPrefix -4068
#define kATPSendError -4069

#define kInternalErrorAlert -4061

#define kErrorDictionaryID -4065

// getRsrcStr copies the contents of the 'STR ' resource with the given id
// into the destination string.
void getRsrcStr(short id, Str255 dest);

// getErrorString looks up an OSerror in the error message dictionary
// (in the singlet ERST resource) and copies the corresponding message into
// the destination string.
void getErrorString(OSErr error, Str255 dest);

// trapAvailable returns true if the trap given is available (use the macros
// in Traps.h).
Boolean trapAvailable(short theTrap);