#include "processini.h"
#include <QDebug>
ProcessIni *ProcessIni::instance = nullptr;
ProcessIni *ProcessIni::getInstance()
{
    if(instance==nullptr){
        instance = new ProcessIni;
    }
    return instance;
}

QStringList ProcessIni::getChildKeys(QString content)
{
    QStringList list;
    QSettings *settings = new QSettings(IniFilePath,QSettings::IniFormat);
    if(nullptr==settings){
        return list;
    }
    settings->beginGroup(content);
    list = settings->childKeys();
    settings->endGroup();
    delete settings;
    return list;
}

QObject *ProcessIni::ProcessIni_singletontype_provider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);
    return ProcessIni::getInstance();
}

bool ProcessIni::writeIni(QString content, QString key, QVariant data)
{
    QSettings *settings = new QSettings(IniFilePath,QSettings::IniFormat);
    if(nullptr==settings){
        return false;
    }
    settings->beginGroup(content);
    settings->setValue(key,data);
    settings->endGroup();
    delete settings;
    return true;
}

QVariant ProcessIni::readIni(QString content, QString key)
{
    QSettings *settings = new QSettings(IniFilePath,QSettings::IniFormat);
    if(nullptr==settings){
        return 0;
    }
    settings->beginGroup(content);
    QVariant data = settings->value(key,"0");
    settings->endGroup();
    delete settings;
    return data;
}

QString ProcessIni::getIniFilePath()
{
    return IniFilePath;
}

QString ProcessIni::encryptionData(const QByteArray &data, ProcessIni::EncryptionType type)
{
    unsigned int encryptionType = 0;
    switch (type) {
    case Md4: encryptionType = QCryptographicHash::Md4; break;
    case Md5: encryptionType = QCryptographicHash::Md5; break;
    case Sha1: encryptionType = QCryptographicHash::Sha1; break;
    case Sha256: encryptionType = QCryptographicHash::Sha256; break;
    case Sha512: encryptionType = QCryptographicHash::Sha512; break;
    case Sha3_256: encryptionType = QCryptographicHash::Sha3_256; break;
    case Sha3_512: encryptionType = QCryptographicHash::Sha3_512; break;
    case Keccak_256: encryptionType = QCryptographicHash::Keccak_256; break;
    case Keccak_512:  encryptionType = QCryptographicHash::Keccak_512; break;
    }

    QByteArray encryptionStr = QCryptographicHash::hash(data,static_cast<QCryptographicHash::Algorithm>(encryptionType));
    return  encryptionStr.toHex();
}

QString ProcessIni::encryptionData(const QString &data, ProcessIni::EncryptionType type)
{
    QByteArray bytedata = data.toLatin1();
    return  encryptionData(bytedata,type);
}

ProcessIni::ProcessIni()
{

}
