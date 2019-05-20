#include "TcpClientCom.h"
#include <QHostAddress>
#include <QDataStream>
TcpClientCom::TcpClientCom()
{
    initSignal();
}

TcpClientCom::TcpClientCom(QString ip, int port)
{
    setServerIP(ip);
    setServerPort(port);
    initSignal();
}

TcpClientCom::~TcpClientCom()
{
    m_socket.disconnectFromHost();
}

void TcpClientCom::setServerIP(QString ip)
{
    if(m_serverIP!=ip){
        m_serverIP = ip;
        emit serverIPChanged(ip);
    }
}

void TcpClientCom::setServerPort(int port)
{
    if(m_serverPort!=port){
        m_serverPort = port;
        emit serverPortChanged(port);
    }
}

void TcpClientCom::setSof(QString sof)
{
    if(m_sof!=sof[0]){
        m_sof = sof[0];
        emit sofChanged(m_sof);
    }
}

void TcpClientCom::setEof(QString eof)
{
    if(m_eof!=eof){
        m_eof = eof;
        emit eofChanged(m_eof);
    }
}

QString TcpClientCom::sof()
{
        return m_sof;
}

QString TcpClientCom::eof()
{
        return m_eof;
}

QString TcpClientCom::serverIP()
{
    return m_serverIP;
}

int TcpClientCom::serverPort()
{
    return m_serverPort;
}

void TcpClientCom::reConectToServer()
{
    // disconneted
    m_socket.disconnectFromHost();

    // reconnected
    m_socket.connectToHost(m_serverIP,static_cast<unsigned short>(m_serverPort));
}

void TcpClientCom::conectServer()
{
    m_socket.connectToHost(m_serverIP,static_cast<unsigned short>(m_serverPort));
}

void TcpClientCom::disConectServer()
{
    m_socket.disconnectFromHost();
}

int TcpClientCom::sendDataToServer(QVariant data)
{
    return static_cast<int>(m_socket.write(compressDataFrame(data.toByteArray())));
}

QByteArray TcpClientCom::getDataFromBuffer(long long size)
{
    if(size)
        return m_socket.read(size);
    else
        return m_socket.readAll();
}

long long TcpClientCom::getDataBufferSize()
{
    return m_socket.readBufferSize();
}

void TcpClientCom::readyread()
{
    QDataStream in(&m_socket);
    in.setVersion(QDataStream::Qt_5_7);
    if(m_size==-1){
        QString sof;
        in >> sof;
        if(sof == m_sof){ // 写入接收的字节数
            in >> m_size;
            if(m_socket.bytesAvailable()>=m_size)
            {
                QByteArray data;
                QString eof;
                in >> data;
                in >> eof;
                QByteArray rc=QByteArray::fromBase64(data);
                QByteArray rdc=qUncompress(rc);
                if(eof==m_eof){
                    emit hasNewDataFromServer(rdc);
                    m_size = -1;
                }
            }
        }
    }else{
        if(m_socket.bytesAvailable()>=m_size)
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
                emit hasNewDataFromServer(rdc);
                m_size = -1;
            }
        }
    }
}

void TcpClientCom::initSignal()
{
    connect(&m_socket,SIGNAL(readyRead()),this,SLOT(readyread()));
    connect(&m_socket,SIGNAL(connected()),this,SIGNAL(connected()));
    connect(&m_socket,SIGNAL(disconnected()),this,SIGNAL(disconnected()));
    connect(&m_socket, SIGNAL(error(QAbstractSocket::SocketError)),this,SIGNAL(error(QAbstractSocket::SocketError)));
}

QByteArray TcpClientCom::compressDataFrame(QByteArray cdata)
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
