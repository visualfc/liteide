#include "unixptyprocess.h"
//#include <QStandardPaths>

#include <termios.h>
#include <errno.h>
#include <utmpx.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <QFileInfo>
#include <QCoreApplication>
#include <signal.h>

// support for build with MUSL on Alpine Linux
#ifndef _PATH_UTMPX
#include <sys/time.h>
# define _PATH_UTMPX	"/var/log/utmp"
#endif

/* for pty_getproc */
#if defined(__linux__)
#include <stdio.h>
#include <stdint.h>
#elif defined(__APPLE__)
#include <sys/sysctl.h>
#include <libproc.h>
#endif

static char *pty_getproc(int fd, char *tty);

UnixPtyProcess::UnixPtyProcess()
    : IPtyProcess()
    , m_readMasterNotify(0)
{
    //m_shellProcess.setWorkingDirectory(QStandardPaths::writableLocation(QStandardPaths::HomeLocation));
    connect(&m_shellProcess,SIGNAL(finished(int, QProcess::ExitStatus)),this,SLOT(finished(int, QProcess::ExitStatus)));
    connect(&m_shellProcess,SIGNAL(stateChanged(QProcess::ProcessState)),this,SLOT(stateChanged(QProcess::ProcessState)));
}

UnixPtyProcess::~UnixPtyProcess()
{
    kill();
}

