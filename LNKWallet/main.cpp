#include <QApplication>
#ifdef WIN32
#include <windows.h>
#include "DbgHelp.h"
#include "tchar.h"
#include "ShlObj.h"
#endif

#include <qapplication.h>
#include <QTranslator>
#include <QThread>


#include "wallet.h"
#include "frame.h"


#ifdef WIN32
bool checkOnly()
{

    //  创建互斥量
    HANDLE m_hMutex  =  CreateMutex(NULL, FALSE,  L"HXIndicator" );
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

LONG WINAPI TopLevelExceptionFilter(struct _EXCEPTION_POINTERS *pExceptionInfo)
{
    qDebug() << "Enter TopLevelExceptionFilter Function" ;
    HANDLE hFile = CreateFile(  _T(L"project.dmp"),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
    MINIDUMP_EXCEPTION_INFORMATION stExceptionParam;
    stExceptionParam.ThreadId    = GetCurrentThreadId();
    stExceptionParam.ExceptionPointers = pExceptionInfo;
    stExceptionParam.ClientPointers    = FALSE;
    MiniDumpWriteDump(GetCurrentProcess(),GetCurrentProcessId(),hFile,MiniDumpWithFullMemory,&stExceptionParam,NULL,NULL);
    CloseHandle(hFile);

    qDebug() << "End TopLevelExceptionFilter Function" ;
    return EXCEPTION_EXECUTE_HANDLER;
}

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

    QString strMsg("");
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
    qDebug() <<  "witnessConfig init: " << HXChain::getInstance()->witnessConfig->init();

//    QStringList keys = HXChain::getInstance()->transactionDB.keys();
    qInstallMessageHandler(myMessageOutput);
//    removeUpdateDeleteFile();

    if(checkOnly()==false)  return 0;    // 防止程序多次启动

    Frame frame;
    HXChain::getInstance()->mainFrame = &frame;   // 一个全局的指向主窗口的指针
    frame.show();

    a.installEventFilter(&frame);

    CommonHelper::setStyle(":/ui/qss/style.qss");

    int result = a.exec();
    HXChain::getInstance()->quit();

    return result;
}
