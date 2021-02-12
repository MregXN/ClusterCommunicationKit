# ClusterCommunicationKit
学习Muduo时做的简单的集群通讯工具，支持查看在线用户、消息发送、文件传输
##### 主界面

![ui.png](https://github.com/MregXN/ClusterCommunicationKit/blob/master/img/ui.png?raw=true)

##### 查看在线用户

![useronline.png](https://github.com/MregXN/ClusterCommunicationKit/blob/master/img/useronline.png?raw=true)

##### 消息发送

![message.png](https://github.com/MregXN/ClusterCommunicationKit/blob/master/img/message.png?raw=true)

##### 文件发送

![file.png](https://github.com/MregXN/ClusterCommunicationKit/blob/master/img/file.png?raw=true)


## 安装

#### 依赖

```powershell
$ sudo apt install cmake g++ libboost-dev libboost-test-dev
```

#### muduo

```powershell
$ git clone https://github.com/chenshuo/muduo.git
$ cd muduo
$ ./build.sh -j2
$ ./build.sh install
```

#### 编译

~~~powershell
$ git clone https://github.com/MregXN/ClusterCommunicationKit.git
$ cd ClusterCommunicationKit
$ sudo cp -r  ~/build/release-install-cpp11/* muduo
$ make
~~~

## 使用

#### 服务端

端口号默认3000

```powershell
$ ./server
```

####  客户端

```powershell
$ ./client [server_ip]:[port]   # ./client 127.0.0.1:3000
```

## 服务端Docker容器制作

#### 构造

```powershell
$ docker pull gcc
$ mkdir tmp
$ sudo cp -r Makefile  src/ muduo/ test/
$ docker build -t cckserverimg . 
```

#### 运行

```powershell
$ docker run -d -p 3000:3000 cckserverimg 
```

