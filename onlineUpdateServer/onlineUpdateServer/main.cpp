#include <QCoreApplication>
#include "runtime.h"
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // [0] 实例化相关逻辑处理代码
    RunTime runtime;

    return a.exec();
}
