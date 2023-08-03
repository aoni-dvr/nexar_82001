#ifdef _WIN32
#include "public.h"
#include "config.h"
#include "worker.h"

#define SZAPPNAME            "httpd"
#define SZSERVICENAME        "httpd"
#define SZSERVICEDISPLAYNAME "Http Server Service"
#define SZSERVICEDESCRIPTION "Http Server Service"

// list of service dependencies - "dep1\0dep2\0\0"
#define SZDEPENDENCIES       ""
//////////////////////////////////////////////////////////////////////////////



////
////       If a service_stop procedure is going to take longer than
////       3 seconds to execute, it should spawn a thread to
////       execute the stop code, and return.  Otherwise, the
////       ServiceControlManager will believe that the service has
////       stopped responding
////
bool service_start(std::string const& config_pathname);
VOID service_stop();
//////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////
//// The following are procedures which
//// may be useful to call within the above procedures,
//// but require no implementation by the user.
//// They are implemented in service.c

//
//  FUNCTION: report_status()
//
//  PURPOSE: Sets the current status of the service and
//           reports it to the Service Control Worker
//
//  PARAMETERS:
//    dwCurrentState - the state of the service
//    dwWin32ExitCode - error code to report
//    dwWaitHint - worst case estimate to next checkpoint
//
//  RETURN VALUE:
//    TRUE  - success
//    FALSE - failure
//
BOOL report_status(DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint);


// internal variables
SERVICE_STATUS          g_service_status;       // current status of the service
SERVICE_STATUS_HANDLE   g_service_status_handle;
DWORD                   g_error = 0;
BOOL                    g_debug_mode = FALSE;
char                    g_error_text[256];
std::unique_ptr<Worker> g_worker;

// internal function prototypes
VOID WINAPI service_ctrl(DWORD dwCtrlCode);
VOID WINAPI service_main(DWORD dwArgc, LPTSTR *lpszArgv);
VOID cmd_install_service();
VOID cmd_remove_service();
VOID cmd_debug_service(int argc, char **argv);
BOOL WINAPI control_handler(DWORD dwCtrlType);
LPTSTR get_last_error_text(LPTSTR lpszBuf, DWORD dwSize);

int win_main(int argc, char* argv[]) {
  SERVICE_TABLE_ENTRY dispatchTable[] =
  {
    { TEXT(SZSERVICENAME), (LPSERVICE_MAIN_FUNCTION)service_main },
    { NULL, NULL }
  };

  if ((argc > 1) &&
    ((*argv[1] == '-') || (*argv[1] == '/'))) {
    if (_stricmp("install", argv[1] + 1) == 0) {
      cmd_install_service();
    } else if (_stricmp("remove", argv[1] + 1) == 0) {
      cmd_remove_service();
    } else if (_stricmp("debug", argv[1] + 1) == 0) {
      g_debug_mode = TRUE;
      cmd_debug_service(argc, argv);
    } else {
      goto dispatch;
    }
    return 0;
  }

  // if it doesn't match any of the above parameters
  // the service control manager may be starting the service
  // so we must call StartServiceCtrlDispatcher
dispatch:
  // this is just to be friendly
  printf("%s -install          to install the service\n", SZAPPNAME);
  printf("%s -remove           to remove the service\n", SZAPPNAME);
  printf("%s -debug <params>   to run as a console app for debugging\n", SZAPPNAME);
  printf("\nStartServiceCtrlDispatcher being called.\n");
  printf("This may take several seconds.  Please wait.\n");

  if (!StartServiceCtrlDispatcher(dispatchTable)) {
    printf("StartServiceCtrlDispatcher failed.\n");
  }
  return 0;
}