bool UnixPtyProcess::startProcess(const QString &shellPath, const QStringList &arguments, const QString &workingDirectory, QStringList environment, qint16 cols, qint16 rows)
{
    if (m_shellProcess.state() == QProcess::Running)
        return false;

    QFileInfo fi(shellPath);
    if (fi.isRelative() || !QFile::exists(shellPath))
    {
        //todo add auto-find executable in PATH env var
        m_lastError = QString("Error: shell file path must be absolute");
        return false;
    }
#ifdef Q_OS_MAC
    m_shellName = fi.fileName();
#else
    m_shellName = shellPath;
#endif

    m_shellPath = shellPath;
    m_size = QPair<qint16, qint16>(cols, rows);

    int rc = 0;

    m_shellProcess.m_handleMaster = ::posix_openpt(O_RDWR | O_NOCTTY);
    if (m_shellProcess.m_handleMaster <= 0)
    {
        m_lastError = QString("UnixPty Error: unable to open master -> %1").arg(strerror(errno));
        kill();
        return false;
    }

    m_shellProcess.m_handleSlaveName = ptsname(m_shellProcess.m_handleMaster);
    if ( m_shellProcess.m_handleSlaveName.isEmpty())
    {
        m_lastError = QString("UnixPty Error: unable to get slave name -> %1").arg(strerror(errno));
        kill();
        return false;
    }

    signal(SIGINT, SIG_DFL);

    rc = grantpt(m_shellProcess.m_handleMaster);
    if (rc != 0)
    {
        m_lastError = QString("UnixPty Error: unable to change perms for slave -> %1").arg(strerror(errno));
        kill();
        return false;
    }

    rc = unlockpt(m_shellProcess.m_handleMaster);
    if (rc != 0)
    {
        m_lastError = QString("UnixPty Error: unable to unlock slave -> %1").arg(strerror(errno));
        kill();
        return false;
    }

    m_shellProcess.m_handleSlave = ::open(m_shellProcess.m_handleSlaveName.toLatin1().data(), O_RDWR | O_NOCTTY);
    if (m_shellProcess.m_handleSlave < 0)
    {
        m_lastError = QString("UnixPty Error: unable to open slave -> %1").arg(strerror(errno));
        kill();
        return false;
    }

    rc = fcntl(m_shellProcess.m_handleMaster, F_SETFD, FD_CLOEXEC);
    if (rc == -1)
    {
        m_lastError = QString("UnixPty Error: unable to set flags for master -> %1").arg(strerror(errno));
        kill();
        return false;
    }

    rc = fcntl(m_shellProcess.m_handleSlave, F_SETFD, FD_CLOEXEC);
    if (rc == -1)
    {
        m_lastError = QString("UnixPty Error: unable to set flags for slave -> %1").arg(strerror(errno));
        kill();
        return false;
    }

#ifdef Q_OS_MAC
    fcntl(m_shellProcess.m_handleMaster, F_SETFL, fcntl(m_shellProcess.m_handleMaster, F_GETFL) | O_NONBLOCK);
    fcntl(m_shellProcess.m_handleSlave, F_SETFL, fcntl(m_shellProcess.m_handleSlave, F_GETFL) | O_NONBLOCK);
#endif
    struct ::termios ttmode;
    rc = tcgetattr(m_shellProcess.m_handleMaster, &ttmode);
    if (rc != 0)
    {
        m_lastError = QString("UnixPty Error: termios fail -> %1").arg(strerror(errno));
        kill();
        return false;
    }

    ttmode.c_iflag = ICRNL | IXON | IXANY | IMAXBEL | BRKINT;
#if defined(IUTF8)
    ttmode.c_iflag |= IUTF8;
#endif

    ttmode.c_oflag = OPOST | ONLCR;
    ttmode.c_cflag = CREAD | CS8 | HUPCL;
    ttmode.c_lflag = ICANON | ISIG | IEXTEN | ECHO | ECHOE | ECHOK | ECHOKE | ECHOCTL;

    ttmode.c_cc[VEOF] = 4;
    ttmode.c_cc[VEOL] = -1;
    ttmode.c_cc[VEOL2] = -1;
    ttmode.c_cc[VERASE] = 0x7f;
    ttmode.c_cc[VWERASE] = 23;
    ttmode.c_cc[VKILL] = 21;
    ttmode.c_cc[VREPRINT] = 18;
    ttmode.c_cc[VINTR] = 3;
    ttmode.c_cc[VQUIT] = 0x1c;
    ttmode.c_cc[VSUSP] = 26;
    ttmode.c_cc[VSTART] = 17;
    ttmode.c_cc[VSTOP] = 19;
    ttmode.c_cc[VLNEXT] = 22;
    ttmode.c_cc[VDISCARD] = 15;
    ttmode.c_cc[VMIN] = 1;
    ttmode.c_cc[VTIME] = 0;

#if (__APPLE__)
    ttmode.c_cc[VDSUSP] = 25;
    ttmode.c_cc[VSTATUS] = 20;
#endif

    cfsetispeed(&ttmode, B38400);
    cfsetospeed(&ttmode, B38400);

    rc = tcsetattr(m_shellProcess.m_handleMaster, TCSANOW, &ttmode);
    if (rc != 0)
    {
        m_lastError = QString("UnixPty Error: unabble to set associated params -> %1").arg(strerror(errno));
        kill();
        return false;
    }

    m_readMasterNotify = new QSocketNotifier(m_shellProcess.m_handleMaster, QSocketNotifier::Read, &m_shellProcess);
    m_readMasterNotify->setEnabled(true);
    m_readMasterNotify->moveToThread(m_shellProcess.thread());
    connect(m_readMasterNotify,SIGNAL(activated(int)),this,SLOT(readActivated(int)));

    QStringList defaultVars;

    defaultVars.append("TERM=xterm-256color");
    defaultVars.append("ITERM_PROFILE=Default");
    defaultVars.append("XPC_FLAGS=0x0");
    defaultVars.append("XPC_SERVICE_NAME=0");
    defaultVars.append("LANG=en_US.UTF-8");
    defaultVars.append("LC_ALL=en_US.UTF-8");
    defaultVars.append("LC_CTYPE=UTF-8");
    defaultVars.append("INIT_CWD=" + QCoreApplication::applicationDirPath());
    defaultVars.append("COMMAND_MODE=unix2003");
    defaultVars.append("COLORTERM=truecolor");

    QStringList varNames;
    foreach (QString line, environment)
    {
        varNames.append(line.split("=").first());
    }

    //append default env vars only if they don't exists in current env
    foreach (QString defVar, defaultVars)
    {
        if (!varNames.contains(defVar.split("=").first()))
            environment.append(defVar);
    }

    QProcessEnvironment envFormat;
    foreach (QString line, environment)
    {
        envFormat.insert(line.split("=").first(), line.split("=").last());
    }
    m_shellProcess.setWorkingDirectory(workingDirectory);
    m_shellProcess.setProcessEnvironment(envFormat);
    m_shellProcess.setReadChannel(QProcess::StandardOutput);
    m_shellProcess.start(m_shellPath, arguments);
    m_shellProcess.waitForStarted();

    m_pid = m_shellProcess.pid();

    resize(cols, rows);

    return true;
}

