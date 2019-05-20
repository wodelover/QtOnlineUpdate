#include "runtime.h"
#include <processini.h>
#include <QCoreApplication>
#include <QFile>
#include <QTime>

RunTime::RunTime(QObject *parent) : QObject(parent)
{
    init();
}

void RunTime::connected()
{
    // [0] 读取当前所有文件的名字以及版本信息
    QStringList names = ProcessIni::getInstance()->getChildKeys(FILES);
    for(int i=0; i<names.size();i++){
        QString version = ProcessIni::getInstance()->readIni(FILES,names[i]).toString();
        m_files.insert(names[i],version);
    }

    // [1] 启动更新会话
    updateFile();
}

void RunTime::disconnected()
{
    print("disconnected from server...");
    exitSystem();
}

void RunTime::exitSystem()
{
    print("update program normal quit...");
    qApp->exit(0);
}

void RunTime::error(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::ConnectionRefusedError:{
        print("remote server can not connected,this time will not be update...");
        exitSystem();
        break;
    }
    default: break;
    }
}

void RunTime::print(QVariant msg)
{
    QString m = msg.toString();
    printf("%s\n",m.toStdString().c_str());
}

void RunTime::updateFile()
{
    // [0] 检查是否还有文件需要更新
    if(0==m_files.size()){
        print("No files to be update....");
        // 告诉服务器已经不需要再进行更新了
        QString data = QString::number(NextFile) + "-N";
        m_client.sendDataToServer(data);
        exitSystem();
        return;
    }

    // [1] 发送文件名字和版本信息,等待服务器响应更新
    QString filename = m_files.firstKey();
    QString version = m_files.value(filename);

    print("Check File Version:" + filename + " " + version);

    QString data = QString::number(FileNameVersion) + "-" + filename + "-" + version;
    if(!m_client.sendDataToServer(data)){
        print(filename + "---> update failed, skip this file this time...");
        // 移除当前这个文件，避免重复更新死循环
        m_files.remove(m_files.firstKey());
        updateFile();
    }
}

void RunTime::init()
{
    //[0] 读取是否进行更新操作
    QString needupdate = ProcessIni::getInstance()->readIni(INIUpdate,NeedUpdate).toString();

    if(needupdate!="true"){
        print("[0]:This time will not update,if you want to update,please set config file with:\n"
              "[UPDATE]\n"
              "NeedUpdate=true\n");
        connect(&m_timer,SIGNAL(timeout()),this,SLOT(exitSystem()));
        m_timer.start(1000);
        return;
    }
    // [1] 读取服务器网络配置并连接服务器
    QString ip = ProcessIni::getInstance()->readIni(NetworkContent,listenIP).toString();
    int port = ProcessIni::getInstance()->readIni(NetworkContent,listenPort).toString().toInt();
    m_client.setServerIP(ip);
    m_client.setServerPort(port);
    connect(&m_client,SIGNAL(connected()),this,SLOT(connected()));
    connect(&m_client,SIGNAL(disconnected()),this,SLOT(disconnected()));
    connect(&m_client,SIGNAL(hasNewDataFromServer(QByteArray)),this,SLOT(hasNewDataFromServer(QByteArray)));
    connect(&m_client,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(error(QAbstractSocket::SocketError)));
    m_client.conectServer();
}