//
//  FUNCTION: service_main
//
//  PURPOSE: To perform actual initialization of the service
//
//  PARAMETERS:
//    dwArgc   - number of command line arguments
//    lpszArgv - array of command line arguments
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    This routine performs the service initialization and then calls
//    the user defined service_start() routine to perform majority
//    of the work.
//
void WINAPI service_main(DWORD argc, LPTSTR *argv) {
  if (argc < 2) {
    OutputDebugStringA("need parameter: path of config.ini");
    return;
  }

  // register our service control handler:
  //
  g_service_status_handle = RegisterServiceCtrlHandler(SZSERVICENAME, service_ctrl);

  if (!g_service_status_handle)
    return;

  // SERVICE_STATUS members that don't change in example
  //
  g_service_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
  g_service_status.dwServiceSpecificExitCode = 0;

  if (!report_status(SERVICE_RUNNING, NO_ERROR, 0))
    return;

  if (!service_start(argv[1])) {
    // exit,let SCManager reset service
    return;
  }

  (VOID)report_status(
    SERVICE_STOPPED,
    g_error,
    0);
}

//
//  FUNCTION: service_ctrl
//
//  PURPOSE: This function is called by the SCM whenever
//           ControlService() is called on this service.
//
//  PARAMETERS:
//    dwCtrlCode - type of control requested
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//
VOID WINAPI service_ctrl(DWORD dwCtrlCode) {
  // Handle the requested control code.
  //
  switch (dwCtrlCode) {
    // Stop the service.
    //
    // SERVICE_STOP_PENDING should be reported before
    // setting the Stop Event - hServerStopEvent - in
    // service_stop().  This avoids a race condition
    // which may result in a 1053 - The Service did not respond...
    // error.
  case SERVICE_CONTROL_STOP:
    report_status(SERVICE_STOP_PENDING, NO_ERROR, 3000);
    service_stop();
    return;
  case SERVICE_CONTROL_SHUTDOWN:
    report_status(SERVICE_STOP_PENDING, NO_ERROR, 3000);
    service_stop();
    return;
  case SERVICE_CONTROL_INTERROGATE:
    break;

    // invalid control code
    //
  default:
    break;

  }

  report_status(g_service_status.dwCurrentState, NO_ERROR, 0);
}



//
//  FUNCTION: report_status()
//
//  PURPOSE: Sets the current status of the service and
//           reports it to the Service Control Worker
//
//  PARAMETERS:
//    dwCurrentState - the state of the service
//    dwWin32ExitCode - error code to report
//    dwWaitHint - worst case estimate to next checkpoint
//
//  RETURN VALUE:
//    TRUE  - success
//    FALSE - failure
//
//  COMMENTS:
//
BOOL report_status(DWORD dwCurrentState,
  DWORD dwWin32ExitCode,
  DWORD dwWaitHint) {
  static DWORD dwCheckPoint = 1;
  BOOL fResult = TRUE;

  if (!g_debug_mode) // when debugging we don't report to the SCM
  {
    if (dwCurrentState == SERVICE_START_PENDING)
      g_service_status.dwControlsAccepted = 0;
    else
      g_service_status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;

    g_service_status.dwCurrentState = dwCurrentState;
    g_service_status.dwWin32ExitCode = dwWin32ExitCode;
    g_service_status.dwWaitHint = dwWaitHint;

    if ((dwCurrentState == SERVICE_RUNNING) ||
      (dwCurrentState == SERVICE_STOPPED))
      g_service_status.dwCheckPoint = 0;
    else
      g_service_status.dwCheckPoint = dwCheckPoint++;


    // Report the status of the service to the service control manager.
    //
    if (!(fResult = SetServiceStatus(g_service_status_handle, &g_service_status))) {
      LOGE("SetServiceStatus error: " << GetLastError());
    }
  }
  return fResult;
}

///////////////////////////////////////////////////////////////////
//
//  The following code handles service installation and removal
//


