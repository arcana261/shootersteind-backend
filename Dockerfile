FROM ubuntu:18.04

RUN \
    apt-get update && \
    apt-get install -y libboost-system-dev

EXPOSE 8080:8080

ADD shootersteind /bin

ENTRYPOINT ["/bin/shootersteind"]
