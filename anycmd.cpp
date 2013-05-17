/*
anycmd.cpp - Total Commander lister plugin

Copyright (C) 2012-2013 by Serge Lamikhov-Center

MIT License

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

#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <Shlwapi.h>
#include <stdlib.h>
#include <shellapi.h>
#include <malloc.h>
#include <richedit.h>
#include <commdlg.h>
#include <math.h> 
#include <algorithm> 
#include <string>

#include "anycmd.h"

#define PLUGIN_NAME        "AnyCmd"
#define DETECT_STRING_KEY  "DetectString"
#define COMMAND_STRING_KEY "Command"
#define STREAM_SELECT_KEY  "Stream"

HINSTANCE hinst;
HWND      listWin = 0;

std::string g_text;
std::string g_text_lo;
char        inifilename[MAX_PATH]="anycmd.ini";

char detect_string[MAX_PATH];
char command_string[MAX_PATH];
char cmd[MAX_PATH];
int  streams = 3;

//---------------------------------------------------------------------------
static char*
strlcpy( char* p, char* p2, int maxlen )
{
    strncpy( p, p2, maxlen );
    p[maxlen] = 0;
    return p;
}


//---------------------------------------------------------------------------
static void
searchAndReplace( std::string& value, std::string const& search,
                  std::string const& replace ) 
{ 
    std::string::size_type  next; 

    for ( next = value.find( search );      // Try and find the first match 
        next != std::string::npos;          // next is npos if nothing was found 
        next = value.find( search, next )   // search for the next match starting after 
        // the last match that was found. 
        ) { 
            // Inside the loop. So we found a match. 
            if ( next == 0 || value[next - 1] != '\r' ) {
                value.replace( next, search.length(), replace );  // Do the replacement. 
            }
            // Move to just after the replace. This is the point were we start 
            // the next search from. 
            next += replace.length();
    }
}


//---------------------------------------------------------------------------
void
find_and_substitute_env_vars( char* str, size_t size )
{
    std::string work( str );

    bool found = true;
    std::basic_string <char>::size_type start_from = 0;
    while ( found ) {
        found = false;
        // Find first occurrence of '%' sign
        std::basic_string <char>::size_type first = work.find( '%', start_from );
        if ( first != std::string::npos ) {
            // Find second occurrence of '%' sign
            std::basic_string <char>::size_type second = work.find( '%', first + 1 );
            if ( second != std::string::npos ) {
                found      = true;
                start_from = first + 1;

                std::string var = work.substr( first + 1, second - first - 1 );

                // Try to substitute the substring between two '%' signs
                char buffer[MAX_PATH];
                if ( GetEnvironmentVariable( var.c_str(), buffer, MAX_PATH ) != 0 ) {
                    buffer[MAX_PATH - 1] = 0;
                    work.replace( first, second - first + 1, buffer );
                }
            }
        }
    }
    std::strncpy( str, work.c_str(), size );
    str[size - 1] = 0;
}


//---------------------------------------------------------------------------
BOOL APIENTRY
DllMain( HANDLE hModule, 
         DWORD  ul_reason_for_call, 
         LPVOID lpReserved )
{
    switch ( ul_reason_for_call )
    {
    case DLL_PROCESS_ATTACH:
        hinst = (HINSTANCE)hModule;
        break;
    case DLL_PROCESS_DETACH:
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    }

    return TRUE;
}


//---------------------------------------------------------------------------
void APIENTRY
ListGetDetectString( char* detectString, int maxlen )
{
    strlcpy( detectString, detect_string, maxlen );
}


//---------------------------------------------------------------------------
static
bool
FindPrivateIniName()
{
    DWORD   flags = GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                    GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;
    HMODULE hmod  = 0;
    DWORD   len = 0;

    ::GetModuleHandleEx( flags,
                         reinterpret_cast<LPCTSTR>( &::FindPrivateIniName ),
                         &hmod );
    if ( hmod == 0 ) {
        return false;
    }
    
    len = GetModuleFileName(hmod, inifilename, MAX_PATH);
    if ( len == 0 ) {
        return false;
    }

    PathRemoveExtension( inifilename );
    strcat( inifilename, ".ini" );

    if ( !PathFileExists( inifilename ) ) {
        return false;
    }

    return true;
}


//---------------------------------------------------------------------------
void APIENTRY
ListSetDefaultParams( ListDefaultParamStruct* dps )
{
    dps->PluginInterfaceVersionHi  = ANYELF_VERSION_HI;
    dps->PluginInterfaceVersionLow = ANYELF_VERSION_LOW;

    if ( !FindPrivateIniName() ) {
        strlcpy( inifilename, dps->DefaultIniName, MAX_PATH-1 );
    }

    GetPrivateProfileString( PLUGIN_NAME,
                             DETECT_STRING_KEY,
                             "EXT=TXT",
                             detect_string,
                             sizeof( detect_string ),
                             inifilename );
    GetPrivateProfileString( PLUGIN_NAME,
                             COMMAND_STRING_KEY,
                             "sort.exe %s",
                             command_string,
                             sizeof( command_string ),
                             inifilename );
    streams = GetPrivateProfileInt( PLUGIN_NAME,
                                    STREAM_SELECT_KEY,
                                    ANYCMD_CATCH_STD_OUT | ANYCMD_CATCH_STD_ERR,
                                    inifilename );
    if ( ( streams & ( ANYCMD_CATCH_STD_OUT | ANYCMD_CATCH_STD_ERR ) ) == 0 ) {
        streams = ANYCMD_CATCH_STD_OUT | ANYCMD_CATCH_STD_ERR;
    }

    sprintf_s( cmd, "%d", streams );
    WritePrivateProfileString( PLUGIN_NAME,
                               DETECT_STRING_KEY,
                               detect_string,
                               inifilename );
    WritePrivateProfileString( PLUGIN_NAME,
                               COMMAND_STRING_KEY,
                               command_string,
                               inifilename );
    WritePrivateProfileString( PLUGIN_NAME,
                               STREAM_SELECT_KEY,
                               cmd,
                               inifilename );
    
    // Substitute environment variables within the command
    find_and_substitute_env_vars( command_string, sizeof( command_string ) );
}


//---------------------------------------------------------------------------
HWND APIENTRY
    ListLoad( HWND parentWin, char* fileToLoad, int showFlags )
{
    RECT r;

    GetClientRect( parentWin, &r );
    // Create window invisbile, only show when data fully loaded!
    listWin = CreateWindow( "EDIT", "", WS_CHILD | ES_MULTILINE | ES_WANTRETURN |
                            ES_READONLY | WS_HSCROLL | WS_VSCROLL |
                            ES_AUTOVSCROLL | ES_NOHIDESEL,
                            r.left, r.top, r.right-r.left, r.bottom-r.top,
                            parentWin, NULL, hinst, NULL);

    if ( listWin != 0 ) {
        SendMessage( listWin, EM_SETMARGINS, EC_LEFTMARGIN, 8 );
        SendMessage( listWin, EM_SETEVENTMASK, 0, ENM_UPDATE ); //ENM_SCROLL doesn't work for thumb movements!

        ShowWindow( listWin, SW_SHOW );

        int ret = ListLoadNext( parentWin, listWin, fileToLoad, showFlags );
        if ( ret == LISTPLUGIN_ERROR ) {
            DestroyWindow( listWin );
            listWin = 0;
        }
    }

    return listWin;
}


//---------------------------------------------------------------------------
int APIENTRY
ListLoadNext( HWND parentWin, HWND listWin, char* fileToLoad, int showFlags)
{
    std::string cmd( command_string );

    searchAndReplace( cmd, "%s", fileToLoad );

    g_text = receive_text( cmd.c_str(), streams );
    if ( g_text.empty() ) {
        return LISTPLUGIN_ERROR;
    }

    searchAndReplace( g_text, "\n", "\r\n" );
    g_text_lo.resize( g_text.length() );
    std::transform( g_text.begin(), g_text.end(), g_text_lo.begin(), ::tolower );

    HFONT font;
    if ( showFlags & lcp_ansi ) {
        font = (HFONT)GetStockObject( ANSI_FIXED_FONT );
    }
    else {
        font = (HFONT)GetStockObject( SYSTEM_FIXED_FONT );
    }
    SendMessage( listWin, WM_SETFONT, (WPARAM)font, MAKELPARAM( true, 0 ) );

    SendMessage( listWin, WM_SETTEXT, 0, (LPARAM)g_text.c_str() ); 

    PostMessage( parentWin, WM_COMMAND, MAKELONG( 0, itm_percent ), (LPARAM)listWin );

    return LISTPLUGIN_OK;
}


//---------------------------------------------------------------------------
void APIENTRY
ListCloseWindow( HWND listWin )
{
    DestroyWindow( listWin );
    return;
}


//---------------------------------------------------------------------------
int APIENTRY
ListNotificationReceived( HWND listWin, int message, WPARAM wParam, LPARAM lParam )
{
    int firstvisible;
    int linecount;

    switch ( message ) {
    case WM_COMMAND:
        switch ( HIWORD( wParam ) ) {
        case EN_UPDATE:
        case EN_VSCROLL:
            firstvisible = (int)SendMessage( listWin, EM_GETFIRSTVISIBLELINE, 0, 0 );
            linecount    = (int)SendMessage( listWin, EM_GETLINECOUNT, 0, 0 );
            if ( linecount > 0 ) {
                int percent = MulDiv( firstvisible, 100, linecount );
                PostMessage( GetParent( listWin ), WM_COMMAND,
                             MAKELONG( percent, itm_percent ), (LPARAM)listWin );
            }
            return 0;
        }
        break;
        
    case WM_NOTIFY:
        break;
        
    case WM_MEASUREITEM:
        break;
        
    case WM_DRAWITEM:
        break;
        
    }
    return 0;
}


//---------------------------------------------------------------------------
int APIENTRY
ListSendCommand( HWND listWin, int command, int parameter )
{
    switch ( command ) {
    case lc_copy:
        SendMessage( listWin, WM_COPY, 0, 0 );
        return LISTPLUGIN_OK;

    case lc_newparams:
        HFONT font;
        if ( parameter & lcp_ansi ) {
            font = (HFONT)GetStockObject( ANSI_FIXED_FONT );
        }
        else {
            font = (HFONT)GetStockObject( SYSTEM_FIXED_FONT );
        }
        SendMessage( listWin, WM_SETFONT, (WPARAM)font, MAKELPARAM( true, 0 ) );
        PostMessage( GetParent( listWin ), WM_COMMAND, MAKELONG( 0, itm_next ), (LPARAM)listWin );
        return LISTPLUGIN_ERROR;
        
    case lc_selectall:
        SendMessage( listWin, EM_SETSEL, 0, -1 );
        return LISTPLUGIN_OK;
        
    case lc_setpercent:
        int firstvisible = (int)SendMessage( listWin, EM_GETFIRSTVISIBLELINE, 0, 0 );
        int linecount    = (int)SendMessage( listWin, EM_GETLINECOUNT, 0, 0 );
        if ( linecount > 0 ) {
            int pos = MulDiv( parameter, linecount, 100 );
            SendMessage( listWin, EM_LINESCROLL, 0, pos - firstvisible );
            firstvisible = (int)SendMessage( listWin, EM_GETFIRSTVISIBLELINE, 0, 0 );
            // Place caret on first visible line!
            int firstchar = (int)SendMessage( listWin, EM_LINEINDEX, firstvisible, 0);
            SendMessage( listWin, EM_SETSEL, firstchar, firstchar );
            pos = MulDiv( firstvisible, 100, linecount );
            // Update percentage display
            PostMessage( GetParent( listWin ), WM_COMMAND, MAKELONG( pos, itm_percent ), (LPARAM)listWin );
            return LISTPLUGIN_OK;
        }
        break;
        
    }

    return LISTPLUGIN_ERROR;
}


//---------------------------------------------------------------------------
static int
find_string( std::string search, int start, int params )
{
    std::string* text;

    if ( !( params & lcs_matchcase ) ) {
        text = &g_text_lo;
        std::transform( search.begin(), search.end(), search.begin(), ::tolower );
    }
    else {
        text = &g_text;
    }

    size_t pos;
    do {
        if ( params & lcs_backwards ) {
            start = start - 1;
            pos   = text->rfind( search, start - 1 );
        }
        else {
            pos   = text->find( search, start );
            start = (int)pos + 1;
        }
    } while ( ( pos != std::string::npos ) &&
              ( params & lcs_wholewords )  &&
              ( ::isalnum( (*text)[pos - 1] ) | ::isalnum( (*text)[pos + search.size()] ) ) );
    if ( pos == std::string::npos ) {
        return -1;
    }
    return (int)pos;
}


//---------------------------------------------------------------------------
int APIENTRY
ListSearchText( HWND listWin, char* searchString, int searchParameter )
{
    int startPos;

    if ( ( searchParameter & lcs_findfirst ) && !( searchParameter & lcs_backwards ) ) {
        //Find first: Start at top visible line
        int firstline = (int)SendMessage( listWin, EM_GETFIRSTVISIBLELINE, 0, 0 );
        startPos      = (int)SendMessage( listWin, EM_LINEINDEX, firstline, 0 );
        SendMessage( listWin, EM_SETSEL, startPos, startPos );
    } else {
        //Find next: Start at current selection+1
        SendMessage( listWin, EM_GETSEL, (WPARAM)&startPos, 0 );
        ++startPos;
    }

    int index = find_string( searchString, startPos, searchParameter );

    if ( index != -1 ) {
        int indexend = index + (int)strlen( searchString );
        SendMessage( listWin, EM_SETSEL, index, indexend );
        int line = (int)SendMessage( listWin, EM_LINEFROMCHAR, index, 0 ) - 3;
        if ( line < 0 )
            line = 0;
        line -= (int)SendMessage( listWin, EM_GETFIRSTVISIBLELINE, 0, 0 );
        SendMessage( listWin, EM_LINESCROLL, 0, line );

        return LISTPLUGIN_OK;
    }

    SendMessage( listWin, EM_SETSEL, -1, -1);  // Restart search at the beginning

    return LISTPLUGIN_ERROR;
}
