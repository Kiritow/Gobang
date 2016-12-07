#include "config.h"
#include <windows.h>
namespace Global
{
    void ErrorQuit(const char* ErrorMessage)
    {
        fprintf(stderr,ErrorMessage);
        exit(0);
    }
}

SDL_Window* wnd=NULL;
SDL_Renderer* rnd=NULL;

/// From MSDN!!
HWND GetConsoleHwnd(void)
{
#define MY_BUFSIZE 1024 // Buffer size for console window titles.
    HWND hwndFound;         // This is what is returned to the caller.
    char pszNewWindowTitle[MY_BUFSIZE]; // Contains fabricated
    // WindowTitle.
    char pszOldWindowTitle[MY_BUFSIZE]; // Contains original
    // WindowTitle.
    // Fetch current window title.
    GetConsoleTitle(pszOldWindowTitle, MY_BUFSIZE);
    // Format a "unique" NewWindowTitle.
    wsprintf(pszNewWindowTitle,"%d/%d",
             GetTickCount(),
             GetCurrentProcessId());
    // Change current window title.
    SetConsoleTitle(pszNewWindowTitle);
    // Ensure window title has been updated.
    Sleep(40);
    // Look for NewWindowTitle.
    hwndFound=FindWindow(NULL, pszNewWindowTitle);
    // Restore original window title.
    SetConsoleTitle(pszOldWindowTitle);
    return(hwndFound);
}
void HideConsole()
{
    ShowWindow(GetConsoleHwnd(),SW_HIDE);
}
void ShowConsole()
{
    ShowWindow(GetConsoleHwnd(),SW_SHOW);
}