//
//  FUNCTION: cmd_install_service()
//
//  PURPOSE: Installs the service
//
//  PARAMETERS:
//    none
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//
void cmd_install_service() {
  SC_HANDLE   schService = NULL;
  SC_HANDLE   schSCManager = NULL;

  TCHAR szPath[512];

  if (GetModuleFileName(NULL, szPath, 512) == 0) {
    printf("Unable to install %s - %s\n", SZSERVICEDISPLAYNAME, get_last_error_text(g_error_text, 256));
    return;
  }

  schSCManager = OpenSCManager(
    NULL,                   // machine (NULL == local)
    NULL,                   // database (NULL == default)
    SC_MANAGER_CONNECT | SC_MANAGER_CREATE_SERVICE | SERVICE_CHANGE_CONFIG// access required
    );
  if (schSCManager) {
    schService = CreateService(
      schSCManager,               // SCManager database
      TEXT(SZSERVICENAME),        // name of service
      TEXT(SZSERVICEDISPLAYNAME), // name to display
      SERVICE_QUERY_STATUS |
      SERVICE_CHANGE_CONFIG |
      SERVICE_START,// desired access
      SERVICE_WIN32_OWN_PROCESS |
      SERVICE_INTERACTIVE_PROCESS,  // service type
      SERVICE_AUTO_START,       // start type
      SERVICE_ERROR_NORMAL,       // error control type
      szPath,                     // service's binary
      NULL,                       // no load ordering group
      NULL,                       // no tag identifier
      TEXT(SZDEPENDENCIES),       // dependencies
      NULL,                       // LocalSystem account
      NULL);                      // no password

    if (schService) {
      printf("%s installed.\n", SZSERVICEDISPLAYNAME);

      // SERVICE DESCRIPTION
      SERVICE_DESCRIPTION sdBuf;
      sdBuf.lpDescription = SZSERVICEDESCRIPTION;

      if (!ChangeServiceConfig2(
        schService,                // handle to service
        SERVICE_CONFIG_DESCRIPTION, // change: description
        &sdBuf))                  // value: new description
      {
        printf("Change Service Description ERROR: %d\n", GetLastError());
      } else
        printf("Change Service Description SUCCESS\n");

      // FAILURE_ACTIONS
      SERVICE_FAILURE_ACTIONS sfActions;
      SC_ACTION Actions[3];
      for (int i = 0; i < sizeof(Actions) / sizeof(SC_ACTION); i++) {
        Actions[i].Delay = 60 * 1000;	// 60 seconds reset service
        Actions[i].Type = SC_ACTION_RESTART;
      }
      sfActions.dwResetPeriod = 60 * 3;	// 3 minute
      sfActions.lpRebootMsg = NULL;
      sfActions.cActions = sizeof(Actions) / sizeof(SC_ACTION);
      sfActions.lpCommand = NULL;
      sfActions.lpsaActions = Actions;

      if (!ChangeServiceConfig2(
        schService,						// handle to service
        SERVICE_CONFIG_FAILURE_ACTIONS, // change: description
        &sfActions))						// value: new description
      {
        printf("Change Service Failure Actions ERROR: %d\n", GetLastError());
      } else
        printf("Change Service Failure Actions SUCCESS\n");
    } else {
      printf("CreateService failed - %s\n", get_last_error_text(g_error_text, 256));
    }
  } else
    printf("OpenSCManager failed - %s\n", get_last_error_text(g_error_text, 256));

  if (schService) CloseServiceHandle(schService);
  if (schSCManager) CloseServiceHandle(schSCManager);
}

void cmd_remove_service() {
  SC_HANDLE   schService;
  SC_HANDLE   schSCManager;

  schSCManager = OpenSCManager(
    NULL,                   // machine (NULL == local)
    NULL,                   // database (NULL == default)
    SC_MANAGER_CONNECT   // access required
    );
  if (schSCManager) {
    schService = OpenService(schSCManager, TEXT(SZSERVICENAME), DELETE | SERVICE_STOP | SERVICE_QUERY_STATUS);

    if (schService) {
      // try to stop the service
      if (ControlService(schService, SERVICE_CONTROL_STOP, &g_service_status)) {
        printf("Stopping %s.", SZSERVICEDISPLAYNAME);
        Sleep(1000);

        while (QueryServiceStatus(schService, &g_service_status)) {
          if (g_service_status.dwCurrentState == SERVICE_STOP_PENDING) {
            printf(".");
            Sleep(1000);
          } else
            break;
        }

        if (g_service_status.dwCurrentState == SERVICE_STOPPED)
          printf("\n%s stopped.\n", SZSERVICEDISPLAYNAME);
        else
          printf("\n%s failed to stop.\n", SZSERVICEDISPLAYNAME);

      }

      // now remove the service
      if (DeleteService(schService))
        printf("%s removed.\n", SZSERVICEDISPLAYNAME);
      else
        printf("DeleteService failed - %s\n", get_last_error_text(g_error_text, 256));


      CloseServiceHandle(schService);
    } else
      printf("OpenService failed - %s\n", get_last_error_text(g_error_text, 256));

    CloseServiceHandle(schSCManager);
  } else
    printf("OpenSCManager failed - %s\n", get_last_error_text(g_error_text, 256));
}




