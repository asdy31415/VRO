#include "WindowList.h"
#include <windows.h>
#include <tchar.h>
#include <vector>
#include <string>
#include <algorithm>

std::string TCHARToString(TCHAR* tcha) {
#ifdef UNICODE
    std::wstring wstr(tcha);
    // Convert wstring to string only for demonstration output
    return std::string(wstr.begin(), wstr.end()); 
#else
    return std::string(tcha);
#endif
}

HWND WindowList::getHWNDbyTitle(std::string Title) {
    for (size_t i = 0; i < windowTitleList.size(); ++i) {
        if (windowTitleList[i] == Title) {
            return windowHWNDList[i];
        }
    }
    // return NULL if no window with the given name is found
    return NULL;
}

std::string WindowList::getTitlebyHWND(HWND hwnd) {
    int length = GetWindowTextLength(hwnd);

    if (length = 0) return "";

    TCHAR* windowTitle = new TCHAR[length + 1];
    GetWindowText(hwnd, windowTitle, length+1);

    return TCHARToString(windowTitle);
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    auto* windowList = reinterpret_cast<std::vector<HWND>*>(lParam);

    if (IsWindowVisible(hwnd)) {
        windowList->push_back(hwnd);
    }

    return TRUE;
}

void WindowList::updateList() {
    // Temporary list of currently open windows
    std::vector<HWND> currentHWNDList;

    // Enumerate all windows and get their HWNDs
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&currentHWNDList));

    for (size_t i = 0; i < windowHWNDList.size(); ) {
        auto it = std::find(currentHWNDList.begin(), currentHWNDList.end(), windowHWNDList[i]);

        if (it == currentHWNDList.end()) {
            // Remove closed windows from windowHWNDList and windowTitleList
            windowHWNDList.erase(windowHWNDList.begin() + i);
            windowTitleList.erase(windowTitleList.begin() + i);
        } else {
            // Check if Title has changed
            std::string currentTitle = getTitlebyHWND(windowHWNDList[i]);
            std::string newTitle = getTitlebyHWND(*it);

            if (currentTitle != newTitle) {
                // Update the Title
                windowTitleList[i] = newTitle;
            }
            ++i;
        }
    }
    // Add new windows that are not in the current list
    for (const auto& hwnd : currentHWNDList) {
        auto it = std::find(windowHWNDList.begin(), windowHWNDList.end(), hwnd);
        if (it == windowHWNDList.end()) {
            windowHWNDList.push_back(hwnd);
            windowTitleList.push_back(getTitlebyHWND(hwnd));
        }
    }
}