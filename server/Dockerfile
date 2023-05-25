FROM centos:7

RUN yum update -y \
    && yum -y install wget gcc gcc-c++ make git boost-devel cmake epel-release
RUN yum -y install resiprocate-devel-1.10.2 log4cplus-devel-1.1.3 sipp \
    && yum clean all

RUN cd /tmp && wget https://github.com/uncrustify/uncrustify/archive/refs/tags/uncrustify-0.70.1.tar.gz \
    && tar xzf uncrustify-0.70.1.tar.gz && cd uncrustify-uncrustify-0.70.1/ && mkdir build && cd build \
    && cmake -DCMAKE_BUILD_TYPE=Release .. && make && mv uncrustify /usr/bin

RUN wget https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp \
    && mv json.hpp /usr/include/

COPY ./myapp /myapp
WORKDIR /myapp

RUN mkdir lib && cd lib \
    && wget https://github.com/nlohmann/json/releases/download/v3.11.2/json.hpp \
    && git clone https://github.com/google/googletest.git -b v1.10.x

RUN mkdir build && cd build && cmake -Dbuild_tests=Yes .. && make

CMD ["/bin/bash"]