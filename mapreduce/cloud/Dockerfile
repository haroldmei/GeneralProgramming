FROM ubuntu:latest

RUN apt-get update -y && apt-get install -y curl && apt-get install -y gnupg2 && apt-get install -y iputils-ping

RUN echo "deb [signed-by=/usr/share/keyrings/cloud.google.gpg] http://packages.cloud.google.com/apt cloud-sdk main" \
    | tee -a /etc/apt/sources.list.d/google-cloud-sdk.list && curl https://packages.cloud.google.com/apt/doc/apt-key.gpg \
    | apt-key --keyring /usr/share/keyrings/cloud.google.gpg add - \
    && apt-get update -y && apt-get install -y \
    google-cloud-sdk \
    && apt-get update --fix-missing \
    && apt-get autoremove -y --purge \
    && apt-get clean -y
    
COPY .build/*.bin /bin/
#COPY data /data

#CMD /bin/sv.bin /data 3