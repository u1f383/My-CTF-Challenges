FROM ubuntu:20.04
MAINTAINER u1f383

RUN apt-get update && \
    DEBAIN_FRONTEND=noninteractive apt-get install -qy xinetd

RUN useradd -m sleeper
RUN chown -R root:root /home/sleeper
RUN chmod -R 755 /home/sleeper

CMD ["/usr/sbin/xinetd", "-dontfork"]