#include <MacTypes.h>

/* net.{c,h} contain functions that send stuff out to the network or
   receive stuff from it. */

// sendConfiguration sends an ssid and a password to any airtalk that
// happens to be listening.
void sendConfiguration(Str255 ssid, Str255 pwd);
