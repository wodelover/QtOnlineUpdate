#ifndef RUNTIME_H
#define RUNTIME_H

#include <QObject>
#include <QVariant>
#include <QMap>

#include <TcpServerCom.h>
// 配置服务器配置文件头,需要在配置文件中修改实际的IP和端口
const QString NetworkContent = "NetWork";
const QString listenIP = "IP";
const QString listenPort = "PORT";

// 定义文件目录
const QString FILES = "FILES";
const QString WorkDir = "WorkDir";
const QString NewFileHomePath = "FileDir";

/**
   * @className: RunTime
   * @descption: 文件在线更新服务端逻辑代码
   * @author: zhangh
   * @date: 2019-05-07
*/
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

public slots:
    void newClientConnected(QString ip,int port);
    void clientDisConnected(QString ip,int port);
    void hasNewDataFromClient(QString ip,int port,QByteArray data);

private:
    // <ip,filename>
    QMap<QString,QString> m_updateInfo;


    TcpServerCom m_server;

    void init();

    /**
   * @name: print
   * @descption: 控制台打印函数
   * @author: zhangh
   * @date: 2019-05-07
*/
    void print(QVariant msg);

    /**
   * @name: checkNeedUpdate
   * @descption: 校验文件是否需要更新
   * @author: zhangh
   * @date: 2019-05-07
   * @param: [QString] filename 文件名
   * @param: [QString] version 版本信息
   * @return: [bool] 是否更新
*/
    bool checkNeedUpdate(QString filename,QString version);


    /**
   * @name: sendNewFileVersion
   * @descption:  给客户端发送最新文件
   * @author: zhangh
   * @date: 2019-05-07
   * @param: [QString] ip ip地址
   * @param: [QString] version 新文件版本
*/
    void sendNewFileVersion(QString ip,QString version);

    /**
   * @name: sendNewFileSize
   * @descption:  给客户端发送最新文件
   * @author: zhangh
   * @date: 2019-05-07
   * @param: [QString] ip ip地址
   * @param: [QString] size 文件名字
*/
    void sendNewFileSize(QString ip,QString filename);

    /**
   * @name: sendNewFile
   * @descption:  给客户端发送最新文件
   * @author: zhangh
   * @date: 2019-05-07
   * @param: [QString] ip ip地址
   * @param: [QString] filename 文件名字
*/
    void sendNewFileData(QString ip,QString filename);

    /**
   * @name: cleanClientInfo
   * @descption: 清除客户端信息
   * @author: zhangh
   * @param: [QString] ip IP地址
   * @date: 2019-05-07
*/
    void cleanClientInfo(QString ip);

    /**
   * @name: replaceFileName
   * @descption:  替换待更新的文件名字
   * @author: zhangh
   * @date: 2019-05-07
   * @param: [QString] ip ip地址
   * @param: [QString] filename 文件名字
*/
    void replaceFileName(QString ip,QString filename);
};

#endif // RUNTIME_H
