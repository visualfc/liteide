#include "winptyprocess.h"
#include <QFile>
#include <QFileInfo>
#include <sstream>
#include <QCoreApplication>
#include <windows.h>

#define DEBUG_VAR_LEGACY "WINPTYDBG"
#define DEBUG_VAR_ACTUAL "WINPTY_DEBUG"
#define SHOW_CONSOLE_VAR "WINPTY_SHOW_CONSOLE"
#define WINPTY_AGENT_NAME "winpty-agent.exe"
#define WINPTY_DLL_NAME "winpty32.dll"

//#define WINPTY_DLL "winpty.dll"

static HMODULE hWinPtyDLL = NULL;


/* Gets the error code from the error object. */
WINPTY_API winpty_result_t (*winpty_error_code)(winpty_error_ptr_t err);

/* Returns a textual representation of the error.  The string is freed when
 * the error is freed. */
WINPTY_API LPCWSTR (*winpty_error_msg)(winpty_error_ptr_t err);

/* Free the error object.  Every error returned from the winpty API must be
 * freed. */
WINPTY_API void (*winpty_error_free)(winpty_error_ptr_t err);



WINPTY_API winpty_config_t *
(*winpty_config_new)(UINT64 agentFlags, winpty_error_ptr_t *err /*OPTIONAL*/);

/* Free the cfg object after passing it to winpty_open. */
WINPTY_API void (*winpty_config_free)(winpty_config_t *cfg);

WINPTY_API void
(*winpty_config_set_initial_size)(winpty_config_t *cfg, int cols, int rows);

/* Set the mouse mode to one of the WINPTY_MOUSE_MODE_xxx constants. */
WINPTY_API void
(*winpty_config_set_mouse_mode)(winpty_config_t *cfg, int mouseMode);

/* Amount of time to wait for the agent to startup and to wait for any given
 * agent RPC request.  Must be greater than 0.  Can be INFINITE. */
WINPTY_API void
(*winpty_config_set_agent_timeout)(winpty_config_t *cfg, DWORD timeoutMs);



WINPTY_API winpty_t *
(*winpty_open)(const winpty_config_t *cfg,
            winpty_error_ptr_t *err /*OPTIONAL*/);

WINPTY_API HANDLE (*winpty_agent_process)(winpty_t *wp);
WINPTY_API LPCWSTR (*winpty_conin_name)(winpty_t *wp);
WINPTY_API LPCWSTR (*winpty_conout_name)(winpty_t *wp);
WINPTY_API LPCWSTR (*winpty_conerr_name)(winpty_t *wp);
WINPTY_API winpty_spawn_config_t *
(*winpty_spawn_config_new)(UINT64 spawnFlags,
                        LPCWSTR appname /*OPTIONAL*/,
                        LPCWSTR cmdline /*OPTIONAL*/,
                        LPCWSTR cwd /*OPTIONAL*/,
                        LPCWSTR env /*OPTIONAL*/,
                        winpty_error_ptr_t *err /*OPTIONAL*/);
WINPTY_API void (*winpty_spawn_config_free)(winpty_spawn_config_t *cfg);
WINPTY_API BOOL
(*winpty_spawn)(winpty_t *wp,
             const winpty_spawn_config_t *cfg,
             HANDLE *process_handle /*OPTIONAL*/,
             HANDLE *thread_handle /*OPTIONAL*/,
             DWORD *create_process_error /*OPTIONAL*/,
             winpty_error_ptr_t *err /*OPTIONAL*/);
WINPTY_API BOOL
(*winpty_set_size)(winpty_t *wp, int cols, int rows,
                winpty_error_ptr_t *err /*OPTIONAL*/);
WINPTY_API int
(*winpty_get_console_process_list)(winpty_t *wp, int *processList, const int processCount,
                                winpty_error_ptr_t *err /*OPTIONAL*/);
WINPTY_API void (*winpty_free)(winpty_t *wp);


