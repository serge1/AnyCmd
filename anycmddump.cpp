/*
ELFDump.cpp - Dump ELF file using ELFIO library.

Copyright (C) 2001-2011 by Serge Lamikhov-Center

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifdef _MSC_VER
#define _SCL_SECURE_NO_WARNINGS
#endif

#define __STDC_FORMAT_MACROS

#define BUFSIZE 4096

#include <windows.h>
#include <string>

#include "resource.h"

extern HINSTANCE hinst;
extern HWND      listWin;
extern char      command_string[];

std::string CreateChildProcess( const char* cmd );
//std::string ReadFromPipe(void);

PROCESS_INFORMATION piProcInfo; 

HANDLE g_hChildStd_OUT_Rd = 0;
HANDLE g_hChildStd_OUT_Wr = 0;


std::string receive_text( const char* cmd )
{
    SECURITY_ATTRIBUTES saAttr; 

    saAttr.nLength              = sizeof(SECURITY_ATTRIBUTES); 
    saAttr.bInheritHandle       = TRUE; 
    saAttr.lpSecurityDescriptor = 0; 

    // Create a pipe for the child process's STDOUT
    if ( ! CreatePipe( &g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0 ) ) {
        return "Error on CreatePipe"; 
    }
    // Ensure the read handle to the pipe for STDOUT is not inherited.
    if ( ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) ) {
        return "Stdout SetHandleInformation";
    }

    // Create the child process. 
    std::string ret = CreateChildProcess( cmd );

    // Read from pipe that is the standard output for child process. 
    //    ReadFromPipe(); 

    return ret;
}


BOOL CALLBACK ToolDlgProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg ) {
    case WM_COMMAND:
        switch( LOWORD( wParam ) ) {
        case IDCANCEL:
            TerminateProcess( piProcInfo.hProcess, 9 );
            break;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}


std::string res;


DWORD WINAPI Reader( LPVOID lpParameter )
{
    DWORD       dwRead; 
    CHAR        chBuf[BUFSIZE]; 
    BOOL        bSuccess1 = TRUE;

    // Read output from the child process's pipe for STDOUT
    // Stop when there is no more data. 
    bSuccess1 = TRUE;
    while (bSuccess1) 
    { 
        bSuccess1 = ReadFile( g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL );
        if( ! bSuccess1 || dwRead == 0 ) {
            break; 
        }
        res += std::string( chBuf, dwRead );
    }

    return 0;
}


std::string CreateChildProcess( const char* cmd )
    // Create a child process that uses the previously created pipes for STDIN and STDOUT.
{ 
    STARTUPINFO siStartInfo;
    BOOL        bSuccess = FALSE; 

    DWORD       dwRead; 
    CHAR        chBuf[BUFSIZE]; 
    BOOL        bSuccess1 = TRUE;

    // Set up members of the PROCESS_INFORMATION structure. 
    ZeroMemory( &piProcInfo, sizeof( PROCESS_INFORMATION ) );

    // Set up members of the STARTUPINFO structure. 
    // This structure specifies the STDIN and STDOUT handles for redirection.
    ZeroMemory( &siStartInfo, sizeof( STARTUPINFO ) );
    siStartInfo.cb          = sizeof( STARTUPINFO ); 
    siStartInfo.hStdError   = g_hChildStd_OUT_Wr;
    siStartInfo.hStdOutput  = g_hChildStd_OUT_Wr;
    siStartInfo.hStdInput   = 0;
    siStartInfo.dwFlags    |= STARTF_USESTDHANDLES;

    // Create the child process. 
    bSuccess = CreateProcess( NULL, 
        (LPSTR)cmd,       // command line 
        NULL,             // process security attributes 
        NULL,             // primary thread security attributes 
        TRUE,             // handles are inherited 
        CREATE_NO_WINDOW, // creation flags 
        NULL,             // use parent's environment 
        NULL,             // use parent's current directory 
        &siStartInfo,     // STARTUPINFO pointer 
        &piProcInfo );    // receives PROCESS_INFORMATION 

    // If an error occurs, exit the application. 
    if ( bSuccess ) {
        res = "";

        DWORD  threadID;
        HANDLE thread = CreateThread( 0, 0, Reader, 0, 0, &threadID );

        HWND waitDialog = 0;

        waitDialog = CreateDialog( hinst,
            MAKEINTRESOURCE( IDD_WAIT_DIALOG ),
            listWin,
            ToolDlgProc );

        int count = 0;
        if( waitDialog != NULL ) {
            MSG msg;
            // To make the GetMessage() loop spin, send idle messages
            PostMessage( waitDialog, WM_ENTERIDLE, MSGF_DIALOGBOX, (LPARAM)waitDialog );
            while( GetMessage( &msg, NULL, 0, 0 ) )
            {
                if( !IsDialogMessage( waitDialog, &msg ) ) {
                    TranslateMessage( &msg );
                    DispatchMessage( &msg );
                }

                DWORD wait_state = WaitForSingleObject( piProcInfo.hProcess, 50 );
                if ( wait_state == WAIT_OBJECT_0 || wait_state == WAIT_FAILED ) {
                    break;
                }

                if ( ++count == 40 ) {
                    ShowWindow( waitDialog, SW_SHOW );
                }
                else if ( count < 40 ) {
                    PostMessage( waitDialog, WM_ENTERIDLE, MSGF_DIALOGBOX, (LPARAM)waitDialog );
                }
            }

            DestroyWindow( waitDialog );
        }

        TerminateThread( thread, 0 );
    }

    // Close handles to the child process and its primary thread.
    CloseHandle( piProcInfo.hProcess );
    CloseHandle( piProcInfo.hThread );
    CloseHandle( g_hChildStd_OUT_Wr );

    // Read output from the child process's pipe for STDOUT
    // Stop when there is no more data. 
    bSuccess1 = TRUE;
    while (bSuccess1) 
    { 
        bSuccess1 = ReadFile( g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL );
        if( ! bSuccess1 || dwRead == 0 ) {
            break; 
        }
        res += std::string( chBuf, dwRead );
    } 

    return res;
}