bool UnixPtyProcess::resize(qint16 cols, qint16 rows)
{
    struct winsize winp;
    winp.ws_col = cols;
    winp.ws_row = rows;
    winp.ws_xpixel = 0;
    winp.ws_ypixel = 0;

    bool res =  ((ioctl(m_shellProcess.m_handleMaster, TIOCSWINSZ, &winp) != -1)  && (ioctl(m_shellProcess.m_handleSlave, TIOCSWINSZ, &winp) != -1) );
    if (res)
    {
        m_size = QPair<qint16, qint16>(cols, rows);
    }

    return res;
}

bool tty_kill(int fd, int signal)
{
#if defined(TIOCSIG)
    if (ioctl(fd, TIOCSIG, signal) == -1)
        return false;
#elif defined(TIOCSIGNAL)
    if (ioctl(fd, TIOCSIGNAL, signal) == -1)
        return false;
#endif
    return  true;
}

bool UnixPtyProcess::kill()
{
    m_shellProcess.m_handleSlaveName = QString();
    if (m_shellProcess.m_handleSlave >= 0)
    {
        ::close(m_shellProcess.m_handleSlave);
        m_shellProcess.m_handleSlave = -1;
    }
    if (m_shellProcess.m_handleMaster >= 0)
    {
        ::close(m_shellProcess.m_handleMaster);
        m_shellProcess.m_handleMaster = -1;
    }
    if (!m_readMasterNotify) {
        return false;
    }

    if (m_shellProcess.state() == QProcess::Running)
    {
        m_readMasterNotify->disconnect();
        m_readMasterNotify->deleteLater();

        m_shellProcess.terminate();
        m_shellProcess.waitForFinished(1000);

        if (m_shellProcess.state() == QProcess::Running)
        {
            QProcess::startDetached( QString("kill -9 %1").arg( pid() ) );
            m_shellProcess.kill();
            m_shellProcess.waitForFinished(1000);
        }

        return (m_shellProcess.state() == QProcess::NotRunning);
    }
    return false;
}

IPtyProcess::PtyType UnixPtyProcess::type()
{
    return IPtyProcess::UnixPty;
}

QString UnixPtyProcess::dumpDebugInfo()
{
#ifdef PTYQT_DEBUG
    return QString("PID: %1, In: %2, Out: %3, Type: %4, Cols: %5, Rows: %6, IsRunning: %7, Shell: %8, SlaveName: %9")
            .arg(m_pid).arg(m_shellProcess.m_handleMaster).arg(m_shellProcess.m_handleSlave).arg(type())
            .arg(m_size.first).arg(m_size.second).arg(m_shellProcess.state() == QProcess::Running)
            .arg(m_shellPath).arg(m_shellProcess.m_handleSlaveName);
#else
    return QString("Nothing...");
#endif
}

QIODevice *UnixPtyProcess::notifier()
{
    return &m_shellProcess;
}

QByteArray UnixPtyProcess::readAll()
{
    QByteArray tmpBuffer =  m_shellReadBuffer;
    m_shellReadBuffer.clear();
    return tmpBuffer;
}

qint64 UnixPtyProcess::write(const QByteArray &byteArray)
{
    int result = ::write(m_shellProcess.m_handleMaster, byteArray.constData(), byteArray.size());
    Q_UNUSED(result)

    return byteArray.size();
}

bool UnixPtyProcess::isAvailable()
{
    return true;
}

bool UnixPtyProcess::hasProcessList() const
{
    return getUnixProc() != m_shellName;
}

QString UnixPtyProcess::getUnixProc() const
{
    QString tmp;
    char *buf = pty_getproc(m_shellProcess.m_handleMaster,m_shellProcess.m_handleSlaveName.toUtf8().data());
    if (buf) {
        tmp = QString::fromUtf8(buf);
        free(buf);
    }
    return tmp;
}

void UnixPtyProcess::stateChanged(QProcess::ProcessState newState)
{
    if (newState == QProcess::Starting) {
        emit started();
    }
}

void UnixPtyProcess::moveToThread(QThread *targetThread)
{
    m_shellProcess.moveToThread(targetThread);
}

void UnixPtyProcess::finished(int /*exitCode*/, QProcess::ExitStatus /*exitStatus*/)
{
    emit exited();
}

