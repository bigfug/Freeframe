/////////////////////////////////////////////////////////////////////////////
// Name:        StreamRecordDialog.h
// Purpose:     
// Author:      Alex May
// Modified by: 
// Created:     08/10/06 16:00:44
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _STREAMRECORDDIALOG_H_
#define _STREAMRECORDDIALOG_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "StreamRecordDialog.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/spinctrl.h"
#include "wx/tglbtn.h"
////@end includes

#include "StreamDecoderUDP.h"
#include "AVIGenerator.h"

/*!
 * Forward declarations
 */

////@begin forward declarations
class wxSpinCtrl;
class wxToggleButton;
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
#define ID_DIALOG 10000
#define ID_SPINCTRL2 10010
#define ID_SPINCTRL3 10011
#define ID_CHOICE 10001
#define ID_SPINCTRL5 10015
#define ID_OUTPUT_LOCK_BUTTON 10014
#define ID_SPINCTRL 10006
#define ID_TEXTCTRL 10003
#define ID_BUTTON1 10004
#define ID_TOGGLEBUTTON 10005
#define ID_ABOUT_BUTTON 10013
#define SYMBOL_STREAMRECORDDIALOG_STYLE wxCAPTION|wxSYSTEM_MENU|wxCLOSE_BOX|wxMINIMIZE_BOX|wxDIALOG_MODAL
#define SYMBOL_STREAMRECORDDIALOG_TITLE _("StreamRecord")
#define SYMBOL_STREAMRECORDDIALOG_IDNAME ID_DIALOG
#define SYMBOL_STREAMRECORDDIALOG_SIZE wxSize(400, 300)
#define SYMBOL_STREAMRECORDDIALOG_POSITION wxDefaultPosition
////@end control identifiers

/*!
 * Compatibility
 */

#ifndef wxCLOSE_BOX
#define wxCLOSE_BOX 0x1000
#endif
#ifndef wxFIXED_MINSIZE
#define wxFIXED_MINSIZE 0
#endif

/*!
 * StreamRecordDialog class declaration
 */

class StreamRecordDialog: public wxDialog
{    
    DECLARE_DYNAMIC_CLASS( StreamRecordDialog )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    StreamRecordDialog( );
    StreamRecordDialog( wxWindow* parent, wxWindowID id = SYMBOL_STREAMRECORDDIALOG_IDNAME, const wxString& caption = SYMBOL_STREAMRECORDDIALOG_TITLE, const wxPoint& pos = SYMBOL_STREAMRECORDDIALOG_POSITION, const wxSize& size = SYMBOL_STREAMRECORDDIALOG_SIZE, long style = SYMBOL_STREAMRECORDDIALOG_STYLE );

    /// Creation
    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_STREAMRECORDDIALOG_IDNAME, const wxString& caption = SYMBOL_STREAMRECORDDIALOG_TITLE, const wxPoint& pos = SYMBOL_STREAMRECORDDIALOG_POSITION, const wxSize& size = SYMBOL_STREAMRECORDDIALOG_SIZE, long style = SYMBOL_STREAMRECORDDIALOG_STYLE );

    /// Creates the controls and sizers
    void CreateControls();

////@begin StreamRecordDialog event handler declarations

    /// wxEVT_CLOSE_WINDOW event handler for ID_DIALOG
    void OnCloseWindow( wxCloseEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_OUTPUT_LOCK_BUTTON
    void OnOutputLockButtonClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_SPINCTRL_UPDATED event handler for ID_SPINCTRL
    void OnPortUpdated( wxSpinEvent& event );

    /// wxEVT_COMMAND_TEXT_UPDATED event handler for ID_SPINCTRL
    void OnPortTextUpdated( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON1
    void OnFileNameBrowseClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_TOGGLEBUTTON
    void OnRecordClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ABOUT_BUTTON
    void OnAboutButtonClick( wxCommandEvent& event );

////@end StreamRecordDialog event handler declarations

	void StreamRecordDialog::OnTimer( wxTimerEvent &event );

////@begin StreamRecordDialog member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end StreamRecordDialog member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin StreamRecordDialog member variables
    wxSpinCtrl* mVidWidth;
    wxSpinCtrl* mVidHeight;
    wxChoice* mVidDepth;
    wxSpinCtrl* mVidFps;
    wxToggleButton* mOutputLockButton;
    wxSpinCtrl* mPort;
    wxStaticText* mInputWidthText;
    wxStaticText* mInputHeightText;
    wxStaticText* mInputDepthText;
    wxStaticText* mInputFpsText;
    wxTextCtrl* mFilename;
    wxButton* mFileBrowseButton;
    wxToggleButton* mRecord;
////@end StreamRecordDialog member variables

	WSADATA					 mWsaData;
	int						 mWsaError;
	StreamDecoder			*mDecoder;
	wxTimer					 mTimer;
	time_t					 mLastFpsTime;
	int						 mFpsCount;
	CAVIGenerator			 mAVI;
	HRESULT					 mAviResult;
	int						 mFrameCount;
	TIMECAPS				 mTimeCaps;
	MMRESULT				 mTimerId;
	DWORD					 mLastFrameTime;
	DWORD					 mFramePeriod;

	static void CALLBACK StaticTimeProc( UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2 );
	void TimeProc( UINT uID, UINT uMsg, DWORD dw1, DWORD dw2 );
};

#endif
    // _STREAMRECORDDIALOG_H_
