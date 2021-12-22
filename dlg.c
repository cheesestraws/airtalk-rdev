#include <Types.h>
#include <Memory.h>
#include <Resources.h>
#include <OSUtils.h>
#include <Quickdraw.h>
#include <Fonts.h>
#include <Events.h>
#include <Windows.h>
#include <Menus.h>
#include <Dialogs.h>
#include <TextEdit.h>
#include <Traps.h>

#include <strings.h>

#include "dlg.h"
#include "util.h"

/* Password dialog box stolen wholesale from a DTS example */

#define kRevealKeyChk 4
#define kOpenRadio 7
#define kEncRadio 8
#define kKeyBox 2
#define kKeyLabel 11
#define kCancelBtn 12 // yes this should be 2 let us never speak of this again

ControlHandle GetCtlHandle(DialogPtr dlog, short item)
{
    short itemtype;
    Rect r;
    Handle thandle;
    
    GetDialogItem(dlog, item, &itemtype, &thandle, &r);
    return ((ControlHandle)thandle);
}


void setKeyVisibility(DialogPtr dlog, int vis) {
	Point			size;

	size.v = dlog->portRect.bottom - dlog->portRect.top;
	size.h = dlog->portRect.right - dlog->portRect.left;
	if (vis)
		size.v += 35;
	else
		size.v -= 35;
	
	SizeWindow(dlog,size.h,size.v,true);		// Resize window
}


// returns 1 if the user chose OK, 0 for cancel
int DoLoginDialog (StringPtr password)
{
	DialogPtr		dlog;
	Handle			itemH;
	ControlHandle	chkBox;
	short			item,itemType,chkVal;
	Rect			box;
	Boolean			newDialogs;

	ModalFilterUPP twoItemFilterUPP;
	
	/* set up a UPP for the dialog filter */
	twoItemFilterUPP = NewModalFilterProc(TwoItemFilter);
	
	dlog = GetNewDialog(-4095,0L,(WindowPtr) -1L);
	chkBox = GetCtlHandle(dlog, kEncRadio);
	SetControlValue(chkBox,1);
	
	if (dlog) {
		// Set up default and cancel items, if we have the new Dialog Manager
		if(trapAvailable(_DialogDispatch)) {
			SetDialogDefaultItem(dlog, ok);
			SetDialogCancelItem(dlog, kCancelBtn);
		}
	
		do {	
			ModalDialog(twoItemFilterUPP,&item);
			if (item == kRevealKeyChk) {
				chkBox = GetCtlHandle(dlog, kRevealKeyChk);
				chkVal = !GetControlValue(chkBox);
				SetControlValue(chkBox,chkVal);
				setKeyVisibility(dlog, chkVal);
			}
			
			if (item == kOpenRadio) {
				chkBox = GetCtlHandle(dlog, kOpenRadio);
				SetControlValue(chkBox,1);
				chkBox = GetCtlHandle(dlog, kEncRadio);
				SetControlValue(chkBox,0);
				
				// hide the key UI if an open network is selected
				HideDialogItem(dlog, kKeyBox);
				HideDialogItem(dlog, kKeyLabel);
				HideDialogItem(dlog, kRevealKeyChk);
				
				// collapse the visible key, if we need to
				chkBox = GetCtlHandle(dlog, kRevealKeyChk);
				chkVal = GetControlValue(chkBox);
				if (chkVal) {
					SetControlValue(chkBox,0);
					setKeyVisibility(dlog, 0);
				}
			}
			
			if (item == kEncRadio) {
				chkBox = GetCtlHandle(dlog, kOpenRadio);
				SetControlValue(chkBox,0);
				chkBox = GetCtlHandle(dlog, kEncRadio);
				SetControlValue(chkBox,1);
				ShowDialogItem(dlog, kKeyBox);
				ShowDialogItem(dlog, kKeyLabel);
				ShowDialogItem(dlog, kRevealKeyChk);
			}
		} while (item != 1 && item != 12);			// Until the OK button is hit
	
		GetDialogItem(dlog,3,&itemType,&itemH,&box);		// Get text from hidden dialog item
		GetDialogItemText(itemH, password);
		
		DisposeDialog(dlog);
		
		return (item == 1);
	}
}

pascal Boolean
TwoItemFilter(DialogPtr dlog,EventRecord *event,short *itemHit)
{	DialogPtr	evtDlog;
	short		selStart,selEnd;
	unsigned long dummyLong;
	ModalFilterUPP nextFilter;
	OSErr err;
	
	if (event->what == keyDown || event->what == autoKey) {	
		switch (event->message & charCodeMask) {	
			case '\r':
			case '\n':			// Return  (hitting return or enter is the same as hitting the OK button)
			case '\003':		// Enter
				HiliteControl(GetCtlHandle(dlog, ok), 10);
				Delay(8, &dummyLong);     
				HiliteControl(GetCtlHandle(dlog, ok), false);
				
				*itemHit = 1;		// OK Button
				return true;		// We handled the event
			case 0x1b:
				HiliteControl(GetCtlHandle(dlog, kCancelBtn), 10);
				Delay(8, &dummyLong);     
				HiliteControl(GetCtlHandle(dlog, kCancelBtn), false);
				
				*itemHit = 12;		// OK Button
				return true;		// We handled the event
			case '\t':			// Tab
				event->what = nullEvent;	// Do nothing (don't let the user tab to the hidden field)
				return false;
			case '\034':		// Left arrow  (Keys that just change the selection)
			case '\035':		// Right arrow
			case '\036':		// Up arrow
			case '\037':		// Down arrow
				return false;			// Let ModalDialog handle them
			default:
				selStart = (**((DialogPeek)dlog)->textH).selStart;		// Get the selection in the visible item
				selEnd = (**((DialogPeek)dlog)->textH).selEnd;
				SelectDialogItemText(dlog,3,selStart,selEnd);				// Select text in invisible item
				DialogSelect(event,&evtDlog,itemHit);			// Input key
				SelectDialogItemText(dlog,2,selStart,selEnd);				// Select same area in visible item
				if ((event->message & charCodeMask) != '\010')	// If it's not a backspace (backspace is the only key that can affect both the text and the selection- thus we need to process it in both fields, but not change it for the hidden field.
					event->message = '¥';						// Replace with character to use
				DialogSelect(event,&evtDlog,itemHit);			// Put in fake character
				return true;
		}
	} else {
		if (trapAvailable(_DialogDispatch)) {
			err = GetStdFilterProc(&nextFilter);
			if (err == noErr) {
				return CallModalFilterProc(nextFilter, dlog, event, itemHit);
			}
		}
	}
}

void InternalErrorDialog(Str255 errText, OSErr error) {
	DialogPtr		dlog;
	short item;
	ControlHandle textItemHandle;
	Str255 errNumber;
	Str255 errorMessage;
	
	NumToString(error, errNumber);
	getErrorString(error, errorMessage);
	ParamText(errText, errorMessage, errNumber, NULL);
	StopAlert(kInternalErrorAlert, NULL);
}
