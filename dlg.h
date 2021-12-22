#ifndef DLG_H
#define DLG_H

/* dlg.{h,c} contain the functions that do user interface stuff; they manage the
   login dialog and internal error signalling. */

int DoLoginDialog (StringPtr password);
void InternalErrorDialog(Str255 errText, OSErr err);
pascal Boolean	TwoItemFilter(DialogPtr dlog,EventRecord *event,short *itemHit);

#endif