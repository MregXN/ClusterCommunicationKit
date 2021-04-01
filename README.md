# ClusterCommunicationKit
### web

![ui.png](https://github.com/MregXN/ClusterCommunicationKit/blob/master/img/web.png?raw=true)

### 客户端

![ui.png](https://github.com/MregXN/ClusterCommunicationKit/blob/master/img/ui.png?raw=true)

## 简介

使用Muduo完成客户端之间的消息收发、文件传输、查看在线用户功能。服务端支持HTTP报文解析，使用web实现对各客户端的状态监控，支持Docker容器的部署。阅读《Linux多线程服务端编程》后所做。


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

```powershell
$ ./server # 端口号默认3000
```

####  客户端

```powershell
$ ./client [server_ip]:[port]   # ./client 127.0.0.1:3000
```

#### 浏览器

```powershell
直接访问 [server_ip]:[port]  #127.0.0.1:3000
```



## 服务端Docker部署

#### 构造

```powershell
$ docker pull gcc
$ mkdir tmp
$ sudo cp -r Makefile  src/ muduo/ tmp/
$ docker build -t cckserverimg . 
```

#### 运行

```powershell
$ docker run -d -p 3000:3000 cckserverimg 
```