///////////////////////////////////////////////////////////////////
//
//  The following code is for running the service as a console app
//


//
//  FUNCTION: cmd_debug_service(int argc, char ** argv)
//
//  PURPOSE: Runs the service as a console application
//
//  PARAMETERS:
//    argc - number of command line arguments
//    argv - array of command line arguments
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//
void cmd_debug_service(int argc, char ** argv) {
  printf("Debugging %s.\n", SZSERVICEDISPLAYNAME);

  SetConsoleCtrlHandler(control_handler, TRUE);

  if (argc < 3) {
    printf("error: need parameter config_path_name\n");
    return;
  }

  service_start(argv[2]);
}


//
//  FUNCTION: control_handler ( DWORD dwCtrlType )
//
//  PURPOSE: Handled console control events
//
//  PARAMETERS:
//    dwCtrlType - type of control event
//
//  RETURN VALUE:
//    True - handled
//    False - unhandled
//
//  COMMENTS:
//
BOOL WINAPI control_handler(DWORD dwCtrlType) {
  switch (dwCtrlType) {
  case CTRL_BREAK_EVENT:  // use Ctrl+C or Ctrl+Break to simulate
  case CTRL_C_EVENT:      // SERVICE_CONTROL_STOP in debug mode
    printf("%s Service stoping\n", SZSERVICEDISPLAYNAME);
    service_stop();
    return TRUE;
    break;

  }
  return FALSE;
}

//
//  FUNCTION: get_last_error_text
//
//  PURPOSE: copies error message text to string
//
//  PARAMETERS:
//    lpszBuf - destination buffer
//    dwSize - size of buffer
//
//  RETURN VALUE:
//    destination buffer
//
//  COMMENTS:
//
LPTSTR get_last_error_text(LPTSTR lpszBuf, DWORD dwSize) {
  DWORD dwRet;
  LPTSTR lpszTemp = NULL;

  dwRet = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ARGUMENT_ARRAY,
    NULL,
    GetLastError(),
    LANG_NEUTRAL,
    (LPTSTR)&lpszTemp,
    0,
    NULL);

  // supplied buffer is not long enough
  if (!dwRet || ((long)dwSize < (long)dwRet + 14))
    lpszBuf[0] = '\0';
  else {
    lpszTemp[lstrlen(lpszTemp) - 2] = '\0';  //remove cr and newline character
    sprintf_s(lpszBuf, dwSize, "%s (%d)", lpszTemp, GetLastError());
  }

  if (lpszTemp)	LocalFree((HLOCAL)lpszTemp);

  return lpszBuf;
}

//
//  FUNCTION: service_start
//
//  PURPOSE: Actual code of the service that does the work.
//
//  PARAMETERS:
//    dwArgc   - number of command line arguments
//    lpszArgv - array of command line arguments
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//
bool service_start(std::string const& config_pathname) {
  LOGI("processor: " << getpid());
  g_worker.reset(new Worker(config_pathname));
  bool ret = g_worker->run();
  g_worker.reset();
  LOGI("processor return: " << ret);
  return ret;
}

//
//  FUNCTION: service_stop
//
//  PURPOSE: Stops the service
//
//  PARAMETERS:
//    none
//
//  RETURN VALUE:
//    none
//
//  COMMENTS:
//    If a service_stop procedure is going to
//    take longer than 3 seconds to execute,
//    it should spawn a thread to execute the
//    stop code, and return.  Otherwise, the
//    ServiceControlManager will believe that
//    the service has stopped responding.
//
VOID service_stop() {
  report_status(SERVICE_STOP_PENDING, NO_ERROR, 3000);
  if (g_worker) {
    g_worker->async_close();
  }
}
#endif // #ifdef _WIN32