void UnixPtyProcess::readActivated(int socket)
{
    Q_UNUSED(socket)

    QByteArray buffer;
    int readSize = 4096;
    QByteArray data;
    do
    {
        char nativeBuffer[4096];
        int len = ::read(m_shellProcess.m_handleMaster, nativeBuffer, readSize);
        data = QByteArray(nativeBuffer, len);
        buffer.append(data);
    } while (data.size() == readSize); //last data block always < readSize

    m_shellReadBuffer.append(buffer);
    m_shellProcess.emitReadyRead();
}

void ShellProcess::setupChildProcess()
{
    dup2(m_handleSlave, STDIN_FILENO);
    dup2(m_handleSlave, STDOUT_FILENO);
    dup2(m_handleSlave, STDERR_FILENO);


    pid_t sid = setsid();
    ioctl(m_handleSlave, TIOCSCTTY, 0);
    tcsetpgrp(m_handleSlave, sid);

    struct utmpx utmpxInfo;
    memset(&utmpxInfo, 0, sizeof(utmpxInfo));

    strncpy(utmpxInfo.ut_user, qgetenv("USER"), sizeof(utmpxInfo.ut_user));

    QString device(m_handleSlaveName);
    if (device.startsWith("/dev/"))
        device = device.mid(5);

    const char *d = device.toLatin1().constData();

    strncpy(utmpxInfo.ut_line, d, sizeof(utmpxInfo.ut_line));

    strncpy(utmpxInfo.ut_id, d + strlen(d) - sizeof(utmpxInfo.ut_id), sizeof(utmpxInfo.ut_id));

    struct timeval tv;
    gettimeofday(&tv, 0);
    utmpxInfo.ut_tv.tv_sec = tv.tv_sec;
    utmpxInfo.ut_tv.tv_usec = tv.tv_usec;

    utmpxInfo.ut_type = USER_PROCESS;
    utmpxInfo.ut_pid = getpid();

    utmpxname(_PATH_UTMPX);
    setutxent();
    pututxline(&utmpxInfo);
    endutxent();

#if !defined(Q_OS_UNIX)
    updwtmpx(_PATH_UTMPX, &loginInfo);
#endif


    struct sigaction action;
    sigemptyset(&action.sa_mask);
    action.sa_handler = SIG_DFL;
    action.sa_flags = 0;
    for (int signal = 1; signal < NSIG; signal++) {
        sigaction(signal, &action, 0);
    }
}


/**
 * pty_getproc
 * Taken from tmux.
 */

// Taken from: tmux (http://tmux.sourceforge.net/)
// Copyright (c) 2009 Nicholas Marriott <nicm@users.sourceforge.net>
// Copyright (c) 2009 Joshua Elsasser <josh@elsasser.org>
// Copyright (c) 2009 Todd Carson <toc@daybefore.net>
//
// Permission to use, copy, modify, and distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF MIND, USE, DATA OR PROFITS, WHETHER
// IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
// OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

#if defined(__linux__)

static char *
pty_getproc(int fd, char *tty) {
  FILE *f;
  char *path, *buf;
  size_t len;
  int ch;
  pid_t pgrp;
  int r;

  if ((pgrp = tcgetpgrp(fd)) == -1) {
    return NULL;
  }

  r = asprintf(&path, "/proc/%lld/cmdline", (long long)pgrp);
  if (r == -1 || path == NULL) return NULL;

  if ((f = fopen(path, "r")) == NULL) {
    free(path);
    return NULL;
  }

  free(path);

  len = 0;
  buf = NULL;
  while ((ch = fgetc(f)) != EOF) {
    if (ch == '\0') break;
    buf = (char *)realloc(buf, len + 2);
    if (buf == NULL) return NULL;
    buf[len++] = ch;
  }

  if (buf != NULL) {
    buf[len] = '\0';
  }

  fclose(f);
  return buf;
}

#elif defined(__APPLE__)

static char *
pty_getproc(int fd, char *tty) {
  int mib[4] = { CTL_KERN, KERN_PROC, KERN_PROC_PID, 0 };
  size_t size;
  struct kinfo_proc kp;

  if ((mib[3] = tcgetpgrp(fd)) == -1) {
    return NULL;
  }

  size = sizeof kp;
  if (sysctl(mib, 4, &kp, &size, NULL, 0) == -1) {
    return NULL;
  }

  if (*kp.kp_proc.p_comm == '\0') {
    return NULL;
  }

  return strdup(kp.kp_proc.p_comm);
}

#else

static char *
pty_getproc(int fd, char *tty) {
  return NULL;
}

#endif
