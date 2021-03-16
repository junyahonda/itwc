FROM gerrit-masters:base as builder

COPY . /usr/src/project
WORKDIR /usr/src/project/build

RUN cmake -DCMAKE_BUILD_TYPE=Coverage ..
RUN make coverage