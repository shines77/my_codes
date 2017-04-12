
#ifndef _WIN32_WINNT            // 指定要求的最低平台是 Windows XP。
#define _WIN32_WINNT 0x0501     // 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif

#ifndef _WIN32_WINNT            // 指定要求的最低平台是 Windows XP。
#define _WIN32_WINNT 0x0501     // 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif

#ifndef _WIN32_WINDOWS          // 指定要求的最低平台是 Windows XP。
#define _WIN32_WINDOWS 0x0501   // 将此值更改为适当的值，以适用于 Windows 的其他版本。
#endif

#ifndef _WIN32_IE               // 指定要求的最低平台是 Internet Explorer 6.0。
#define _WIN32_IE 0x0600        // 将此值更改为相应的值，以适用于 IE 的其他版本。
#endif

// Windows 头文件
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN // 从 Windows 头中排除极少使用的资料
#endif
#include <windows.h>

#include <stdio.h>
#include <stdlib.h>

#include <cstdio>
#include <iostream>
#include <string>
#include <atomic>
#include <mutex>
#include <thread>
#include <chrono>
#include <functional>
#include <tuple>
#include <type_traits>

volatile bool g_bExit = false;

class FooA
{
public:
    FooA() {};
    ~FooA() {};

    static VOID CALLBACK WaitOrTimerCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
    {
        if (lpParameter != NULL) {
            ((FooA *)lpParameter)->OnWaitOrTimerCallback(TimerOrWaitFired);
        }
    }
protected:
    void OnWaitOrTimerCallback(BOOLEAN TimerOrWaitFired)
    {
        printf("call OnWaitOrTimerCallback()\n");
    }

public:
    void OnWaitOrTimerCallbackWrapper(BOOLEAN TimerOrWaitFired)
    {
        printf("call OnWaitOrTimerCallbackWrapper()\n");
    }
};

typedef struct _PARAM_DATA {
    std::function<void(BOOLEAN)> Callback;
    PVOID Host;
} PARAM_DATA, * PPARAM_DATA;

template <typename T>
class CallbackWrapper {
public:
    static VOID CALLBACK WaitOrTimerCallback(PVOID lpParameter, BOOLEAN TimerOrWaitFired)
    {
        PARAM_DATA * ParamData = (PARAM_DATA *)lpParameter;
        if (ParamData != NULL) {
            if (ParamData->Callback) {
                (ParamData->Callback)(TimerOrWaitFired);
            }
            else if (ParamData->Host != NULL) {
                ((T *)ParamData->Host)->OnWaitOrTimerCallbackWrapper(TimerOrWaitFired);
            }
            else {
                // Unknown error
            }
        }
    }
};

template <typename T>
BOOL CreateTimerQueueTimerWrapper(
    _Outptr_ PHANDLE phNewTimer,
    _In_opt_ HANDLE TimerQueue,
    _In_ std::function<void(BOOLEAN)> const & Callback,
    _In_ T const & Host,
    _In_ PARAM_DATA & ParamData,
    _In_opt_ PVOID Parameter,
    _In_ DWORD DueTime,
    _In_ DWORD Period,
    _In_ ULONG Flags
    )
{
    ParamData.Callback = Callback;
    ParamData.Host = (PVOID)const_cast<T *>(&Host);
    return CreateTimerQueueTimer(phNewTimer, TimerQueue, &CallbackWrapper<T>::WaitOrTimerCallback,
                                 (PVOID)&ParamData, DueTime, Period, Flags);
}

void test_timequeue()
{
    FooA foo;
    PARAM_DATA ParamData;
    HANDLE hTimer1 = NULL;
    HANDLE hTimer2 = NULL;

    CreateTimerQueueTimer(&hTimer1, NULL, &FooA::WaitOrTimerCallback, (PVOID)&foo, 0, 1000, 0);

    std::function<void(BOOLEAN)> callback = std::bind(&FooA::OnWaitOrTimerCallbackWrapper, &foo, std::placeholders::_1);
    CreateTimerQueueTimerWrapper(&hTimer2, NULL, callback, foo, ParamData, NULL, 0, 1000, 0);

    MSG msg;
    BOOL isQuit = FALSE;
    while (GetMessage(&msg, NULL, 0, 0)) {
        if (TranslateMessage(&msg)) {
            if (msg.message == WM_QUIT ||
                msg.message == WM_CLOSE) {
                printf("\nWill be quit!\n\n");
                DispatchMessage(&msg);
                break;
            }

            DispatchMessage(&msg);
        }
    }

    if (hTimer1) {
        CloseHandle(hTimer1);
    }
    if (hTimer2) {
        CloseHandle(hTimer2);
    }
}

BOOL CALLBACK CosonleHandler(DWORD event)
{
    BOOL bResult = FALSE;
    switch (event) {
    case CTRL_C_EVENT:
        printf("Ctrl + C, exiting ...\n");
        g_bExit = true;
        PostMessage(GetConsoleWindow(), WM_QUIT, 0, 0);
        Sleep(10000);
        bResult = TRUE;
        break;

    case CTRL_BREAK_EVENT:
        printf("Ctrl + Break, exiting ...\n");
        g_bExit = true;
        PostMessage(GetConsoleWindow(), WM_QUIT, 0, 0);
        Sleep(10000);
        bResult = TRUE;
        break;

    case CTRL_CLOSE_EVENT:
        printf("Click close button, exiting ...\n");
        g_bExit = true;
        PostMessage(GetConsoleWindow(), WM_QUIT, 0, 0);
        Sleep(10000);
        bResult = TRUE;
        break;

    default:
        break;
    }
    return bResult;
}

int main(int argn, char * argv[])
{
    SetConsoleCtrlHandler(CosonleHandler, TRUE);

    test_timequeue();
    return 0;
}