void RunTime::hasNewDataFromServer(QByteArray data){

//    qDebug()<<"step: "<<data;

    // [0] 解析待处理流程标识
    QStringList msg = QString(data).split('-');

    // [1] 获取当前更新进度
    bool ok =false;
    UpdateRecord step = static_cast<UpdateRecord>(msg[0].toInt(&ok));
    if(!ok){
        print("UpdateRecord packet solve error,skip this file...");
        // 移除当前这个文件，避免重复更新死循环
        m_files.remove(m_files.firstKey());
        updateFile();
    }

    // [2] 处理更新的每一个步骤程序
    switch (step) {

    case FileStatus:{
        // [2.1] 检查当前文件是否是最新文件
        if(msg[1]=="Y"){

            print("File: " + m_files.firstKey() + " need update...");

            // [2.2] 发送请求最新文件信息,等待服务器响应更新
            QString data = QString::number(RequestNewFile) + "-Y";
            if(!m_client.sendDataToServer(data)){
                print(m_files.firstKey() + "--->Request update file failed, skip this file this time...");
                // 移除当前这个文件，避免重复更新死循环
                m_files.remove(m_files.firstKey());
                updateFile();
            }
        }else{
            print(m_files.firstKey() + ": is new,Update Check Next File...");
            // 移除当前这个文件，避免重复更新死循环
            m_files.remove(m_files.firstKey());
            updateFile();
        }

        break;
    }

    case FileVersion:{
        // [2.3] 校验版本信息
        if(msg.size()!=2){
            print(m_files.firstKey() + "--->update file version failed, skip this file this time...");

            // [2.3.1] 回复服务器收到版本信息
            QString data = QString::number(FileVersion) + "-N";
            m_client.sendDataToServer(data);
            // 移除当前这个文件，避免重复更新死循环
            m_files.remove(m_files.firstKey());
            updateFile();
        }else {

            print("File: " + m_files.firstKey() + " now is updating...");

            // [2.3.2] 设置更新文件的版本
            m_currentVersion = msg[1];

            // [2.3.3] 回复服务器收到版本信息
            QString data = QString::number(FileVersion) + "-Y";
            if(!m_client.sendDataToServer(data)){
                print(m_files.firstKey() + "--->Request update file failed, skip this file this time...");
                // 移除当前这个文件，避免重复更新死循环
                m_files.remove(m_files.firstKey());
                updateFile();
            }
        }
        break;
    }

    case FileDataSize:{
        // [2.4] 校验文件大小信息
        if(msg.size()==2){
            // [2.4.1] 设置更新文件的文件大小
            m_currentFileSize = msg[1].toInt();

            // [2.4.2] 回复服务器收到文件大小信息
            QString data = QString::number(FileDataSize) + "-Y";
            if(!m_client.sendDataToServer(data)){
                print(m_files.firstKey() + "--->Request update file size, skip this file this time...");
                // 移除当前这个文件，避免重复更新死循环
                m_files.remove(m_files.firstKey());
                updateFile();
            }
        }else{
            print(m_files.firstKey() + "--->update file size failed, skip this file this time...");
            // [2.4.3] 回复服务器收到文件大小
            QString data = QString::number(FileDataSize) + "-N";
            m_client.sendDataToServer(data);
            // 移除当前这个文件，避免重复更新死循环
            m_files.remove(m_files.firstKey());
            updateFile();
        }
        break;
    }

    case FileData:{
        // 分割文件数据,去除前面 两个字符标识
        data = data.remove(0,2);

        if(data.size()==m_currentFileSize){ // 接收数据完全

            // 回复数据接收成功
            QString reply = QString::number(FileData) + "-Y";

            QString path = ProcessIni::getInstance()->readIni(WorkDir,NewFileHomePath).toString() + m_files.firstKey();
            QFile file(path);
            if(file.open(QIODevice::WriteOnly)){ // 文件打开成功
                // 写文件
                qint64 size = file.write(data);

                if(size == m_currentFileSize){ //写入文件成功
                    // 更新配置文件
                    if(ProcessIni::getInstance()->writeIni(FILES,m_files.firstKey(),m_currentVersion)){
                        QString reply = QString::number(UpdateStatus) + "-Y";
                        qDebug()<<"bbb:"<<m_client.sendDataToServer(reply);
                        print("File:" + m_files.firstKey() + " Update Sucess,New Version is:" + m_currentVersion);

                        QString data = QString::number(NextFile) + "-Y";
                        m_client.sendDataToServer(data);

                        // 移除当前这个文件，避免重复更新死循环
                        m_files.remove(m_files.firstKey());
                        updateFile();
                    }else{
                        print("File:" + m_files.firstKey() + " Update Failed...");
                        QString reply = QString::number(UpdateStatus) + "-N";
                        m_client.sendDataToServer(reply);
                        print(m_files.firstKey() + "--->Write File New Version Failed, skip this file this time...");
                        // 移除当前这个文件，避免重复更新死循环
                        m_files.remove(m_files.firstKey());
                        updateFile();
                    }
                }
            }else { // open failed
                QString data = QString::number(FileData) + "-N";
                m_client.sendDataToServer(data);
                print(m_files.firstKey() + "--->Open File Failed, skip this file this time...");
                // 移除当前这个文件，避免重复更新死循环
                m_files.remove(m_files.firstKey());
                updateFile();
            }
        }
        break;
    }

    case Error:{
        if(msg.size()==2&&msg[1]=="Y"){
            // 服务器出现错误，跳过本文件更新
            print("UpdateRecord packet solve error,skip this file...");
            // 移除当前这个文件，避免重复更新死循环
            m_files.remove(m_files.firstKey());
            updateFile();
        }else{
            // 出现未知错误，退出更新程序
            m_client.disConectServer();
            disconnected();
        }
        break;
    }

    default:{
        print(" Error: No UpdateRecord Step In The Update Process...");
        return;
    }
    }
}
