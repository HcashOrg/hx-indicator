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
    HANDLE m_hMutex  =  CreateMutex(NULL, FALSE,  L"HXWallet" );
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
    const char filename[] = "/tmp/HXwalletlockfile";
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
//    QStringList keys = HXChain::getInstance()->transactionDB.keys();

//    qInstallMessageHandler(outputMessage);  // 重定向qebug 到log.txt
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
