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
#include <sstream>

extern char command_string[];
char        cmd[MAX_PATH];

void        CreateChildProcess( std::string fileToLoad ); 
std::string ReadFromPipe(void);

HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;


std::string elfdump( std::string fileToLoad )
{
    SECURITY_ATTRIBUTES saAttr; 

    saAttr.nLength              = sizeof(SECURITY_ATTRIBUTES); 
    saAttr.bInheritHandle       = TRUE; 
    saAttr.lpSecurityDescriptor = NULL; 

    // Create a pipe for the child process's STDOUT
    if ( ! CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0) ) 
        return "Error on CreatePipe"; 
    // Ensure the read handle to the pipe for STDOUT is not inherited.

    if ( ! SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
        return "Stdout SetHandleInformation";

    // Create the child process. 

    CreateChildProcess( fileToLoad );

    // Read from pipe that is the standard output for child process. 

    std::string ret = ReadFromPipe(); 

    // The remaining open handles are cleaned up when this process terminates. 
    // To avoid resource leaks in a larger application, close handles explicitly. 

    return ret;
}


void CreateChildProcess( std::string fileToLoad )
    // Create a child process that uses the previously created pipes for STDIN and STDOUT.
{ 
    PROCESS_INFORMATION piProcInfo; 
    STARTUPINFO         siStartInfo;
    BOOL                bSuccess    = FALSE; 

    // Set up members of the PROCESS_INFORMATION structure. 

    ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );

    // Set up members of the STARTUPINFO structure. 
    // This structure specifies the STDIN and STDOUT handles for redirection.

    ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
    siStartInfo.cb          = sizeof(STARTUPINFO); 
    siStartInfo.hStdError   = g_hChildStd_OUT_Wr;
    siStartInfo.hStdOutput  = g_hChildStd_OUT_Wr;
    siStartInfo.hStdInput   = 0;
    siStartInfo.dwFlags    |= STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
    siStartInfo.wShowWindow = SW_HIDE;

    // Create the child process. 

    sprintf_s( cmd, command_string, fileToLoad.c_str() );
    bSuccess = CreateProcess( NULL, 
                              cmd,           // command line 
                              NULL,          // process security attributes 
                              NULL,          // primary thread security attributes 
                              TRUE,          // handles are inherited 
                              0,             // creation flags 
                              NULL,          // use parent's environment 
                              NULL,          // use parent's current directory 
                              &siStartInfo,  // STARTUPINFO pointer 
                              &piProcInfo ); // receives PROCESS_INFORMATION 

    // If an error occurs, exit the application. 
    if ( bSuccess ) {
        WaitForSingleObject( piProcInfo.hProcess, INFINITE );
    }

    // Close handles to the child process and its primary thread.
    // Some applications might keep these handles to monitor the status
    // of the child process, for example. 

    CloseHandle( piProcInfo.hProcess );
    CloseHandle( piProcInfo.hThread );
    CloseHandle( g_hChildStd_OUT_Wr );
}

std::string ReadFromPipe(void) 

    // Read output from the child process's pipe for STDOUT
    // and write to the parent process's pipe for STDOUT. 
    // Stop when there is no more data. 
{ 
    DWORD       dwRead; 
    CHAR        chBuf[BUFSIZE]; 
    std::string res;
    BOOL        bSuccess = TRUE;

    while (bSuccess) 
    { 
        bSuccess = ReadFile( g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
        if( ! bSuccess || dwRead == 0 ) {
            break; 
        }
        res += std::string( chBuf, dwRead );
    } 
    return res;
} 
