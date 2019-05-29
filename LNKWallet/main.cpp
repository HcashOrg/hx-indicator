#include <QApplication>

#ifdef WIN32
#include <windows.h>
#ifdef QT_NO_DEBUG
#include "DbgHelp.h"
#include "tchar.h"
#include "ShlObj.h"
#else
#include "VisualLeakDetector/include/vld.h"
#endif
#endif

#include <qapplication.h>
#include <QTranslator>
#include <QThread>
#include <QMessageBox>

#include "wallet.h"
#include "frame.h"


#ifdef WIN32
bool checkOnly()
{
#ifdef TEST_WALLET
    //  创建互斥量
    HANDLE m_hMutex  =  CreateMutex(NULL, FALSE,  L"HXIndicator_test" );
#else
    //  创建互斥量
    HANDLE m_hMutex  =  CreateMutex(NULL, FALSE,  L"HXIndicator" );
#endif
    //  检查错误代码
    if  (GetLastError()  ==  ERROR_ALREADY_EXISTS)  {
      //  如果已有互斥量存在则释放句柄并复位互斥量
     CloseHandle(m_hMutex);
     m_hMutex  =  NULL;
      //  程序退出
      return  false;
    }
    else
        return true;
}

LONG ApplicationCrashHandler(EXCEPTION_POINTERS *pException)        // 捕获异常
{
    // 向node发送stop指令  防止数据损坏需要replay
    HXChain::getInstance()->isExiting = true;
    HXChain::getInstance()->postRPC( "id-witness_node_stop", toJsonFormat( "witness_node_stop", QJsonArray()), -1);

    EXCEPTION_RECORD* record = pException->ExceptionRecord;
    QString errCode(QString::number(record->ExceptionCode,16)),errAdr(QString::number((uint)record->ExceptionAddress,16)),errMod;
    QMessageBox::critical(NULL,"GUI CRASHED",
        QString("error code: %1 \nerror position: %2").arg(errCode).arg(errAdr),
        QMessageBox::Ok);
    return EXCEPTION_EXECUTE_HANDLER;
}

#ifdef QT_NO_DEBUG
LONG WINAPI TopLevelExceptionFilter(struct _EXCEPTION_POINTERS *pExceptionInfo)
{
    qDebug() << "Enter TopLevelExceptionFilter Function" ;
    HANDLE hFile = CreateFile(  _T("project.dmp"),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    MINIDUMP_EXCEPTION_INFORMATION stExceptionParam;
    stExceptionParam.ThreadId    = GetCurrentThreadId();
    stExceptionParam.ExceptionPointers = pExceptionInfo;
    stExceptionParam.ClientPointers    = FALSE;
    MiniDumpWriteDump(GetCurrentProcess(),GetCurrentProcessId(),hFile,MiniDumpWithFullMemory,&stExceptionParam,NULL,NULL);
    CloseHandle(hFile);

    qDebug() << "End TopLevelExceptionFilter Function" ;
    return EXCEPTION_EXECUTE_HANDLER;
}
#endif

LPWSTR ConvertCharToLPWSTR(const char * szString)
{
    int dwLen = strlen(szString) + 1;
    int nwLen = MultiByteToWideChar(CP_ACP, 0, szString, dwLen, NULL, 0);//算出合适的长度
    LPWSTR lpszPath = new WCHAR[dwLen];
    MultiByteToWideChar(CP_ACP, 0, szString, dwLen, lpszPath, nwLen);
    return lpszPath;
}




#endif

#ifdef TARGET_OS_MAC
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
bool checkOnly()
{
    const char filename[] = "/tmp/HXIndicatorlockfile";
    int fd = open( filename, O_WRONLY | O_CREAT, 0644);
    int flock = lockf(fd, F_TLOCK, 0);
    if( fd == -1)
    {
        perror("open lockfile/n");
        return false;
    }
    if( flock == -1)
    {
        perror("lock file error/n");
        return false;
    }
    return true;
}
#endif

class CommonHelper
{
public:
    static void setStyle(const QString &style)
    {
        QFile qss(style);
        qss.open(QFile::ReadOnly);
        qApp->setStyleSheet(qss.readAll());
        qss.close();
    }
};

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // 加锁
    static QMutex mutex;
    mutex.lock();

    QByteArray localMsg = msg.toLocal8Bit();

    QString strMsg;
    switch(type)
    {
    case QtDebugMsg:
        strMsg = QString("Debug:");
        break;
    case QtWarningMsg:
        strMsg = QString("Warning:");
        break;
    case QtCriticalMsg:
        strMsg = QString("Critical:");
        break;
    case QtFatalMsg:
        strMsg = QString("Fatal:");
        break;
    }

    // 设置输出信息格式
    QString strDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString strMessage = QString("DateTime:%1 Message:%2").arg(strDateTime).arg(localMsg.constData());

    // 输出信息至文件中（读写、追加形式），超过50M删除日志
    QFileInfo info("hx_log.txt");
    if(info.size() > 1024*1024*50) QFile::remove("hx_log.txt");
    QFile file("hx_log.txt");
    file.open(QIODevice::ReadWrite | QIODevice::Append);
    QTextStream stream(&file);
    stream << strMessage << "\n";
    file.flush();
    file.close();

    // 解锁
    mutex.unlock();
}

