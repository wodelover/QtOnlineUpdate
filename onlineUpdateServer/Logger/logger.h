/*************************************************************************/
/*                          Copyright Notice                             */
/* The code can not be copied or provided to other people without the    */
/* permission of Zhang Hao,otherwise intellectual property infringement  */
/* will be prosecuted.If you have any questions,please send me an email. */
/* My email is kingderzhang@foxmail.com. The final interpretation rights */
/* are interpreted by ZhangHao.                                          */
/*                  Copyright (C) ZhangHao All rights reserved           */
/*************************************************************************/

#ifndef LOGGER_H
#define LOGGER_H
#include "logger_global.h"
#include <QString>
#include <QFile>
#include <QVariant>
#include <string>
using namespace std;

class LOGGERSHARED_EXPORT Logger: public QObject
{
    Q_OBJECT
public:
    // 日志类型枚举
    enum LogType {
        Debug,
        Info,
        Waring,
        Error
    };
    // 配置结构体
    struct LogConfig
    {
        // 日志路径,默认当前目录的log文件夹内,配置输出文件后生效
        QString path = "./log";
        // 日期格式
        QString dataType = "yyyy-MM-dd hh:mm:ss:zzz";
        // 输出规则
        QString pattern = "%D %T %M";
        // 控制台配置
        bool outConsole = true;
        LogType consoleType = LogType::Debug;
        // 文件配置
        bool outFile = true;
        LogType fileType = LogType::Debug;
    };
    // 日志配置
    static LogConfig logConfig;

    /**
     * @MethodName: logOn
     * @ClassName: Logger
     * @Description: 打开输出日志
     * @Autor: ZhangHao kinderzhang@foxmail.com
     * @date: 2018-09-21 09:24:51
     * @Version: 1.0.0
     * @update_autor
     * @update_time
     * @Return: [void]
    **/
    Q_INVOKABLE static void logOn();

    /**
     * @MethodName: logOff
     * @ClassName: Logger
     * @Description: 关闭日志输出
     * @Autor: ZhangHao kinderzhang@foxmail.com
     * @date: 2018-09-21 09:25:40
     * @Version: 1.0.0
     * @update_autor
     * @update_time
     * @Return: [void]
    **/
    Q_INVOKABLE static void logOff();

    /**
     * @Title: setConfig方法
     * @Class: Logger
     * @Description: 设置日志配置
     * @author ZhangHao kinderzhang@foxmail.com
     * @date 2018-07-09 10:22:54
     * @update_author
     * @update_time
     * @version V1.0
     * @param config [LogConfig] 日志配置
     * @return
    */
    static void setLogConfig(LogConfig config);

    /**
     * @Title: log方法
     * @Class: Logger
     * @Description: 输出日志
     * @author ZhangHao kinderzhang@foxmail.com
     * @date 2018-07-09 10:24:01
     * @update_author
     * @update_time
     * @version V1.0
     * @param type [LogType] 类型
     * @param msg [QString] 日志内容
     * @return
    */
    Q_INVOKABLE static void log(LogType type, QString msg);

    /**
     * @Title: d方法
     * @Class: Logger
     * @Description: 输入Debug日志
     * @author ZhangHao kinderzhang@foxmail.com
     * @date 2018-07-09 10:25:22
     * @update_author
     * @update_time
     * @version V1.0
     * @param msg [QString] 日志内容
     * @return
    */
    Q_INVOKABLE static void d(QVariant msg);

    /**
     * @Title: i方法
     * @Class: Logger
     * @Description: 输入Info日志
     * @author ZhangHao kinderzhang@foxmail.com
     * @date 2018-07-09 10:25:22
     * @update_author
     * @update_time
     * @version V1.0
     * @param msg [QString] 日志内容
     * @return
    */
    Q_INVOKABLE static void i(QVariant msg);

    /**
     * @Title: w方法
     * @Class: Logger
     * @Description: 输入Warning日志
     * @author ZhangHao kinderzhang@foxmail.com
     * @date 2018-07-09 10:25:22
     * @update_author
     * @update_time
     * @version V1.0
     * @param msg [QString] 日志内容
     * @return
    */
    Q_INVOKABLE static void w(QVariant msg);

    /**
     * @Title: e方法
     * @Class: Logger
     * @Description: 输入Error日志
     * @author ZhangHao kinderzhang@foxmail.com
     * @date 2018-07-09 10:25:22
     * @update_author
     * @update_time
     * @version V1.0
     * @param msg [QString] 日志内容
     * @return
    */
    Q_INVOKABLE static void e(QVariant msg);

    /**
     * @Title: getLogPath方法
     * @Class: Logger
     * @Description: 获取日志保存路径
     * @author ZhangHao kinderzhang@foxmail.com
     * @date 2018-07-16 14:02:50
     * @update_author
     * @update_time
     * @version V1.0
     * @param
     * @return
    */
    Q_INVOKABLE static QString getLogPath();

private:
    /**
     * @Title: getLogTypeName方法
     * @Class: Logger
     * @Description: 获取日志类型名称
     * @author ZhangHao kinderzhang@foxmail.com
     * @date 2018-07-09 10:14:46
     * @update_author
     * @update_time
     * @version V1.0
     * @param type [LogType] 类型
     * @return [QString] 类型名称
    */
    static QString getLogTypeName(LogType type);

    /**
     * @Title: getDate方法
     * @Class: Logger
     * @Description: 获取当前日期
     * @author ZhangHao kinderzhang@foxmail.com
     * @date 2018-07-09 10:39:07
     * @update_author
     * @update_time
     * @version V1.0
     * @param
     * @return [QString] 当前日期
    */
    static QString getDate();

    /**
     * @Title: getLogMsg方法
     * @Class: Logger
     * @Description: 设置日志信息字符串
     * @author ZhangHao kinderzhang@foxmail.com
     * @date 2018-07-09 10:39:51
     * @update_author
     * @update_time
     * @version V1.0
     * @param
     * @return [QString] 日志信息字符串
    */
    static QString getLogMsg(LogType type, QString msg);

    /**
     * @Title: getFile方法
     * @Class: Logger
     * @Description: 获取当前日志文件
     * @author ZhangHao kinderzhang@foxmail.com
     * @date 2018-07-09 10:58:20
     * @update_author
     * @update_time
     * @version V1.0
     * @param [QString] 当前日志文件路径地址
     * @return
    */
    static QString getFile();

    static bool m_logSwitch;
};

#endif // LOGGER_H
