#include "conptyprocess.h"
#include <QFile>
#include <QFileInfo>
#include <QThread>
#include <sstream>
#include <QTimer>
#include <QMutexLocker>
#include <QCoreApplication>
#include <QDebug>

#define READ_INTERVAL_MSEC 500

HRESULT ConPtyProcess::createPseudoConsoleAndPipes(HPCON* phPC, HANDLE* phPipeIn, HANDLE* phPipeOut, qint16 cols, qint16 rows)
{
    HRESULT hr{ E_UNEXPECTED };
    HANDLE hPipePTYIn{ INVALID_HANDLE_VALUE };
    HANDLE hPipePTYOut{ INVALID_HANDLE_VALUE };

    // Create the pipes to which the ConPTY will connect
    if (CreatePipe(&hPipePTYIn, phPipeOut, NULL, 0) &&
            CreatePipe(phPipeIn, &hPipePTYOut, NULL, 0))
    {
        // Create the Pseudo Console of the required size, attached to the PTY-end of the pipes
        hr = m_winContext.createPseudoConsole({cols, rows}, hPipePTYIn, hPipePTYOut, 0, phPC);

        // Note: We can close the handles to the PTY-end of the pipes here
        // because the handles are dup'ed into the ConHost and will be released
        // when the ConPTY is destroyed.
        if (INVALID_HANDLE_VALUE != hPipePTYOut) CloseHandle(hPipePTYOut);
        if (INVALID_HANDLE_VALUE != hPipePTYIn) CloseHandle(hPipePTYIn);
    }

    return hr;
}

// Initializes the specified startup info struct with the required properties and
// updates its thread attribute list with the specified ConPTY handle
HRESULT ConPtyProcess::initializeStartupInfoAttachedToPseudoConsole(STARTUPINFOEX* pStartupInfo, HPCON hPC)
{
    HRESULT hr{ E_UNEXPECTED };

    if (pStartupInfo)
    {
        SIZE_T attrListSize{};

        pStartupInfo->StartupInfo.cb = sizeof(STARTUPINFOEX);
        pStartupInfo->StartupInfo.dwFlags = STARTF_USESTDHANDLES;


        // Get the size of the thread attribute list.
        InitializeProcThreadAttributeList(NULL, 1, 0, &attrListSize);

        // Allocate a thread attribute list of the correct size
        pStartupInfo->lpAttributeList =
                reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(malloc(attrListSize));

        // Initialize thread attribute list
        if (pStartupInfo->lpAttributeList
                && InitializeProcThreadAttributeList(pStartupInfo->lpAttributeList, 1, 0, &attrListSize))
        {
            // Set Pseudo Console attribute
            hr = UpdateProcThreadAttribute(
                        pStartupInfo->lpAttributeList,
                        0,
                        PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE,
                        hPC,
                        sizeof(HPCON),
                        NULL,
                        NULL)
                    ? S_OK
                    : HRESULT_FROM_WIN32(GetLastError());
        }
        else
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
        }
    }
    return hr;
}

ConPtyProcess::ConPtyProcess()
    : IPtyProcess()
    , m_ptyHandler { INVALID_HANDLE_VALUE }
    , m_hPipeIn { INVALID_HANDLE_VALUE }
    , m_hPipeOut { INVALID_HANDLE_VALUE }
    , m_readThread(nullptr)
{

}

ConPtyProcess::~ConPtyProcess()
{
    qDebug() << "~ConPtyProcess";
    kill();
}

VOID NTAPI waitForExit(PVOID p, BOOLEAN b) {
    ConPtyProcess *pThis = (ConPtyProcess*)(p);
    emit pThis->exited();
}

