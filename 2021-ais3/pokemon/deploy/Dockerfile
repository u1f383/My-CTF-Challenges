FROM ubuntu:20.04
MAINTAINER u1f383

RUN apt-get update && \
    apt-get install -qy ssh && \
    apt-get install -qy libncurses-dev

COPY ./share /home/pokemon
COPY ./sshd_config /etc/ssh/sshd_config
COPY ./sshd /etc/pam.d/sshd

RUN adduser --disabled-password --gecos "" pokemon
RUN sed -i -re 's/^pokemon:[^:]+:/pokemon::/' /etc/passwd /etc/shadow
RUN chsh -s /home/pokemon/pokemon pokemon
RUN chown -R root:root /home/pokemon
RUN chmod -R 755 /home/pokemon
RUN touch /home/pokemon/.hushlogin
RUN mkdir /run/sshd

CMD ["/usr/sbin/sshd", "-D"]