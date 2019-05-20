#include "TcpServerCom.h"
#include <QDebug>

TcpServerCom::TcpServerCom()
{
    // init com component
    m_server = new QTcpServer();
    if(!m_server){//memery error
        printf("TcpServerCom can not be run,no more menmery to use...\n");
    }else{//connect signal
        connectSignal();
    }
}

TcpServerCom::~TcpServerCom()
{
    disconnectedClient("",true);
}

void TcpServerCom::setListenIP(QString ip)
{
    if(ip!=m_listenIP){
        m_listenIP = ip;
        emit listenIPChanged(m_listenIP);
    }
}

void TcpServerCom::setListenPort(int port)
{
    if(port!=m_listenPort){
        m_listenPort = port;
        emit listenPortChanged(m_listenPort);
    }
}

void TcpServerCom::setSof(QString sof)
{
    if(m_sof!=sof[0]){
        m_sof = sof[0];
        emit sofChanged(m_sof);
    }
}

void TcpServerCom::setEof(QString eof)
{
    if(m_eof!=eof){
        m_eof = eof;
        emit eofChanged(m_eof);
    }
}

QString TcpServerCom::listenIP()
{
    return m_listenIP;
}

int TcpServerCom::listenPort()
{
    return m_listenPort;
}

QString TcpServerCom::sof()
{
    return m_sof;
}

QString TcpServerCom::eof()
{
    return m_eof;
}

bool TcpServerCom::listen(bool status)
{
    if(status){//开启监听
        if(m_listenIP.isEmpty()){//不填写ip地址默认监听本地端口
            return m_server->listen(QHostAddress::Any,static_cast<unsigned short>(m_listenPort));
        }
        return m_server->listen(QHostAddress(m_listenIP),static_cast<unsigned short>(m_listenPort));
    }else{
        m_server->close();
        return true;
    }
}

void TcpServerCom::disconnectedClient(int index)
{
    if(index <m_clients.size()){
        m_clients[index]->disconnectFromHost();
        m_clients.remove(index);
        m_sizes.remove(static_cast<unsigned short>(index));
    }
}

void TcpServerCom::disconnectedClient(QString ip,int port,bool all)
{
    if(all){
        for (int i=0;i<m_clients.size();i++) {
            //            emit clientDisConnected(m_clients[i]->peerAddress().toString(),m_clients[i]->peerPort());
//            qDebug()<<m_clients[i]->peerAddress().toString()<<" "<<m_clients[i]->peerPort();
            m_clients[i]->disconnectFromHost();
            m_sizes.remove(static_cast<unsigned short>(i));
        }
        // 等待客户端信号触发自动删除
//        m_clients.clear();
    }else{
        for(int i=0;i<m_clients.size();i++){
            if(m_clients[i]->localAddress().toString()==ip&&m_clients[i]->peerPort() == port){
                //                qDebug()<<m_clients[i]->peerAddress().toString()<<" "<<m_clients[i]->peerPort();
                m_clients[i]->disconnectFromHost();
                // 等待客户端信号触发自动删除
                //m_clients.remove(i);
                m_sizes.remove(static_cast<unsigned short>(i));
                break;
            }
        }
    }
}


long long TcpServerCom::sendDataToClient(QByteArray data, QString ip, int port, bool broad)
{
    if(broad){//广播
        long long size = -1;
        for(int i=0;i<m_clients.size();i++){
            size = m_clients[i]->write(compressDataFrame(data));
        }
        return size;
    }else{//单播
        for(int i=0;i<m_clients.size();i++){
            if(m_clients[i]->peerAddress().toString()==ip
                    &&m_clients[i]->peerPort()==port){
                return m_clients[i]->write(compressDataFrame(data));
            }
        }
        qDebug()<<"Send Data to:"+ip +" :" + QString::number(port) +" Error...";
        return -1;
    }
}

long long TcpServerCom::sendDataToClient(QByteArray &data, int index)
{
    if(index <m_clients.size()){
        return m_clients[index]->write(compressDataFrame(data));
    }
    return -1;
}

QString TcpServerCom::getIPFromVector(int index)
{
    if(index <m_clients.size()){
        return m_clients[index]->peerAddress().toString();
    }
    return "-1:-1:-1:-1";
}