bool ConPtyProcess::startProcess(const QString &shellPath,const QStringList &arguments, const QString &workingDirectory, QStringList environment, qint16 cols, qint16 rows)
{
    if (!isAvailable())
    {
        m_lastError = m_winContext.lastError();
        return false;
    }

    //already running
    if (m_ptyHandler != INVALID_HANDLE_VALUE)
        return false;

    QFileInfo fi(shellPath);
    if (fi.isRelative() || !QFile::exists(shellPath))
    {
        //todo add auto-find executable in PATH env var
        m_lastError = QString("ConPty Error: shell file path must be absolute");
        return false;
    }

    m_shellPath = shellPath;
    m_size = QPair<qint16, qint16>(cols, rows);

    //env
    std::wstringstream envBlock;
    foreach (QString line, environment)
    {
        envBlock << line.toStdWString() << '\0';
    }
    envBlock << '\0';
    std::wstring env = envBlock.str();
    auto envV = vectorFromString(env);
    LPWSTR envArg = envV.empty() ? nullptr : envV.data();

    HRESULT hr{ E_UNEXPECTED };

    //  Create the Pseudo Console and pipes to it
    hr = createPseudoConsoleAndPipes(&m_ptyHandler, &m_hPipeIn, &m_hPipeOut, cols, rows);

    if (S_OK != hr)
    {
        m_lastError = QString("ConPty Error: CreatePseudoConsoleAndPipes fail");
        return false;
    }

    // Initialize the necessary startup info struct
    STARTUPINFOEX startupInfo{};
    if (S_OK != initializeStartupInfoAttachedToPseudoConsole(&startupInfo, m_ptyHandler))
    {
        m_lastError = QString("ConPty Error: InitializeStartupInfoAttachedToPseudoConsole fail");
        return false;
    }

    // Launch ping to emit some text back via the pipe
    PROCESS_INFORMATION piClient{};
    hr = CreateProcess(
                NULL,                           // No module name - use Command Line
                (LPWSTR)m_shellPath.toStdWString().c_str(),                         // Command Line
                NULL,                           // Process handle not inheritable
                NULL,                           // Thread handle not inheritable
                FALSE,                          // Inherit handles
                EXTENDED_STARTUPINFO_PRESENT | CREATE_UNICODE_ENVIRONMENT,   // Creation flags
                envArg, //NULL,                           // Use parent's environment block
                (LPCWSTR)(workingDirectory.toStdWString().c_str()),                           // Use parent's starting directory
                &startupInfo.StartupInfo,       // Pointer to STARTUPINFO
                &piClient)                      // Pointer to PROCESS_INFORMATION
            ? S_OK
            : GetLastError();

    if (hr != S_OK)
    {
        m_lastError = QString("ConPty Error: Cannot create process -> %1").arg(hr);
        return false;
    }
    m_pid = piClient.dwProcessId;

    HANDLE hWait = NULL;
    RegisterWaitForSingleObject(&hWait,piClient.hThread,waitForExit,this,INFINITE,WT_EXECUTEONLYONCE);

//    QThread::create([this,&piClient,&startupInfo]()
//    {
//        qDebug() << "wait";
//         WaitForSingleObject(piClient.hProcess, INFINITE);
//         qDebug() << "end wait";
////         CloseHandle(piClient.hThread);
////         CloseHandle(piClient.hProcess);
////         emit exited();
//    })->start();

    //this code runned in separate thread
    m_readThread = QThread::create([this, &piClient, &startupInfo]()
    {
        forever
        {
            //buffers
            const DWORD BUFF_SIZE{ 512 };
            char szBuffer[BUFF_SIZE]{};

            //DWORD dwBytesWritten{};
            DWORD dwBytesRead{};
            BOOL fRead{ FALSE };

            // Read from the pipe
            fRead = ReadFile(m_hPipeIn, szBuffer, BUFF_SIZE, &dwBytesRead, NULL);
            {
                QMutexLocker locker(&m_bufferMutex);
                m_buffer.m_readBuffer.append(szBuffer, dwBytesRead);
                m_buffer.emitReadyRead();
            }

            if (QThread::currentThread()->isInterruptionRequested())
                break;

            QCoreApplication::processEvents();
        }

        // Now safe to clean-up client app's process-info & thread
        CloseHandle(piClient.hThread);
        CloseHandle(piClient.hProcess);

        // Cleanup attribute list
        DeleteProcThreadAttributeList(startupInfo.lpAttributeList);
        //free(startupInfo.lpAttributeList);
    });

    //start read thread
    m_readThread->start();

    return true;
}

bool ConPtyProcess::resize(qint16 cols, qint16 rows)
{
    if (m_ptyHandler == nullptr)
    {
        return false;
    }

    bool res = SUCCEEDED(m_winContext.resizePseudoConsole(m_ptyHandler, {cols, rows}));

    if (res)
    {
        m_size = QPair<qint16, qint16>(cols, rows);
    }

    return res;

    return true;
}

bool ConPtyProcess::kill()
{
    bool exitCode = false;

    if ( m_ptyHandler != INVALID_HANDLE_VALUE )
    {
        m_readThread->requestInterruption();
        QThread::msleep(200);
        m_readThread->quit();
        m_readThread->deleteLater();
        m_readThread = nullptr;

        // Close ConPTY - this will terminate client process if running
        m_winContext.closePseudoConsole(m_ptyHandler);

        // Clean-up the pipes
        if (INVALID_HANDLE_VALUE != m_hPipeOut) CloseHandle(m_hPipeOut);
        if (INVALID_HANDLE_VALUE != m_hPipeIn) CloseHandle(m_hPipeIn);
        m_pid = 0;
        m_ptyHandler = INVALID_HANDLE_VALUE;
        m_hPipeIn = INVALID_HANDLE_VALUE;
        m_hPipeOut = INVALID_HANDLE_VALUE;

        exitCode = true;
    }

    return exitCode;
}

IPtyProcess::PtyType ConPtyProcess::type()
{
    return PtyType::ConPty;
}

QString ConPtyProcess::dumpDebugInfo()
{
#ifdef PTYQT_DEBUG
    return QString("PID: %1, Type: %2, Cols: %3, Rows: %4")
            .arg(m_pid).arg(type())
            .arg(m_size.first).arg(m_size.second);
#else
    return QString("Nothing...");
#endif
}

QIODevice *ConPtyProcess::notifier()
{
    return &m_buffer;
}

QByteArray ConPtyProcess::readAll()
{
    QMutexLocker locker(&m_bufferMutex);
    return m_buffer.m_readBuffer;
}

qint64 ConPtyProcess::write(const QByteArray &byteArray)
{
    DWORD dwBytesWritten{};
    WriteFile(m_hPipeOut, byteArray.data(), byteArray.size(), &dwBytesWritten, NULL);
    return dwBytesWritten;
}

bool ConPtyProcess::isAvailable()
{
#ifdef TOO_OLD_WINSDK
    return false; //very importnant! ConPty can be built, but it doesn't work if built with old sdk and Win10 < 1903
#endif

    qint32 buildNumber = QSysInfo::kernelVersion().split(".").last().toInt();
    if (buildNumber < CONPTY_MINIMAL_WINDOWS_VERSION)
        return false;
    return m_winContext.init();
}

void ConPtyProcess::moveToThread(QThread *targetThread)
{
    //nothing for now...
}

bool ConPtyProcess::hasProcessList() const
{
    return true;
}
