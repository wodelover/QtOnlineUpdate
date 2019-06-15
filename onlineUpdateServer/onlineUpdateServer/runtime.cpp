#include "runtime.h"
#include <processini.h>
#include <QFile>
#include <logger.h>
#include <QDateTime>

RunTime::RunTime(QObject *parent) : QObject(parent)
{
    // [0] 初始化服务器相关信息
    init();

}

void RunTime::print(QVariant msg)
{
    QString m = msg.toString();

    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ");

    time += m;

    printf("%s\n",time.toStdString().c_str());
}

bool RunTime::checkNeedUpdate(QString filename, QString version)
{
    // [0] 校验文件版本信息
    QString current_version = ProcessIni::getInstance()->readIni(FILES,filename).toString();

    // [1] 文件不在更新列表中
    if(current_version=="0"){
        return false;
    }

    // [2] 比较版本信息
    if(current_version==version){
        return false;
    }
    return true;
}

void RunTime::sendNewFileVersion(QString ip, QString version)
{
    QString msg = QString::number(static_cast<qint8>(FileVersion))+"-"+version;
    m_server.sendDataToClient(msg.toUtf8(),ip,22,true);
}

void RunTime::sendNewFileSize(QString ip, QString filename)
{
    // [0] 读取文件最新版本信息
    QString new_version = ProcessIni::getInstance()->readIni(FILES,filename).toString();

    // [1] 读取当前需要更新的文件数据
    QString path = ProcessIni::getInstance()->readIni(WorkDir,NewFileHomePath).toString() +filename;
    QFile file(path);
    QByteArray data;
    if(!file.open(QIODevice::ReadOnly)){//文件打开失败
        print(ip + "--->file: " + path + " open file failed.give up update this time....");
        // [2] 发送打开失败
        QByteArray msg = QString::number(static_cast<qint8>(Error)).toUtf8() + "-Y";
        m_server.sendDataToClient(msg,ip,22,true);
        return;
    }else{
        data = file.readAll();
    }

    // [23] 发送文件的大小
    QByteArray msg = QString::number(static_cast<qint8>(FileDataSize)).toUtf8() + "-" + QString::number(data.size()).toUtf8();
    m_server.sendDataToClient(msg,ip,22,true);
    print(ip + ":  " + filename + " file size is: " + QString::number(data.size()));
}

void RunTime::sendNewFileData(QString ip, QString filename)
{
    // [0] 读取文件最新版本信息
    QString new_version = ProcessIni::getInstance()->readIni(FILES,filename).toString();

    // [1] 读取当前需要更新的文件数据
    QString path = ProcessIni::getInstance()->readIni(WorkDir,NewFileHomePath).toString() +filename;
    QFile file(path);
    QByteArray data;
    if(!file.open(QIODevice::ReadOnly)){//文件打开失败
        print(ip + "--->file: " + filename + " open file failed.give up update this file this time....");
        QByteArray msg = QString::number(static_cast<qint8>(Error)).toUtf8() + "-Y";
        m_server.sendDataToClient(msg,ip,22,true);
        return;
    }else{
        data = file.readAll();
    }

    // [2] 发送文件的数据
    QByteArray msg = QString::number(static_cast<qint8>(FileData)).toUtf8() + "-" + data;
    m_server.sendDataToClient(msg,ip,22,true);
}

void RunTime::cleanClientInfo(QString ip)
{
    // [0] 打印客户端断开连接信息
    print("Client DisConnected---->IP: " + ip);

    // [1] 删除客户端信息
    m_updateInfo.remove(ip);

    // [2] 断开客户端连接
    m_server.disconnectedClient(ip,123,true);
}

void RunTime::replaceFileName(QString ip, QString filename)
{
    QMap<QString,QString>::iterator itr = m_updateInfo.begin();
    for(;itr!=m_updateInfo.end();itr++){
        if(itr.key()==ip){
            *itr = filename;
        }
    }
}

void RunTime::newClientConnected(QString ip, int port)
{
    // [0] 打印当前连接客户端信息
    print("NewConnected---->IP: " + ip + "  Port: " + QString::number(port));

}

void RunTime::clientDisConnected(QString ip, int port)
{
    Q_UNUSED(port);
    cleanClientInfo(ip);
}

