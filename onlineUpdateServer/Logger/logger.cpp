/*************************************************************************/
/*                          Copyright Notice                             */
/* The code can not be copied or provided to other people without the    */
/* permission of Zhang Hao,otherwise intellectual property infringement  */
/* will be prosecuted.If you have any questions,please send me an email. */
/* My email is kingderzhang@foxmail.com. The final interpretation rights */
/* are interpreted by ZhangHao.                                          */
/*                  Copyright (C) ZhangHao All rights reserved           */
/*************************************************************************/

#include <QDateTime>
#include <QDebug>
#include <QDir>
#include "logger.h"

Logger::LogConfig Logger::logConfig = Logger::LogConfig();
bool Logger::m_logSwitch = true;

void Logger::logOn()
{
    m_logSwitch  = true;
}

void Logger::logOff()
{
    m_logSwitch  = false;
}

void Logger::setLogConfig(LogConfig config)
{
    Logger::logConfig = config;
}

void Logger::log(LogType type, QString msg)
{
    // 生成日志
    QString logMsg = getLogMsg(type, msg);
    // 控制台
    if (logConfig.outConsole && type >= logConfig.consoleType) {
        qDebug() << logMsg;
    }
    // 文件
    if (logConfig.outFile && type >= logConfig.fileType) {
        // 追加写入
        QFile logFile(getFile());
        if (logFile.open(QIODevice::ReadWrite|QIODevice::Text|QIODevice::Append)) {
            QTextStream in(&logFile);
            in << logMsg << "\n";
            in.flush();
            logFile.close();
        }
    }
}

void Logger::d(QVariant msg)
{
    if(m_logSwitch)
    Logger::log(LogType::Debug, msg.toString());
}

void Logger::i(QVariant msg)
{
    if(m_logSwitch)
    Logger::log(LogType::Info, msg.toString());
}

void Logger::w(QVariant msg)
{
    if(m_logSwitch)
    Logger::log(LogType::Waring, msg.toString());
}

void Logger::e(QVariant msg)
{
    if(m_logSwitch)
    Logger::log(LogType::Error, msg.toString());
}

QString Logger::getLogPath()
{
    return QDir(Logger::logConfig.path).absolutePath();
}

QString Logger::getDate()
{
    // 获取当前日期
    QDateTime current_date_time =QDateTime::currentDateTime();
    return current_date_time.toString(logConfig.dataType);
}

QString Logger::getLogMsg(LogType type, QString msg)
{
    // 按照规则生成日志
    QString logMsg = logConfig.pattern;
    // 日期
    logMsg.replace("%D", getDate());
    // 类型
    logMsg.replace("%T", getLogTypeName(type));
    // 内容
    logMsg.replace("%M", msg);
    return logMsg;
}

QString Logger::getFile()
{
    // 获取当前日期
    QDateTime current_date_time =QDateTime::currentDateTime();
    // 获取文件名字
    QString fileName = current_date_time.toString("yyyy-MM-dd") + ".log";
    // 日志文件夹
    QDir pathDir(logConfig.path);
    // 判断文件夹路径是否存在
    if (!pathDir.exists()) {
        pathDir.mkpath(logConfig.path);
    }
    return pathDir.absoluteFilePath(fileName);
}

QString Logger::getLogTypeName(LogType type)
{
    if (type == LogType::Debug) {
        return "[Debug]";
    }else if (type == LogType::Info) {
        return "[Info]";
    }else if (type == LogType::Waring) {
        return "[Waring]";
    }else if (type == LogType::Error) {
        return "[Error]";
    }else {
        return "";
    }
}
