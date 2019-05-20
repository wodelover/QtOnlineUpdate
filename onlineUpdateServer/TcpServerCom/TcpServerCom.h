/*************************************************************************/
/*                          Copyright Notice                             */
/* The code can not be copied or provided to other people without the    */
/* permission of Zhang Hao,otherwise intellectual property infringement  */
/* will be prosecuted.If you have any questions,please send me an email. */
/* My email is kingderzhang@foxmail.com. The final interpretation rights */
/* are interpreted by ZhangHao.                                          */
/*                  Copyright (C) ZhangHao All rights reserved           */
/*************************************************************************/

#ifndef TCPSERVERCOM_H
#define TCPSERVERCOM_H

#include "tcpservercom_global.h"

// Qt Library
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QByteArray>
#include <QHostAddress>
#include <QVector>
#include <QDataStream>

// Define Usr Single Mode
//#define SINGLEMODE

/**
 * @ClassName: TcpServerCom
 * @Description: TCP服务端通信组件
 * @Autor: zhanghao kinderzhang@foxmail.com
 * @date: 2018-12-11 09:40:44
 * @Version: 1.0.0
 * @update_autor
 * @update_time
**/
class TCPSERVERCOMSHARED_EXPORT TcpServerCom : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString listenIP READ listenIP WRITE setListenIP NOTIFY listenIPChanged)
    Q_PROPERTY(int listenPort READ listenPort WRITE setListenPort NOTIFY listenPortChanged)
    Q_PROPERTY(QString sof READ sof WRITE setSof NOTIFY sofChanged)
    Q_PROPERTY(QString eof READ eof WRITE setEof NOTIFY eofChanged)

