// main_service.cpp
#include "NamedPipeServer.h"
#include "ServerRequestHandler.h"
#include "NumberStore.h"
#include "./../Common/NumberProtocol.h"
#include <windows.h>
#include <iostream>
#include <atomic>

SERVICE_STATUS gServiceStatus{};
SERVICE_STATUS_HANDLE gStatusHandle{};
std::atomic<bool> gStopRequested(false);

NamedPipeServer* gServer = nullptr;
ServerRequestHandler* gSrvHandler = nullptr;
NumberStore gStore;

void WINAPI ServiceCtrlHandler(DWORD ctrlCode) {
    switch (ctrlCode) {
    case SERVICE_CONTROL_STOP:
        gServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
        SetServiceStatus(gStatusHandle, &gServiceStatus);
        gStopRequested.store(true);
        if (gServer) gServer->Stop();
        gServiceStatus.dwCurrentState = SERVICE_STOPPED;
        SetServiceStatus(gStatusHandle, &gServiceStatus);
        break;

    case SERVICE_CONTROL_PAUSE:
        gServiceStatus.dwCurrentState = SERVICE_PAUSED;
        SetServiceStatus(gStatusHandle, &gServiceStatus);
        // Optionally, tell your server to pause work
        break;

    case SERVICE_CONTROL_CONTINUE:
        gServiceStatus.dwCurrentState = SERVICE_RUNNING;
        SetServiceStatus(gStatusHandle, &gServiceStatus);
        // Optionally, resume work
        break;

    case SERVICE_CONTROL_INTERROGATE:
        // Must respond with current status
        SetServiceStatus(gStatusHandle, &gServiceStatus);
        break;

    default:
        break;
    }
}

void WINAPI ServiceMain(DWORD, LPWSTR*) {
    gStatusHandle = RegisterServiceCtrlHandlerW(L"NumberService", ServiceCtrlHandler);
    if (!gStatusHandle) return;

    // --- Report START_PENDING ---
    gServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    gServiceStatus.dwControlsAccepted = 0;  // NONE while starting
    gServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    gServiceStatus.dwWin32ExitCode = NO_ERROR;
    gServiceStatus.dwServiceSpecificExitCode = 0;
    gServiceStatus.dwCheckPoint = 1;
    gServiceStatus.dwWaitHint = 3000;       // 3 seconds
    SetServiceStatus(gStatusHandle, &gServiceStatus);

    // init service objects
    gSrvHandler = new ServerRequestHandler(gStore);
    gServer = new NamedPipeServer(kPipeName,
        [=](const std::string& r) { return gSrvHandler->HandleRequest(r); }, 8);
    gServer->Start();

    // --- Report RUNNING ---
    gServiceStatus.dwControlsAccepted =
        SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_SHUTDOWN;
    gServiceStatus.dwCurrentState = SERVICE_RUNNING;
    gServiceStatus.dwCheckPoint = 0;
    gServiceStatus.dwWaitHint = 0;
    SetServiceStatus(gStatusHandle, &gServiceStatus);

    while (!gStopRequested.load()) {
        Sleep(500);
    }

    // --- Report STOP_PENDING ---
    gServiceStatus.dwControlsAccepted = 0;
    gServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
    gServiceStatus.dwCheckPoint = 1;
    gServiceStatus.dwWaitHint = 3000;
    SetServiceStatus(gStatusHandle, &gServiceStatus);

    if (gServer) { gServer->Stop(); delete gServer; gServer = nullptr; }
    if (gSrvHandler) { delete gSrvHandler; gSrvHandler = nullptr; }

    // --- Report STOPPED ---
    gServiceStatus.dwCurrentState = SERVICE_STOPPED;
    gServiceStatus.dwCheckPoint = 0;
    gServiceStatus.dwWaitHint = 0;
    SetServiceStatus(gStatusHandle, &gServiceStatus);
}


int wmain(int argc, wchar_t* argv[]) {
    if (argc > 1 && wcscmp(argv[1], L"--console") == 0) {
        std::wcout << L"Running as console server...\n";
        ServerRequestHandler handler(gStore);
        NamedPipeServer server(kPipeName, [&](const std::string& req) {
            return handler.HandleRequest(req);
            }, 8);
        server.Start();
        std::wcout << L"Press Enter to stop server...\n";
        std::wstring dummy; std::getline(std::wcin, dummy);
        server.Stop();
        return 0;
    }

    SERVICE_TABLE_ENTRYW serviceTable[] = {
        { (LPWSTR)L"NumberService", ServiceMain },
        { nullptr, nullptr }
    };
    StartServiceCtrlDispatcherW(serviceTable);
    return 0;
}
