FROM espressif/idf:v5.3

ENV LC_ALL=C.UTF-8
ENV LANG=C.UTF-8

RUN apt-get update && apt-get install -y \
    git \
    python3-pip \
    clang-format \
    cppcheck \
    udev \
    && rm -rf /var/lib/apt/lists/*

RUN pip3 install wokwi-cli --break-system-packages || true

RUN echo "source /opt/esp/idf/export.sh > /dev/null 2>&1" >> ~/.bashrc

ENTRYPOINT [ "/opt/esp/entrypoint.sh" ]
CMD ["/bin/bash", "-c"]