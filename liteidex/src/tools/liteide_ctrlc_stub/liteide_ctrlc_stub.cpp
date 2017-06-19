/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company.  For licensing terms and
** conditions see http://www.qt.io/terms-conditions.  For further information
** use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file.  Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, The Qt Company gives you certain additional
** rights.  These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

/* A stub for Windows console processes (like nmake) that is able to terminate
 * its child process via a generated Ctrl-C event.
 * The termination is triggered by sending a custom message to the HWND of
 * this process. */

#ifndef WINVER
#define WINVER 0x0501
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <wchar.h>
#include <cstdlib>
#include <cstdio>

const wchar_t szTitle[] = L"liteide_ctrlcstub";
const wchar_t szWindowClass[] = L"wliteide_ctrlcstub";
UINT uiShutDownWindowMessage;
UINT uiInterruptMessage;
HWND hwndMain = 0;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL WINAPI shutdownHandler(DWORD dwCtrlType);
BOOL WINAPI interruptHandler(DWORD dwCtrlType);
bool isSpaceOrTab(const wchar_t c);
bool startProcess(wchar_t pCommandLine[]);

int main(int argc, char **)
{
    if (argc < 2) {
        fprintf(stderr, "This is an internal helper of LiteIDE. Do not run it manually.\n");
        return 1;
    }

    uiShutDownWindowMessage = RegisterWindowMessage(L"liteide_ctrlcstub_shutdown");
    uiInterruptMessage = RegisterWindowMessage(L"liteide_ctrlcstub_interrupt");

    WNDCLASSEX wcex;
    ZeroMemory(&wcex,sizeof(WNDCLASSEX));
    wcex.cbSize = sizeof(wcex);
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = GetModuleHandle(0);
    wcex.lpszClassName = szWindowClass;
    if (!RegisterClassEx(&wcex))
        return 1;

    hwndMain = CreateWindow(szWindowClass, szTitle, WS_DISABLED,
                            0, 0, 0, 0, NULL, NULL, wcex.hInstance, NULL);
    if (!hwndMain)
        return FALSE;

    // Get the command line and remove the call to this executable.
    wchar_t *strCommandLine = _wcsdup(GetCommandLine());
    const size_t strCommandLineLength = wcslen(strCommandLine);
    size_t pos = 0;
    bool quoted = false;
    while (pos < strCommandLineLength) {
        if (strCommandLine[pos] == L'"') {
            quoted = !quoted;
        } else if (!quoted && isSpaceOrTab(strCommandLine[pos])) {
            while (isSpaceOrTab(strCommandLine[++pos]));
            break;
        }
        ++pos;
    }

    bool bSuccess = startProcess(strCommandLine + pos);
    free(strCommandLine);

    if (!bSuccess)
        return -1;

    MSG msg;
    DWORD dwExitCode = -1;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (msg.message == WM_DESTROY)
            dwExitCode = static_cast<DWORD>(msg.wParam);
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)dwExitCode;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == uiShutDownWindowMessage) {
        SetConsoleCtrlHandler(interruptHandler, FALSE);
        SetConsoleCtrlHandler(shutdownHandler, TRUE);
        GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);
        PostQuitMessage(0);
        return 0;
    } else if (message == uiInterruptMessage) {
        SetConsoleCtrlHandler(interruptHandler, TRUE);
        SetConsoleCtrlHandler(shutdownHandler, FALSE);
        GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);
        return 0;
    }

    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

bool isSpaceOrTab(const wchar_t c)
{
    return c == L' ' || c == L'\t';
}

BOOL WINAPI shutdownHandler(DWORD /*dwCtrlType*/)
{
    PostMessage(hwndMain, WM_DESTROY, 0, 0);
    return TRUE;
}

BOOL WINAPI interruptHandler(DWORD /*dwCtrlType*/)
{
    return TRUE;
}

DWORD WINAPI processWatcherThread(LPVOID lpParameter)
{
    HANDLE hProcess = reinterpret_cast<HANDLE>(lpParameter);
    WaitForSingleObject(hProcess, INFINITE);
    DWORD dwExitCode;
    if (!GetExitCodeProcess(hProcess, &dwExitCode))
        dwExitCode = -1;
    CloseHandle(hProcess);
    PostMessage(hwndMain, WM_DESTROY, dwExitCode, 0);
    return 0;
}

bool startProcess(wchar_t *pCommandLine)
{
    SECURITY_ATTRIBUTES sa;
    ZeroMemory(&sa,sizeof(SECURITY_ATTRIBUTES));
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;

    STARTUPINFO si;
    ZeroMemory(&si,sizeof(STARTUPINFO));
    si.cb = sizeof(si);

    PROCESS_INFORMATION pi;
    DWORD dwCreationFlags = 0;
    BOOL bSuccess = CreateProcess(NULL, pCommandLine, &sa, &sa, TRUE, dwCreationFlags, NULL, NULL, &si, &pi);
    if (!bSuccess) {
        fwprintf(stderr, L"liteide_ctrlc_stub: Command line failed: %s\n", pCommandLine);
        return false;
    }
    CloseHandle(pi.hThread);

    HANDLE hThread = CreateThread(NULL, 0, processWatcherThread, reinterpret_cast<void*>(pi.hProcess), 0, NULL);
    if (!hThread) {
        fwprintf(stderr, L"liteide_ctrlc_stub: The watch dog thread cannot be started.\n");
        return false;
    }
    CloseHandle(hThread);
    return true;
}
