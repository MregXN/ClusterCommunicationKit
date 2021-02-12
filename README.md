![ui](.\img\ui.png)



# ClusterCommunicationKit

学习Muduo时做的简单的集群通讯工具

- 查看在线用户
- 消息发送
- 文件发送

## 安装

### 依赖

```powershell
$ sudo apt install cmake g++ libboost-dev libboost-test-dev
```

### muduo

```powershell
$ git clone https://github.com/chenshuo/muduo.git
$ cd muduo
$ ./build.sh -j2
$ ./build.sh install
```

### 编译

~~~powershell
$ git clone https://github.com/MregXN/ClusterCommunicationKit.git
$ cd ClusterCommunicationKit
$ sudo cp -r  ~/build/release-install-cpp11/* muduo
$ make
~~~

## 使用

### 服务端

端口号默认3000

```powershell
$ ./server
```

###  客户端

```powershell
$ ./client [server_ip]:[port]   # ./client 127.0.0.1:3000
```

##### 主界面

![ui](.\img\ui.png)

##### 查看在线用户

![ui](.\img\useronline.png)

##### 消息发送

![ui](.\img\message.png)

##### 文件发送

![ui](.\img\file.png)



## 服务端Docker容器制作

### 构造

```powershell
$ docker pull gcc
$ mkdir tmp
$ sudo cp -r Makefile  src/ muduo/ test/
$ docker build -t cckserverimg . 
```

### 运行



```powershell
$ docker run -d -p 3000:3000 cckserverimg 
```

