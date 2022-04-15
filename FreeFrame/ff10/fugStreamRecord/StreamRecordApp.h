/////////////////////////////////////////////////////////////////////////////
// Name:        StreamRecordApp.h
// Purpose:     
// Author:      Alex May
// Modified by: 
// Created:     08/10/06 15:50:00
// RCS-ID:      
// Copyright:   
// Licence:     
/////////////////////////////////////////////////////////////////////////////

#ifndef _STREAMRECORDAPP_H_
#define _STREAMRECORDAPP_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "StreamRecordApp.cpp"
#endif

/*!
 * Includes
 */

////@begin includes
#include "wx/image.h"
#include "StreamRecordDialog.h"
////@end includes

/*!
 * Forward declarations
 */

////@begin forward declarations
////@end forward declarations

/*!
 * Control identifiers
 */

////@begin control identifiers
////@end control identifiers

/*!
 * StreamRecordApp class declaration
 */

class StreamRecordApp: public wxApp
{    
    DECLARE_CLASS( StreamRecordApp )
    DECLARE_EVENT_TABLE()

public:
    /// Constructor
    StreamRecordApp();

    /// Initialises the application
    virtual bool OnInit();

    /// Called on exit
    virtual int OnExit();

////@begin StreamRecordApp event handler declarations

////@end StreamRecordApp event handler declarations

////@begin StreamRecordApp member function declarations

////@end StreamRecordApp member function declarations

////@begin StreamRecordApp member variables
////@end StreamRecordApp member variables
};

/*!
 * Application instance declaration 
 */

////@begin declare app
DECLARE_APP(StreamRecordApp)
////@end declare app

#endif
    // _STREAMRECORDAPP_H_