static bool winpty_init()
{
    int i;
    static struct
    {
    char	    *name;
    FARPROC	    *ptr;
    } winpty_entry[] =
    {
    {"winpty_conerr_name", (FARPROC*)&winpty_conerr_name},
    {"winpty_config_free", (FARPROC*)&winpty_config_free},
    {"winpty_config_new", (FARPROC*)&winpty_config_new},
    {"winpty_config_set_mouse_mode",
                      (FARPROC*)&winpty_config_set_mouse_mode},
    {"winpty_config_set_initial_size",
                    (FARPROC*)&winpty_config_set_initial_size},
    {"winpty_conin_name", (FARPROC*)&winpty_conin_name},
    {"winpty_conout_name", (FARPROC*)&winpty_conout_name},
    {"winpty_error_free", (FARPROC*)&winpty_error_free},
    {"winpty_free", (FARPROC*)&winpty_free},
    {"winpty_open", (FARPROC*)&winpty_open},
    {"winpty_spawn", (FARPROC*)&winpty_spawn},
    {"winpty_spawn_config_free", (FARPROC*)&winpty_spawn_config_free},
    {"winpty_spawn_config_new", (FARPROC*)&winpty_spawn_config_new},
    {"winpty_error_msg", (FARPROC*)&winpty_error_msg},
    {"winpty_set_size", (FARPROC*)&winpty_set_size},
    {"winpty_agent_process", (FARPROC*)&winpty_agent_process},
    {NULL, NULL}
    };

    // No need to initialize twice.
    if (hWinPtyDLL)
        return true;
    // Load winpty.dll, prefer using the 'winptydll' option, fall back to just
    // winpty.dll.
    if (!hWinPtyDLL)
        hWinPtyDLL = LoadLibraryA(WINPTY_DLL_NAME);
    qDebug() << "load" << hWinPtyDLL;
    if (!hWinPtyDLL)
    {
        return false;
    }
    for (i = 0; winpty_entry[i].name != NULL
         && winpty_entry[i].ptr != NULL; ++i)
    {
        if ((*winpty_entry[i].ptr = (FARPROC)GetProcAddress(hWinPtyDLL,
                                                            winpty_entry[i].name)) == NULL)
        {
            hWinPtyDLL = NULL;
            return false;
        }
    }

    return true;
}


QString castErrorToString(winpty_error_ptr_t error_ptr)
{
    return QString::fromStdWString(winpty_error_msg(error_ptr));
}

WinPtyProcess::WinPtyProcess()
    : IPtyProcess()
    , m_ptyHandler(0)
    , m_innerHandle(0)
    , m_inSocket(0)
    , m_outSocket(0)
{
    winpty_init();
}

WinPtyProcess::~WinPtyProcess()
{
    kill();
}

bool WinPtyProcess::startProcess(const QString &shellPath, const QStringList &arguments, const QString &workingDirectory, QStringList environment, qint16 rows, qint16 cols)
{
//    if (!isAvailable())
//    {
//        m_lastError = QString("WinPty Error: winpty-agent.exe or winpty.dll not found!");
//        return false;
//    }

    //already running
    if (m_ptyHandler)
        return false;

    QFileInfo fi(shellPath);
    if (fi.isRelative() || !QFile::exists(shellPath))
    {
        //todo add auto-find executable in PATH env var
        m_lastError = QString("WinPty Error: shell file path must be absolute");
        return false;
    }

    m_shellPath = shellPath;
    m_size = QPair<qint16, qint16>(cols, rows);

#ifdef PTYQT_DEBUG
    if (m_trace)
    {
        environment.append(QString("%1=1").arg(DEBUG_VAR_LEGACY));
        environment.append(QString("%1=trace").arg(DEBUG_VAR_ACTUAL));
        environment.append(QString("%1=1").arg(SHOW_CONSOLE_VAR));
        SetEnvironmentVariable(DEBUG_VAR_LEGACY, "1");
        SetEnvironmentVariable(DEBUG_VAR_ACTUAL, "trace");
        SetEnvironmentVariable(SHOW_CONSOLE_VAR, "1");
    }
#endif

    //env
    std::wstringstream envBlock;
    foreach (QString line, environment)
    {
        envBlock << line.toStdWString() << L'\0';
    }
    std::wstring env = envBlock.str();

    //create start config
    winpty_error_ptr_t errorPtr = 0;
    winpty_config_t* startConfig = winpty_config_new(0, &errorPtr);
    if (startConfig == 0)
    {
        m_lastError = QString("WinPty Error: create start config -> %1").arg(castErrorToString(errorPtr));
        return false;
    }
    winpty_error_free(errorPtr);

    //set params
    winpty_config_set_initial_size(startConfig, cols, rows);
    winpty_config_set_mouse_mode(startConfig, WINPTY_MOUSE_MODE_AUTO);
    //winpty_config_set_agent_timeout();

    //start agent
    m_ptyHandler = winpty_open(startConfig, &errorPtr);
    winpty_config_free(startConfig); //start config is local var, free it after use

    if (m_ptyHandler == 0)
    {
        m_lastError = QString("WinPty Error: start agent -> %1").arg(castErrorToString(errorPtr));
        return false;
    }
    winpty_error_free(errorPtr);

    //create spawn config
    winpty_spawn_config_t* spawnConfig = winpty_spawn_config_new(WINPTY_SPAWN_FLAG_AUTO_SHUTDOWN, m_shellPath.toStdWString().c_str(),
                                                                 //commandLine.toStdWString().c_str(), cwd.toStdWString().c_str(),
                                                                 NULL, workingDirectory.toStdWString().c_str(),
                                                                 env.c_str(),
                                                                 &errorPtr);

    if (spawnConfig == 0)
    {
        m_lastError = QString("WinPty Error: create spawn config -> %1").arg(castErrorToString(errorPtr));
        return false;
    }
    winpty_error_free(errorPtr);

    //spawn the new process
    BOOL spawnSuccess = winpty_spawn(m_ptyHandler, spawnConfig, &m_innerHandle, 0, 0, &errorPtr);
    winpty_spawn_config_free(spawnConfig); //spawn config is local var, free it after use
    if (!spawnSuccess)
    {
        m_lastError = QString("WinPty Error: start terminal process -> %1").arg(castErrorToString(errorPtr));
        return false;
    }
    winpty_error_free(errorPtr);

    m_pid = (int)GetProcessId(m_innerHandle);

    //get pipe names
    LPCWSTR conInPipeName = winpty_conin_name(m_ptyHandler);
    m_conInName = QString::fromStdWString(std::wstring(conInPipeName));
    m_inSocket = new QLocalSocket();
    m_inSocket->connectToServer(m_conInName, QIODevice::WriteOnly);
    m_inSocket->waitForConnected();

    LPCWSTR conOutPipeName = winpty_conout_name(m_ptyHandler);
    m_conOutName = QString::fromStdWString(std::wstring(conOutPipeName));
    m_outSocket = new QLocalSocket();
    m_outSocket->connectToServer(m_conOutName, QIODevice::ReadOnly);
    m_outSocket->waitForConnected();

    if (m_inSocket->state() != QLocalSocket::ConnectedState && m_outSocket->state() != QLocalSocket::ConnectedState)
    {
        m_lastError = QString("WinPty Error: Unable to connect local sockets -> %1 / %2").arg(m_inSocket->errorString()).arg(m_outSocket->errorString());
        m_inSocket->deleteLater();
        m_outSocket->deleteLater();
        m_inSocket = 0;
        m_outSocket = 0;
        return false;
    }

    connect(m_outSocket,SIGNAL(disconnected()),this,SLOT(disconnected()));


    return true;
}