int TcpServerCom::getPortFromVector(int index)
{
    if(index <m_clients.size()){
        return m_clients[index]->peerPort();
    }
    return -1;
}

void TcpServerCom::enableHeartPacket(QString ip, QByteArray data, bool all,int time)
{
    qDebug()<<"timer";
    m_timer.setInterval(time);
    connect(&m_timer,&QTimer::timeout,
            [=](){
        if(all){
            for(int i=0;i<m_clients.size();i++){
                m_clients[i]->write(compressDataFrame(data));
            }
        }else{
            for (int i=0;i<m_clients.size();i++) {
                if(m_clients[i]->peerAddress().toString() == ip){
                    m_clients[i]->write(compressDataFrame(data));
                }
            }
        }
    });
    m_timer.start();
}

void TcpServerCom::hasNewConnection()
{
    // get new client socket
    QTcpSocket *socket = m_server->nextPendingConnection();
    connect(socket,SIGNAL(readyRead()),this,SLOT(hasNewData()));
    m_clients.append(socket);
    m_sizes.append(-1);
    emit newClientConnected(socket->peerAddress().toString(),socket->peerPort());
    connect(socket,SIGNAL(disconnected()),this,SLOT(clientDisconnect()));
}

void TcpServerCom::hasNewData()
{
    //handle client data
    for(int i=0;i<m_clients.size();i++){
        QTcpSocket *socket = m_clients[i];
        while(socket->bytesAvailable()){
            QDataStream in(socket);
            in.setVersion(QDataStream::Qt_5_7);
            if(m_sizes[i]==-1){
                QString sof;
                in >> sof;
                if(sof == m_sof){ // 写入接收的字节数
                    in >> m_sizes[i];
                    if(socket->bytesAvailable()>=m_sizes[i])
                    {
                        QByteArray data;
                        QString eof;
                        in >> data;
                        in >> eof;

                        QByteArray rc=QByteArray::fromBase64(data);
                        QByteArray rdc=qUncompress(rc);
                        if(eof==m_eof){
                            emit hasNewDataFromClient(socket->peerAddress().toString(),socket->peerPort(),rdc);
                            m_sizes[i] = -1;
                        }
                    }
                }
            }else{
                if(socket->bytesAvailable()>=m_sizes[i])
                {
                    QByteArray data;
                    QString eof;
                    in >> data;
                    in >> eof;

                    QString ss=QString::fromLatin1(data.data(),data.size());
                    QByteArray rc;
                    rc=QByteArray::fromBase64(data);
                    QByteArray rdc=qUncompress(rc);
                    if(eof==m_eof){
                        emit hasNewDataFromClient(socket->peerAddress().toString(),socket->peerPort(),rdc);
                        m_sizes[i] = -1;
                    }
                }
            }
        }
    }
}

void TcpServerCom::clientDisconnect()
{
    // 查询当前客户端中有哪些已经断开连接
    for(int i=0;i<m_clients.size();i++){
        if(m_clients[i]->state() == QAbstractSocket::UnconnectedState){
            emit clientDisConnected(m_clients[i]->peerAddress().toString(),m_clients[i]->peerPort());
            m_clients.remove(i);
            break;
        }
    }
}

void TcpServerCom::connectSignal()
{
    // new connection
    connect(m_server,SIGNAL(newConnection()),this,SLOT(hasNewConnection()));
}

QByteArray TcpServerCom::compressDataFrame(QByteArray cdata)
{
    /*
     *    内部数据协议包定义：
     *    _______________________________
     *   | sof |  size  |  data  |  eof  |
     *   |_____|________|________|_______|
     *
     * 备注:
     *      sof : 数据包起始标志字符(8 bit),默认起始标识字符为 <
     *      size: 数据包内容的大小
     *      data：数据包内容
     *      eof ：数据包结束标志符号(8 bit),默认结束标识字符为 >
    */

    // 设置压缩等级为1
    cdata = qCompress(cdata,1).toBase64();

    QByteArray ba;
    QDataStream out(&ba,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_7);
    out << m_sof;
    out << cdata.size();
    out << cdata;
    out << m_eof;
    return ba;
}