void RunTime::hasNewDataFromClient(QString ip, int port, QByteArray data)
{
    Q_UNUSED(port);

    //    print("new Data:" + data);

    // [0] 解析待处理流程标识
    QStringList msg = QString(data).split('-');

    // [1] 获取当前更新进度
    bool ok =false;
    UpdateRecord step = static_cast<UpdateRecord>(msg[0].toInt(&ok));
    if(!ok){
        print(ip + ": UpdateRecord packet solve error,skip this time...");
        return;
    }

    // [2] 处理更新的每一个步骤程序
    switch (step) {

    case FileNameVersion:{
        if(msg.size()!=3){ //版本信息校验分包后一定是3个字符串
            print(ip + ": check fileInfo packet error,this time update will be give up...");
            return;
        }
        // 检查更新
        if(!checkNeedUpdate(msg[1],msg[2])){
            // 回复客户端不需要更新文件
            QString data = QString::number(static_cast<qint8>(FileStatus))+"-N";
            m_server.sendDataToClient(data.toUtf8(),ip,port,true);
            print(ip + ":  " + msg[1] + " is new...");
        }else {
            //更新文件
            // [2.1] 保存客户端IP地址信息和文件名字
            m_updateInfo.insert(ip,msg[1]);
            // 回复客户端需要更新文件
            QString data = QString::number(static_cast<qint8>(FileStatus))+"-Y";
            m_server.sendDataToClient(data.toUtf8(),ip,port,false);
            print(ip + ":  " + msg[1] + " will be updated...");
        }
        break;
    }

    case RequestNewFile:{
        if(msg[1]=="Y"){
            QString filename = m_updateInfo.value(ip);
            QString fileVersion = ProcessIni::getInstance()->readIni(FILES,filename).toString();
            sendNewFileVersion(ip,fileVersion);
            print(ip + ":  " + filename + " new version is: " + fileVersion);
        }else{
            print(ip + ": RequestNewFile packet error,this time update will be give up...");
        }
        break;
    }

    case FileVersion:{
        QString filename = m_updateInfo.value(ip);
        sendNewFileSize(ip,filename);
        break;
    }

    case FileDataSize:{
        if(msg.size()==2&&msg[1]=="Y"){
            QString filename = m_updateInfo.value(ip);
            print(ip + ": sending file data...");
            sendNewFileData(ip,filename);
        }else{
            print(ip + ": FileDataSize check Error,give up this time this file...");
            QString data = QString::number(static_cast<qint8>(FileData))+"-N";
            m_server.sendDataToClient(data.toUtf8(),ip,11,true);
        }

        break;
    }

    case FileData:{
        if(msg.size()==2&&msg[1]=="Y"){
            print(ip + " : file " + m_updateInfo.value(m_updateInfo.firstKey()) + " Donwload Done...");
        }
        else if(msg.size()==2&&msg[1]=="N"){
            print(ip + " : file " + m_updateInfo.value(m_updateInfo.firstKey()) + " Donwload Error...");
        }
        break;
    }

    case UpdateStatus:{
        if(msg.size()==2&&msg[1]=="Y"){
            print(ip + " : file " + m_updateInfo.value(m_updateInfo.firstKey()) + " Update Sucessed...");
        }
        else if(msg.size()==2&&msg[1]=="N"){
            print(ip + " : file " + m_updateInfo.value(m_updateInfo.firstKey()) + " Update Failed...");
        }
        break;
    }

    case NextFile:{
        if(msg[1]=="N"){//没有文件需要更新
            print(ip + "All Files Have been Updated...");
            cleanClientInfo(ip);
        }
        break;
    }
    case Error:{

        break;
    }

    default:{
        print(ip + " Error: No UpdateRecord Step In The Update Process...");
        return;
    }
    }
}

void RunTime::init()
{
    // [0] 读取服务器网络配置
    QString ip = ProcessIni::getInstance()->readIni(NetworkContent,listenIP).toString();
    QString port = ProcessIni::getInstance()->readIni(NetworkContent,listenPort).toString();

    // [1] 初始化系统相关
    m_server.setListenIP(ip);
    m_server.setListenPort(port.toInt());

    // [2] 设置信号连接
    connect(&m_server,SIGNAL(newClientConnected(QString,int)),this,SLOT(newClientConnected(QString,int)));
    connect(&m_server,SIGNAL(clientDisConnected(QString,int)),this,SLOT(clientDisConnected(QString,int)));
    connect(&m_server,SIGNAL(hasNewDataFromClient(QString,int,QByteArray)),this,SLOT(hasNewDataFromClient(QString,int,QByteArray)));

    // [3] 打开监听服务
    bool status = m_server.listen(true);
    if(status){
        print("listen success...\n");
    }else {
        print("listen failed,please check config file with:"
              "[NetWork]\n"
              "IP=127.0.0.1\n"
              "PORT=52013\n");
    }
}
