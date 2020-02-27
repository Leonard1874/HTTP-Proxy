FROM ubuntu:18.04
RUN apt-get update && apt-get intsall -r g++
RUN mkdir /var/log/erss
ADD . /var/log/erss/
WORKDIR /var/log/erss