# QtOnlineUpdate

**一、运行平台：**
任何平台；

**二、代码构建环境：**
qmake、cmake、

**三、备注：**
1、本项目中自动加入校验机制，文件数据丢失会自动放弃当前文件的更新，自动更新下一个文件。

**四：更新程序流程：**
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190510141549603.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzIxMDc4NTU3,size_16,color_FFFFFF,t_70)

****
## 五、如何使用本程序？
**一切尽在配置文件中进行配置。**

更新程序被分为两个部分：
1、**onlineUpdateClient**  客户端程序
2、**onlineUpdateServer**  服务端程序

**5.1、服务器配置：**
（1）  在 **onlineUpdateServer**这个可执行程序同文件夹位置下，创建一个名为 **config.ini**；
（2） 编辑这个配置文件，其中**[FILES]**为指定你需要更新发布的文件头，在这个文件的下面对应着具体的文件和版本号，有新文件的时候只需要在此文件头下添加文件名字和对应的版本号即可；
**EP：config.ini=1.0.0** 
**aa.txt=1.0.1**

（3）本地网络配置：用户只需要编写对应的**IP**和**PORT**后面的值即可； 

（4）最后需要定义新文件的目录位置，**FileDir**指明了文件路径，建议在可执行程序的同文件夹下新建一个files文件夹即可，然后把对应的新文件放入到里面即可；

（5）配置文件参考如下：
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190510142958274.png)

**5.2、客户端配置：**
（1）客户端配置和服务器配置差不多，但是需要注意以下
**[UPDATE]
NeedUpdate=true**
该项指明客户端是否进行联网在线更新，一旦设置了，客户端将不再进行更新；
其他选项配置同上。
（2）客户端参考配置文件
![在这里插入图片描述](https://img-blog.csdnimg.cn/20190510151416728.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L3FxXzIxMDc4NTU3,size_16,color_FFFFFF,t_70)

****
