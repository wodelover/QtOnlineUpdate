#ifndef RUNTIME_H
#define RUNTIME_H

#include <QObject>
#include <QVariant>
#include <QTimer>
#include <QMap>

#include <TcpClientCom.h>
// 配置服务器配置文件头,需要在配置文件中修改实际的IP和端口
const QString NetworkContent = "NetWork";
const QString listenIP = "IP";
const QString listenPort = "PORT";

// 定义文件目录
const QString FILES = "FILES";
const QString WorkDir = "WorkDir";
const QString NewFileHomePath = "FileDir";

// 定义是否更新操作
const QString INIUpdate = "UPDATE";
const QString NeedUpdate = "NeedUpdate";

class RunTime : public QObject
{
    Q_OBJECT
public:
    explicit RunTime(QObject *parent = nullptr);
    enum UpdateRecord{       // 文件信息更新步骤记录
        FileNameVersion = 0,     // [0] 文件名和文件版本数据包标识
        FileStatus,                        // [1] 文件是否最新数据包标识
        RequestNewFile,             // [2] 请求最新文件数据包标识
        FileVersion,                       // [3] 版本信息
        FileDataSize,                    // [4] 更新文件的大小
        FileData,                           // [5] 当前最新文件数据包标识
        UpdateStatus,                  // [6] 当前文件更新状态数据包标识
        NextFile,                            // [7] 下一个待更新文件数据包标识
        Error};

protected slots:
    void hasNewDataFromServer(QByteArray data);//新数据到来
    void connected();
    void disconnected();
    void exitSystem();
    void error(QAbstractSocket::SocketError socketError);

private:
    TcpClientCom m_client;
    QTimer m_timer;
    // <filename,version>
    QMap<QString,QString>  m_files;

    // 用于保存更新文件的临时信息
    QString m_currentVersion="";
    qint32 m_currentFileSize=-1;
    QByteArray m_currentFiledata="";

    void init();

    /**
   * @name: print
   * @descption: 控制台打印函数
   * @author: zhangh
   * @date: 2019-05-07
*/
    void print(QVariant msg);

    /**
   * @name: updateFile
   * @descption: 依次下载需要更新的文件
   * @author: zhangh
   * @date: 2019-05-08
*/
    void updateFile();

};

#endif // RUNTIME_H
