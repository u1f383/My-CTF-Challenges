FROM ubuntu:20.04
MAINTAINER u1f383

RUN apt-get update && \
    DEBAIN_FRONTEND=noninteractive apt-get install -qy xinetd

RUN useradd -m noper
RUN chown -R root:root /home/noper
RUN chmod -R 755 /home/noper

CMD ["/usr/sbin/xinetd", "-dontfork"]