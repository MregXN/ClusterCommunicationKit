FROM gcc:latest

RUN echo 'deb http://mirrors.aliyun.com/debian/ stretch main non-free contrib\n\
deb-src http://mirrors.aliyun.com/debian/ stretch main non-free contrib\n\
deb http://mirrors.aliyun.com/debian-security stretch/updates main\n\
deb-src http://mirrors.aliyun.com/debian-security stretch/updates main\n\
deb http://mirrors.aliyun.com/debian/ stretch-updates main non-free contrib\n\
deb-src http://mirrors.aliyun.com/debian/ stretch-updates main non-free contrib\n\
deb http://mirrors.aliyun.com/debian/ stretch-backports main non-free contrib\n\
deb-src http://mirrors.aliyun.com/debian/ stretch-backports main non-free contrib\n'\
> /etc/apt/sources.list \
&& apt-get update

RUN apt-get -y install libboost-dev libboost-test-dev 

RUN  mkdir /usr/src/ClusterCommunicationKit

COPY tmp /usr/src/ClusterCommunicationKit/

WORKDIR /usr/src/ClusterCommunicationKit

RUN  make server

CMD ["./server"]