bool WinPtyProcess::resize(qint16 cols, qint16 rows)
{
    if (m_ptyHandler == 0)
    {
        return false;
    }

    bool res = winpty_set_size(m_ptyHandler, cols, rows, 0);

    if (res)
    {
        m_size = QPair<qint16, qint16>(cols, rows);
    }

    return res;
}

bool WinPtyProcess::kill()
{
    bool exitCode = false;
    if (m_innerHandle != 0 && m_ptyHandler != 0)
    {
        //disconnect all signals (readyRead, ...)
        m_inSocket->disconnect();
        m_outSocket->disconnect();

        //disconnect for server
        m_inSocket->disconnectFromServer();
        m_outSocket->disconnectFromServer();

        m_inSocket->deleteLater();
        m_outSocket->deleteLater();

        m_inSocket = 0;
        m_outSocket = 0;

        winpty_free(m_ptyHandler);
        exitCode = CloseHandle(m_innerHandle);

        m_ptyHandler = 0;
        m_innerHandle = 0;
        m_conInName = QString();
        m_conOutName = QString();
        m_pid = 0;
    }
    return exitCode;
}

IPtyProcess::PtyType WinPtyProcess::type()
{
    return IPtyProcess::WinPty;
}

QString WinPtyProcess::dumpDebugInfo()
{
#ifdef PTYQT_DEBUG
    return QString("PID: %1, ConIn: %2, ConOut: %3, Type: %4, Cols: %5, Rows: %6, IsRunning: %7, Shell: %8")
            .arg(m_pid).arg(m_conInName).arg(m_conOutName).arg(type())
            .arg(m_size.first).arg(m_size.second).arg(m_ptyHandler != nullptr)
            .arg(m_shellPath);
#else
    return QString("Nothing...");
#endif
}

QIODevice *WinPtyProcess::notifier()
{
    return m_outSocket;
}

QByteArray WinPtyProcess::readAll()
{
    return m_outSocket->readAll();
}

qint64 WinPtyProcess::write(const QByteArray &byteArray)
{
    return m_inSocket->write(byteArray);
}

bool WinPtyProcess::isAvailable()
{
#ifdef PTYQT_BUILD_STATIC
    return QFile::exists(QCoreApplication::applicationDirPath() + "/" + WINPTY_AGENT_NAME);
#elif PTYQT_BUILD_DYNAMIC
    return QFile::exists(QCoreApplication::applicationDirPath() + "/" + WINPTY_AGENT_NAME)
            && QFile::exists(QCoreApplication::applicationDirPath() + "/" + WINPTY_DLL_NAME);
#endif

}

void WinPtyProcess::moveToThread(QThread *targetThread)
{
    m_inSocket->moveToThread(targetThread);
    m_outSocket->moveToThread(targetThread);
}

void WinPtyProcess::disconnected()
{
    emit exited();
}