int main(int argc, char *argv[])
{
#ifdef WIN32
    //提前加载libeay32库
//    QString libeay = "D:\\SoftWare\\Visual Leak Detector\\bin\\Win64\\dbghelp.dll";
//    QString libeay = QCoreApplication::applicationDirPath()+"/libeay32.dll";
//    QString ssl = QCoreApplication::applicationDirPath()+"/ssleay32.dll";
//    qDebug()<<libeay;
//    ::LoadLibrary(LPCWSTR(libeay.utf16()));
//    ::LoadLibrary(LPCWSTR(ssl.utf16()));

#endif
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "1");

    QApplication a(argc, argv);

    qDebug()<<QDir::currentPath()<<QCoreApplication::applicationDirPath()<<QCoreApplication::applicationFilePath();

#ifdef TARGET_OS_MAC
    QDir::setCurrent( QCoreApplication::applicationDirPath());
#endif
//    SetUnhandledExceptionFilter(TopLevelExceptionFilter);

//    QTranslator translator;
//    translator.load(QString(":/qm/qt_zh_cn"));
//    a.installTranslator(&translator);
    HXChain::getInstance();  // 在frame创建前先创建实例，读取language
    qDebug() <<  "db init: " << HXChain::getInstance()->transactionDB.init();
//    HXChain::getInstance()->witnessConfig->append("trackaddress", "axiba",  true);
//    HXChain::getInstance()->witnessConfig->modify("crosschain-ip", "192.168.1.111");
//    HXChain::getInstance()->witnessConfig->remove("crosschain-port");
//    qDebug() << HXChain::getInstance()->witnessConfig->value("private-key");
//    HXChain::getInstance()->witnessConfig->addPrivateKey("GGGGGGG", "BBBBBBBBB");
//    HXChain::getInstance()->witnessConfig->addMiner("1.6.27");
//    HXChain::getInstance()->witnessConfig->addMiner("1.6.25");
//    qDebug() << HXChain::getInstance()->witnessConfig->getMiners();
//     HXChain::getInstance()->witnessConfig->save();

//    QStringList keys = HXChain::getInstance()->transactionDB.keys();
//    qInstallMessageHandler(myMessageOutput);
//    removeUpdateDeleteFile();

    if(checkOnly()==false)  return 0;    // 防止程序多次启动

    Frame frame;
    HXChain::getInstance()->mainFrame = &frame;   // 一个全局的指向主窗口的指针
    frame.show();

    a.installEventFilter(&frame);

    CommonHelper::setStyle(":/ui/qss/style.qss");

#ifndef LIGHT_MODE
#ifdef WIN32
    SetUnhandledExceptionFilter(ApplicationCrashHandler);
#endif
#endif

    int result = a.exec();
    HXChain::getInstance()->quit();

    return result;
}
