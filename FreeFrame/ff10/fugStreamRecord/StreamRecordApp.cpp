/////////////////////////////////////////////////////////////////////////////
// Name:        StreamRecordApp.cpp
// Purpose:     
// Author:      Alex May
// Modified by: 
// Created:     08/10/06 15:50:00
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma implementation "StreamRecordApp.h"
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

#include "StreamRecordApp.h"

////@begin XPM images
////@end XPM images

/*!
 * Application instance implementation
 */

////@begin implement app
IMPLEMENT_APP( StreamRecordApp )
////@end implement app

/*!
 * StreamRecordApp type definition
 */

IMPLEMENT_CLASS( StreamRecordApp, wxApp )

/*!
 * StreamRecordApp event table definition
 */

BEGIN_EVENT_TABLE( StreamRecordApp, wxApp )

////@begin StreamRecordApp event table entries
////@end StreamRecordApp event table entries

END_EVENT_TABLE()

/*!
 * Constructor for StreamRecordApp
 */

StreamRecordApp::StreamRecordApp()
{
////@begin StreamRecordApp member initialisation
////@end StreamRecordApp member initialisation
}

/*!
 * Initialisation for StreamRecordApp
 */

bool StreamRecordApp::OnInit()
{    
////@begin StreamRecordApp initialisation
    // Remove the comment markers above and below this block
    // to make permanent changes to the code.

#if wxUSE_XPM
    wxImage::AddHandler( new wxXPMHandler );
#endif
#if wxUSE_LIBPNG
    wxImage::AddHandler( new wxPNGHandler );
#endif
#if wxUSE_LIBJPEG
    wxImage::AddHandler( new wxJPEGHandler );
#endif
#if wxUSE_GIF
    wxImage::AddHandler( new wxGIFHandler );
#endif
    StreamRecordDialog* mainWindow = new StreamRecordDialog(NULL, ID_DIALOG, _("StreamRecord"));
    int returnValue = mainWindow->ShowModal();

    //mainWindow->Destroy();
    // A modal dialog application should return false to terminate the app.
    return false;
////@end StreamRecordApp initialisation

    return true;
}

/*!
 * Cleanup for StreamRecordApp
 */
int StreamRecordApp::OnExit()
{    
////@begin StreamRecordApp cleanup
    return wxApp::OnExit();
////@end StreamRecordApp cleanup
}

