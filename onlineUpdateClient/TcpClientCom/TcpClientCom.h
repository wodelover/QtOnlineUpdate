/*************************************************************************/
/*                          Copyright Notice                             */
/* The code can not be copied or provided to other people without the    */
/* permission of Zhang Hao,otherwise intellectual property infringement  */
/* will be prosecuted.If you have any questions,please send me an email. */
/* My email is kingderzhang@foxmail.com. The final interpretation rights */
/* are interpreted by ZhangHao.                                          */
/*                  Copyright (C) ZhangHao All rights reserved           */
/*************************************************************************/

#ifndef TCPCLIENTCOM_H
#define TCPCLIENTCOM_H

#include "tcpclientcom_global.h"

// Qt Library
#include <QObject>
#include <QTcpSocket>
#include <QVariant>

/**
 * @ClassName: TcpClientCom
 * @Description: Tcp客户端通信组件
 * 需要在调用工程中配置 QT += network
 * @Autor: zhanghao kinderzhang@foxmail.com
 * @date: 2018-12-10 13:09:48
 * @Version: 1.0.0
 * @update_autor
 * @update_time
**/
class TCPCLIENTCOMSHARED_EXPORT TcpClientCom : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString serverIP READ serverIP WRITE setServerIP NOTIFY serverIPChanged)
    Q_PROPERTY(int serverPort READ serverPort WRITE setServerPort NOTIFY serverPortChanged)
    Q_PROPERTY(QString sof READ sof WRITE setSof NOTIFY sofChanged)
    Q_PROPERTY(QString eof READ eof WRITE setEof NOTIFY eofChanged)

public:

    TcpClientCom();
    TcpClientCom(QString ip,int port);
    ~TcpClientCom();

    /**
     * @MethodName: setServerIP
     * @Description: 设置服务器IP地址
     * @Autor: ZhangHao kinderzhang@foxmail.com
     * @date: 2018-12-10 10:46:36
     * @Version: 1.0.0
     * @Parma: [QString] ip 服务器IP地址
    **/
    Q_INVOKABLE void setServerIP(QString ip);
    Q_INVOKABLE void setServerPort(int port);

    // 设置数据包起始标志字符
    Q_INVOKABLE void setSof(QString sof);
    // 设置数据包结束标志字符
    Q_INVOKABLE void setEof(QString eof);

    // 数据包起始标志字符
    Q_INVOKABLE QString sof();
    // 数据包结束标志字符
    Q_INVOKABLE QString eof();


    /**
     * @MethodName: serverIP
     * @Description: 获取服务器IP地址
     * @Autor: ZhangHao kinderzhang@foxmail.com
     * @date: 2018-12-10 10:47:06
     * @Version: 1.0.0
    **/
    Q_INVOKABLE QString  serverIP();
    Q_INVOKABLE int  serverPort();

    /**
     * @MethodName: ReConectToServer
     * @Description: 重新连接服务器
     * @Autor: ZhangHao kinderzhang@foxmail.com
     * @date: 2018-12-10 10:49:22
     * @Version: 1.0.0
    **/
    Q_INVOKABLE void reConectToServer();
    Q_INVOKABLE void conectServer();
    Q_INVOKABLE void disConectServer();

    /**
     * @MethodName: sendDataToServer
     * @Description: 发送数据到服务器
     * @Autor: ZhangHao kinderzhang@foxmail.com
     * @date: 2018-12-10 14:14:43
     * @Version: 1.0.0
     * @Parma: [QVariant] data 数据内容
    **/
    Q_INVOKABLE int sendDataToServer(QVariant data);

    /**
     * @MethodName: getDataFromBuffer
     * @Description: 从缓冲区获取数据,默认全部读取数据
     * @Autor: ZhangHao kinderzhang@foxmail.com
     * @date: 2018-12-10 14:09:17
     * @Version: 1.0.0
     * @Parma: [long long] size 获取数据大小
    **/
    Q_INVOKABLE QByteArray getDataFromBuffer(long long size = 0);

    /**
     * @MethodName: getDataBufferSize
     * @Description: 获取当前缓冲区数据大小
     * @Autor: ZhangHao kinderzhang@foxmail.com
     * @date: 2018-12-10 14:08:30
     * @Version: 1.0.0
    **/
    Q_INVOKABLE long long getDataBufferSize();

signals:
    void serverIPChanged(QString ip);//IP地址改变
    void serverPortChanged(int port);//端口号改变
    void hasNewDataFromServer(QByteArray data);//新数据到来
    void sofChanged(QString sof);
    void eofChanged(QString eof);
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);

protected slots:
    void readyread();

private:
    void initSignal();
    QByteArray compressDataFrame(QByteArray cdata);
    QString m_serverIP;
    int m_serverPort;
    QTcpSocket m_socket;

    QString m_sof="<";
    QString m_eof=">";
    int m_size= -1;
};


#endif // TCPCLIENTCOM_H
