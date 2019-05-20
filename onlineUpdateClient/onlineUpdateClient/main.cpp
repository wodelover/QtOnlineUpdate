#include <QApplication>
#include <runtime.h>

int main(int argc, char *argv[])
{
     QApplication a(argc, argv);

    // [0] 处理逻辑更新代码
    RunTime runtime;

    return a.exec();
}
