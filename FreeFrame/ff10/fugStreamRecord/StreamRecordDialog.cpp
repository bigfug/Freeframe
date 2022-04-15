/////////////////////////////////////////////////////////////////////////////
// Name:        StreamRecordDialog.cpp
// Purpose:     
// Author:      Alex May
// Modified by: 
// Created:     08/10/06 16:00:44
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "StreamRecordDialog.h"
#endif

#include "stdafx.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

////@begin includes
////@end includes

#include "StreamRecordDialog.h"

////@begin XPM images
////@end XPM images

/*!
 * StreamRecordDialog type definition
 */

IMPLEMENT_DYNAMIC_CLASS( StreamRecordDialog, wxDialog )

/*!
 * StreamRecordDialog event table definition
 */

BEGIN_EVENT_TABLE( StreamRecordDialog, wxDialog )

////@begin StreamRecordDialog event table entries
    EVT_CLOSE( StreamRecordDialog::OnCloseWindow )

    EVT_TOGGLEBUTTON( ID_OUTPUT_LOCK_BUTTON, StreamRecordDialog::OnOutputLockButtonClick )

    EVT_SPINCTRL( ID_SPINCTRL, StreamRecordDialog::OnPortUpdated )
    EVT_TEXT( ID_SPINCTRL, StreamRecordDialog::OnPortTextUpdated )

    EVT_BUTTON( ID_BUTTON1, StreamRecordDialog::OnFileNameBrowseClick )

    EVT_TOGGLEBUTTON( ID_TOGGLEBUTTON, StreamRecordDialog::OnRecordClick )

    EVT_BUTTON( ID_ABOUT_BUTTON, StreamRecordDialog::OnAboutButtonClick )

////@end StreamRecordDialog event table entries

	EVT_TIMER( wxID_ANY, StreamRecordDialog::OnTimer )
END_EVENT_TABLE()

/*!
 * StreamRecordDialog constructors
 */

StreamRecordDialog::StreamRecordDialog( )
{
}

StreamRecordDialog::StreamRecordDialog( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Create(parent, id, caption, pos, size, style);
}

/*!
 * StreamRecordDialog creator
 */

bool StreamRecordDialog::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
////@begin StreamRecordDialog member initialisation
    mVidWidth = NULL;
    mVidHeight = NULL;
    mVidDepth = NULL;
    mVidFps = NULL;
    mOutputLockButton = NULL;
    mPort = NULL;
    mInputWidthText = NULL;
    mInputHeightText = NULL;
    mInputDepthText = NULL;
    mInputFpsText = NULL;
    mFilename = NULL;
    mFileBrowseButton = NULL;
    mRecord = NULL;
////@end StreamRecordDialog member initialisation

////@begin StreamRecordDialog creation
    SetExtraStyle(wxWS_EX_BLOCK_EVENTS);
    wxDialog::Create( parent, id, caption, pos, size, style );

    CreateControls();
    if (GetSizer())
    {
        GetSizer()->SetSizeHints(this);
    }
    Centre();
////@end StreamRecordDialog creation
    return true;
}

/*!
 * Control creation for StreamRecordDialog
 */

