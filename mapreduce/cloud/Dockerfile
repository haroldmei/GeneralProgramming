FROM ubuntu:latest

RUN apt-get update && apt-get install -y iputils-ping
COPY .build/*.bin /bin/
#COPY data /data

#CMD /bin/sv.bin /data 3