public:
    TcpServerCom();
    ~TcpServerCom();
    /**
     * @MethodName: setListenIP
     * @Description: 设置本地监听IP地址
     * @Autor: ZhangHao kinderzhang@foxmail.com
     * @date: 2018-12-11 09:57:26
     * @Version: 1.0.0
     * @Parma: [QString] ip 本地ip地址
    **/
    Q_INVOKABLE void setListenIP(QString ip);
    /**
     * @MethodName: setListenPort
     * @Description: 设置本地监听端口
     * @Autor: ZhangHao kinderzhang@foxmail.com
     * @date: 2018-12-11 09:59:13
     * @Version: 1.0.0
     * @Parma: [int] port 本地监听端口
    **/
    Q_INVOKABLE void setListenPort(int port);

    // 设置数据包起始标志字符
    Q_INVOKABLE void setSof(QString sof);
    // 设置数据包结束标志字符
    Q_INVOKABLE void setEof(QString eof);

    /**
     * @MethodName: listenIP
     * @Description: 返回本地监听IP地址
     * @Autor: ZhangHao kinderzhang@foxmail.com
     * @date: 2018-12-11 10:00:20
     * @Version: 1.0.0
    **/
    Q_INVOKABLE QString listenIP();
    /**
     * @MethodName: listenPort
     * @Description: 返回本地监听IP端口
     * @Autor: ZhangHao kinderzhang@foxmail.com
     * @date: 2018-12-11 10:00:20
     * @Version: 1.0.0
    **/
    Q_INVOKABLE int listenPort();

    // 数据包起始标志字符
    Q_INVOKABLE QString sof();
    // 数据包结束标志字符
    Q_INVOKABLE QString eof();

    /**
     * @MethodName: listen
     * @Description: 设置当前是否开启监听,bool参数决定
     * @Autor: ZhangHao kinderzhang@foxmail.com
     * @date: 2018-12-11 10:21:22
     * @Version: 1.0.0
     * @Parma: [bool] status 默认打开监听,也可以设置关闭
    **/
    Q_INVOKABLE bool listen(bool status = true);

    /**
     * @MethodName: disconnectClient
     * @Description: 根据IP地址断开客户端
     * @Autor: ZhangHao kinderzhang@foxmail.com
     * @date: 2018-12-11 13:42:50
     * @Version: 1.0.0
     * @Parma: [QString] ip 客户端IP地址
     * @Param: [bool] all 是否断开全部
    **/
    Q_INVOKABLE void disconnectedClient(QString ip, int port, bool all=false);
    /**
     * @MethodName: disconectClient
     * @Description: 断开客户端通过当前的容器索引
     * @Autor: ZhangHao kinderzhang@foxmail.com
     * @date: 2018-12-19 10:57:09
     * @Version: 1.0.0
     * @Parma: [int] index 客户端在容器中的索引
    **/
    Q_INVOKABLE void disconnectedClient(int index);

    /**
     * @MethodName: sendDataToClient
     * @Description: 发送数据到客户端
     * broad参数控制是否所有客户端发送
     * @Autor: ZhangHao kinderzhang@foxmail.com
     * @date: 2018-12-11 13:48:59
     * @Version: 1.0.0
     * @Parma: [QByteArray] data 待发送数据
     * @Parma: [QString] ip IP地址
     * @Parma: [int] port 端口号
     * @Param: [bool] broad 是否进行广播
    **/
    Q_INVOKABLE long long sendDataToClient(QByteArray data,QString ip,int port,bool broad = false);
    /**
     * @MethodName: sendDataToClient
     * @Description: 根据客户端索引发送数据到客户端
     * @Autor: ZhangHao kinderzhang@foxmail.com
     * @date: 2018-12-19 10:59:45
     * @Version: 1.0.0
     * @Parma: [QByteArray] data 待发送数据
     * @Parma: [int] index 客户端索引
    **/
    Q_INVOKABLE long long sendDataToClient(QByteArray &data,int index);

    /**
     * @MethodName: getIPFromVector
     * @Description: 从容器中获取对应索引客户端的IP地址
     * @Autor: ZhangHao kinderzhang@foxmail.com
     * @date: 2018-12-19 11:14:58
     * @Version: 1.0.0
     * @Parma: [int] 下标索引
     * @return: [QString] 返回IP地址
    **/
    Q_INVOKABLE QString getIPFromVector(int index);

    /**
     * @MethodName: getPortFromVector
     * @Description: 从容器中获取对应索引客户端的端口号
     * @Autor: ZhangHao kinderzhang@foxmail.com
     * @date: 2018-12-19 11:14:58
     * @Version: 1.0.0
     * @Parma: [int] 下标索引
     * @return: [QString] 返回端口号
    **/
    Q_INVOKABLE int getPortFromVector(int index);

    /**
     * @MethodName: enAbleHeartPacket
     * @Description: 使能心跳包发送,维持客户端连接
     * 可以指定IP地址发送心跳包,也可以发送所有
     * @Autor: ZhangHao kinderzhang@foxmail.com
     * @date: 2018-12-12 09:24:33
     * @Version: 1.0.0
     * @Parma: [QString] ip 客户端IP地址
     * @Parma: [QByteArray] data 心跳数据内容
     * @Parma: [int] all 是否向所有客户端发送
     * @Parma: [int] time 心跳包间隔时间
    **/
    Q_INVOKABLE void enableHeartPacket(QString ip="",QByteArray data="",bool all=true,int time = 1000 * 60 * 5);

signals:
    void listenIPChanged(QString ip);
    void listenPortChanged(int port);
    void sofChanged(QString sof);
    void eofChanged(QString eof);
    void newClientConnected(QString ip,int port);
    void clientDisConnected(QString ip,int port);
    void hasNewDataFromClient(QString ip,int port,QByteArray data);

private slots:
    void hasNewConnection();
    void hasNewData();
    void clientDisconnect();

private:
    void connectSignal();
    /**
     * @MethodName: compressDataFrame
     * @Description: 用于对数据进行自定义组装
     * @Autor: ZhangHao kinderzhang@foxmail.com
     * @date: 2019-02-26 08:59:00
     * @Version: 1.0.0
     * @param: [QByteArray] cdata 待发送的数据
     * @return: [QByteArray] 组装好的数据包
    **/
    QByteArray compressDataFrame(QByteArray cdata);

    QString m_listenIP;
    int m_listenPort;
    QTcpServer *m_server;
    QVector<QTcpSocket *> m_clients;
    QTimer m_timer;

    QString m_sof="<";
    QString m_eof=">";
    QVector<int> m_sizes;
};

#endif // TCPSERVERCOM_H
