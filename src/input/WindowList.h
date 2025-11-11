#include <windows.h>
#include <vector>
#include <string>

class WindowList {
public:
    std::vector<HWND> getWindowHWND() const { return windowHWNDList; };
    std::vector<std::string> getWindowName() const { return windowTitleList; };
    HWND getHWNDbyTitle(std::string Title);
    std::string getTitlebyHWND(HWND hwnd);
    void updateList();

private:
    std::vector<HWND> windowHWNDList;
    std::vector<std::string> windowTitleList;
};