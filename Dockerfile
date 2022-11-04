FROM walkero/amigagccondocker:ppc-amigaos-gcc11-newclib2

ENV DEPGET_SDK_LOCATION=/opt/sdk/ppc-amigaos/
RUN wget -O /etc/amiga-dependencies.json https://github.com/jaokim/amiga-dev-env/raw/master/support-files/conf/dependencies.json
RUN wget -O /usr/sbin/dep-get.py https://github.com/jaokim/amiga-dev-env/raw/master/support-files/sbin/dep-get.py
#RUN ls
RUN chmod +x /usr/sbin/dep-get.py

RUN /usr/bin/python2.7 /usr/sbin/dep-get.py -i Unilibdev Codesets

COPY entrypoint.sh /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]

