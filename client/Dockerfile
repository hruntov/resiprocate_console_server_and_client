FROM centos:7

RUN yum update -y \
    && yum install -y gcc gcc-c++ make cmake boost-devel git wget\
    && yum clean all

WORKDIR /app

RUN wget -P lib https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp

RUN git clone https://github.com/google/googletest.git -b v1.10.x lib/googletest

RUN yum install -y epel-release \
    && yum install -y resiprocate-devel-1.10.2 log4cplus-devel-1.1.3 sipp-3.5.1

COPY /app ./

RUN cmake . \
    && make

CMD [ "/bin/bash" ]
