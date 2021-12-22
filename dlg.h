#ifndef DLG_H
#define DLG_H

int DoLoginDialog (StringPtr password);
void InternalErrorDialog(Str255 errText, OSErr err);
pascal Boolean	TwoItemFilter(DialogPtr dlog,EventRecord *event,short *itemHit);

#endif