void StreamRecordDialog::CreateControls()
{    
////@begin StreamRecordDialog content construction
    StreamRecordDialog* itemDialog1 = this;

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemDialog1->SetSizer(itemBoxSizer2);

    wxStaticBox* itemStaticBoxSizer3Static = new wxStaticBox(itemDialog1, wxID_ANY, _("1. Specify AVI output format"));
    wxStaticBoxSizer* itemStaticBoxSizer3 = new wxStaticBoxSizer(itemStaticBoxSizer3Static, wxVERTICAL);
    itemBoxSizer2->Add(itemStaticBoxSizer3, 0, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer4 = new wxFlexGridSizer(3, 4, 0, 0);
    itemStaticBoxSizer3->Add(itemFlexGridSizer4, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText5 = new wxStaticText( itemDialog1, wxID_STATIC, _("Width:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer4->Add(itemStaticText5, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    mVidWidth = new wxSpinCtrl( itemDialog1, ID_SPINCTRL2, _T("320"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 16, 1024, 320 );
    itemFlexGridSizer4->Add(mVidWidth, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText7 = new wxStaticText( itemDialog1, wxID_STATIC, _("Height:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer4->Add(itemStaticText7, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    mVidHeight = new wxSpinCtrl( itemDialog1, ID_SPINCTRL3, _T("240"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 16, 768, 240 );
    itemFlexGridSizer4->Add(mVidHeight, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText9 = new wxStaticText( itemDialog1, wxID_STATIC, _("Depth:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer4->Add(itemStaticText9, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxString mVidDepthStrings[] = {
        _("16 bit"),
        _("24 bit"),
        _("32 bit")
    };
    mVidDepth = new wxChoice( itemDialog1, ID_CHOICE, wxDefaultPosition, wxDefaultSize, 3, mVidDepthStrings, 0 );
    mVidDepth->SetStringSelection(_("32 bit"));
    itemFlexGridSizer4->Add(mVidDepth, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxStaticText* itemStaticText11 = new wxStaticText( itemDialog1, wxID_STATIC, _("FPS:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer4->Add(itemStaticText11, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    mVidFps = new wxSpinCtrl( itemDialog1, ID_SPINCTRL5, _T("15"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 50, 15 );
    itemFlexGridSizer4->Add(mVidFps, 0, wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    mOutputLockButton = new wxToggleButton( itemDialog1, ID_OUTPUT_LOCK_BUTTON, _("Lock"), wxDefaultPosition, wxDefaultSize, 0 );
    mOutputLockButton->SetValue(false);
    itemStaticBoxSizer3->Add(mOutputLockButton, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticBox* itemStaticBoxSizer14Static = new wxStaticBox(itemDialog1, wxID_ANY, _("2. Specify streaming receive port"));
    wxStaticBoxSizer* itemStaticBoxSizer14 = new wxStaticBoxSizer(itemStaticBoxSizer14Static, wxVERTICAL);
    itemBoxSizer2->Add(itemStaticBoxSizer14, 0, wxGROW|wxALL, 5);

    wxFlexGridSizer* itemFlexGridSizer15 = new wxFlexGridSizer(3, 4, 0, 0);
    itemStaticBoxSizer14->Add(itemFlexGridSizer15, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText16 = new wxStaticText( itemDialog1, wxID_STATIC, _("Port:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemFlexGridSizer15->Add(itemStaticText16, 0, wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    mPort = new wxSpinCtrl( itemDialog1, ID_SPINCTRL, _T("30000"), wxDefaultPosition, wxDefaultSize, wxSP_ARROW_KEYS, 1, 65534, 30000 );
    itemFlexGridSizer15->Add(mPort, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer18 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer14->Add(itemBoxSizer18, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText19 = new wxStaticText( itemDialog1, wxID_STATIC, _("Width:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer18->Add(itemStaticText19, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    mInputWidthText = new wxStaticText( itemDialog1, wxID_STATIC, _("N/A"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer18->Add(mInputWidthText, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticText* itemStaticText21 = new wxStaticText( itemDialog1, wxID_STATIC, _("Height:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer18->Add(itemStaticText21, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    mInputHeightText = new wxStaticText( itemDialog1, wxID_STATIC, _("N/A"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer18->Add(mInputHeightText, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticText* itemStaticText23 = new wxStaticText( itemDialog1, wxID_STATIC, _("Depth:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer18->Add(itemStaticText23, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    mInputDepthText = new wxStaticText( itemDialog1, wxID_STATIC, _("N/A"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer18->Add(mInputDepthText, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticText* itemStaticText25 = new wxStaticText( itemDialog1, wxID_STATIC, _("FPS:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer18->Add(itemStaticText25, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    mInputFpsText = new wxStaticText( itemDialog1, wxID_STATIC, _("0"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer18->Add(mInputFpsText, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    wxStaticBox* itemStaticBoxSizer27Static = new wxStaticBox(itemDialog1, wxID_ANY, _("3. Set output file"));
    wxStaticBoxSizer* itemStaticBoxSizer27 = new wxStaticBoxSizer(itemStaticBoxSizer27Static, wxVERTICAL);
    itemBoxSizer2->Add(itemStaticBoxSizer27, 0, wxGROW|wxALL, 5);

    wxBoxSizer* itemBoxSizer28 = new wxBoxSizer(wxHORIZONTAL);
    itemStaticBoxSizer27->Add(itemBoxSizer28, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    wxStaticText* itemStaticText29 = new wxStaticText( itemDialog1, wxID_STATIC, _("Output File:"), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer28->Add(itemStaticText29, 0, wxALIGN_CENTER_VERTICAL|wxALL|wxADJUST_MINSIZE, 5);

    mFilename = new wxTextCtrl( itemDialog1, ID_TEXTCTRL, _T(""), wxDefaultPosition, wxSize(200, -1), 0 );
    itemBoxSizer28->Add(mFilename, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    mFileBrowseButton = new wxButton( itemDialog1, ID_BUTTON1, _("Browse..."), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer28->Add(mFileBrowseButton, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxBoxSizer* itemBoxSizer32 = new wxBoxSizer(wxHORIZONTAL);
    itemBoxSizer2->Add(itemBoxSizer32, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5);

    mRecord = new wxToggleButton( itemDialog1, ID_TOGGLEBUTTON, _("Record"), wxDefaultPosition, wxDefaultSize, 0 );
    mRecord->SetValue(false);
    mRecord->Enable(false);
    itemBoxSizer32->Add(mRecord, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

    wxButton* itemButton34 = new wxButton( itemDialog1, ID_ABOUT_BUTTON, _("About..."), wxDefaultPosition, wxDefaultSize, 0 );
    itemBoxSizer32->Add(itemButton34, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

////@end StreamRecordDialog content construction
}

/*!
 * Should we show tooltips?
 */

bool StreamRecordDialog::ShowToolTips()
{
    return true;
}

/*!
 * Get bitmap resources
 */

wxBitmap StreamRecordDialog::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin StreamRecordDialog bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end StreamRecordDialog bitmap retrieval
}

/*!
 * Get icon resources
 */

wxIcon StreamRecordDialog::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin StreamRecordDialog icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end StreamRecordDialog icon retrieval
}
/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_OUTPUT_LOCK_BUTTON
 */

void StreamRecordDialog::OnOutputLockButtonClick( wxCommandEvent& event )
{
	if( this->mOutputLockButton->GetValue() )
	{
		if( ( mWsaError = WSAStartup( MAKEWORD( 2, 2 ), &mWsaData ) ) != 0 )
		{
			return;
		}

		this->mVidWidth->Disable();
		this->mVidHeight->Disable();
		this->mVidDepth->Disable();
		this->mVidFps->Disable();

		//this->mPort->Enable();

		//this->mFileBrowseButton->Enable();
		//this->mFilename->Enable();

		if( this->mFilename->GetLineLength( 0 ) > 0 )
		{
			this->mRecord->Enable();
		}

		this->mAviResult = S_FALSE;

		int Depth;

		switch( mVidDepth->GetSelection() )
		{
			case 0:
				Depth = FF_CAP_16BITVIDEO;
				break;

			case 1:
				Depth = FF_CAP_24BITVIDEO;
				break;

			case 2:
				Depth = FF_CAP_32BITVIDEO;
				break;
		}

		VideoInfoStruct		VideoInfo;

		memset( &VideoInfo, 0, sizeof( VideoInfo ) );

		VideoInfo.frameWidth  = mVidWidth->GetValue();
		VideoInfo.frameHeight = mVidHeight->GetValue();
		VideoInfo.bitDepth    = Depth;

		if( ( mDecoder = StreamDecoderFactory::getInstance( STREAM_MEM, VideoInfo ) ) == NULL )
		{
			return;
		}

		struct sockaddr_in		Addr;

		Addr.sin_port = htons( mPort->GetValue() );

		mDecoder->setSrcAddr( &Addr );

		this->mTimer.SetOwner( this );
		this->mTimer.Start( 200 );

		mLastFpsTime = time( NULL );
		mFpsCount    = mDecoder->getFrameNumber();

		if( timeGetDevCaps( &mTimeCaps, sizeof( mTimeCaps ) ) != TIMERR_NOERROR )
		{
			return;
		}

		if( timeBeginPeriod( mTimeCaps.wPeriodMin ) != TIMERR_NOERROR )
		{
			return;
		}

		mLastFrameTime = NULL;
		mFramePeriod = 1000 / mVidFps->GetValue();

		if( ( mTimerId = timeSetEvent( mTimeCaps.wPeriodMin, 1, &StreamRecordDialog::StaticTimeProc, (DWORD_PTR)this, TIME_PERIODIC | TIME_CALLBACK_FUNCTION | TIME_KILL_SYNCHRONOUS ) ) == NULL )
		{
		}
	}
	else
	{
		timeKillEvent( mTimerId );

		mTimerId = NULL;

		timeEndPeriod( mTimeCaps.wPeriodMin );

		SAFE_DELETE( mDecoder );

		this->mTimer.Stop();

		this->mVidWidth->Enable();
		this->mVidHeight->Enable();
		this->mVidDepth->Enable();
		this->mVidFps->Enable();

		//this->mPort->Disable();

		//this->mFileBrowseButton->Disable();
		//this->mFilename->Disable();

		this->mRecord->Disable();

		WSACleanup();
	}
}


/*!
 * wxEVT_COMMAND_SPINCTRL_UPDATED event handler for ID_SPINCTRL
 */

void StreamRecordDialog::OnPortUpdated( wxSpinEvent& event )
{
}

/*!
 * wxEVT_COMMAND_TEXT_UPDATED event handler for ID_SPINCTRL
 */

void StreamRecordDialog::OnPortTextUpdated( wxCommandEvent& event )
{
}


void StreamRecordDialog::OnTimer( wxTimerEvent &event )
{
	this->mInputWidthText->SetLabel( mDecoder->getRecvWidth() == NULL ? _( "N/A" ) : wxString::Format( _( "%d" ), mDecoder->getRecvWidth() ) );
	this->mInputHeightText->SetLabel( mDecoder->getRecvHeight() == NULL ? _( "N/A" ) : wxString::Format( _( "%d" ), mDecoder->getRecvHeight() ) );
	this->mInputDepthText->SetLabel( mDecoder->getRecvDepth() == NULL ? _( "N/A" ) : wxString::Format( _( "%d" ), mDecoder->getRecvDepth() ) );

	time_t	CurrFpsTime = time( NULL );

	if( CurrFpsTime != mLastFpsTime )
	{
		this->mInputFpsText->SetLabel( wxString::Format( _( "%d" ), mFrameCount ) );

		mFrameCount = 0;

		mLastFpsTime = CurrFpsTime;
	}
}
/*!
 * wxEVT_CLOSE_WINDOW event handler for ID_DIALOG
 */

void StreamRecordDialog::OnCloseWindow( wxCloseEvent& event )
{
	if( this->mRecord->GetValue() )
	{
		return;
	}

	if( this->mOutputLockButton->GetValue() )
	{
		timeKillEvent( mTimerId );

		mTimerId = NULL;

		timeEndPeriod( mTimeCaps.wPeriodMin );

		SAFE_DELETE( mDecoder );

		this->mTimer.Stop();

		WSACleanup();
	}

	this->Destroy();
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_BUTTON1
 */

void StreamRecordDialog::OnFileNameBrowseClick( wxCommandEvent& event )
{
	wxFileDialog		Dialog( this, _( "Save AVI as..." ), wxEmptyString, wxEmptyString, _( "*.avi" ), wxSAVE | wxOVERWRITE_PROMPT );

	if( Dialog.ShowModal() == wxID_OK )
	{
		this->mFilename->SetValue( Dialog.GetPath() );
	}

	if( this->mOutputLockButton->GetValue() && this->mFilename->GetLineLength( 0 ) > 0 )
	{
		this->mRecord->Enable();
	}
	else
	{
		this->mRecord->Disable();
	}
}


/*!
 * wxEVT_COMMAND_CHECKBOX_CLICKED event handler for ID_TOGGLEBUTTON
 */

void StreamRecordDialog::OnRecordClick( wxCommandEvent& event )
{
	if( mRecord->GetValue() )
	{
		BITMAPINFOHEADER		BitMapInfo;

		this->mOutputLockButton->Disable();

		this->mPort->Disable();

		this->mFileBrowseButton->Disable();
		this->mFilename->Disable();

		memset( &BitMapInfo, 0, sizeof(BITMAPINFOHEADER) );

		switch( mVidDepth->GetSelection() )
		{
			case 0:
				BitMapInfo.biBitCount = 16;
				BitMapInfo.biSizeImage = 2;
				break;

			case 1:
				BitMapInfo.biBitCount = 24;
				BitMapInfo.biSizeImage = 3;
				break;

			case 2:
				BitMapInfo.biBitCount = 32;
				BitMapInfo.biSizeImage = 4;
				break;
		}

		BitMapInfo.biSize        = sizeof(BITMAPINFOHEADER);
		BitMapInfo.biHeight      = mVidHeight->GetValue();
		BitMapInfo.biPlanes      = 1;
		BitMapInfo.biSizeImage  *= ( mVidWidth->GetValue() * mVidHeight->GetValue() );
		BitMapInfo.biWidth       = mVidWidth->GetValue();
		BitMapInfo.biCompression = BI_RGB;

		mAVI.SetBitmapHeader( &BitMapInfo );
		mAVI.SetFileName( this->mFilename->GetValue() );
		mAVI.SetRate( this->mVidFps->GetValue() );

		if( ( mAviResult = mAVI.InitEngine() ) != AVIERR_OK )
		{
			mAVI.ReleaseEngine();

			MessageBox( NULL, mAVI.GetLastErrorMessage(), _( "Stream Record Error" ), MB_OK );

			mRecord->SetValue( false );
		}
	}
	else
	{
		mAviResult = S_FALSE;

		mAVI.ReleaseEngine();

		this->mPort->Enable();

		this->mFileBrowseButton->Enable();
		this->mFilename->Enable();

		this->mOutputLockButton->Enable();
	}
}


/*!
 * wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ABOUT_BUTTON
 */

void StreamRecordDialog::OnAboutButtonClick( wxCommandEvent& event )
{
////@begin wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ABOUT_BUTTON in StreamRecordDialog.
    // Before editing this code, remove the block markers.
    wxMessageBox(_("StreamRecord v1.0\n\nBy Alex May\n\nhttp://www.bigfug.com"));
////@end wxEVT_COMMAND_BUTTON_CLICKED event handler for ID_ABOUT_BUTTON in StreamRecordDialog. 
}


void StreamRecordDialog::StaticTimeProc( UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2 )
{
	((StreamRecordDialog *)dwUser)->TimeProc( uID, uMsg, dw1, dw2 );
}

void StreamRecordDialog::TimeProc( UINT uID, UINT uMsg, DWORD dw1, DWORD dw2 )
{
	DWORD			CurrTime;

	if( mDecoder->hasNewFrame() )
	{
		mFrameCount++;
	}

	CurrTime = timeGetTime();

	if( mLastFrameTime == NULL || CurrTime >= mLastFrameTime + mFramePeriod )
	{
		mLastFrameTime = CurrTime;

		if( mAviResult == AVIERR_OK )
		{
			BYTE	*ReadPtr;

			if( ( ReadPtr = mDecoder->lockReadBuffer() ) != NULL )
			{
				this->mAVI.AddFrame( ReadPtr );

				mDecoder->unlockReadBuffer();
			}
		}
	}
}
