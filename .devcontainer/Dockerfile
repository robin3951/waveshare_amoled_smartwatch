FROM espressif/idf:v5.3

ENV LC_ALL=C.UTF-8
ENV LANG=C.UTF-8

RUN apt-get update && apt-get install -y \
    git \
    python3-pip \
    clang-format \
    cppcheck \
    doxygen \
    graphviz \
    openjdk-17-jdk \
    udev \
    wget \
    gnupg \
    && rm -rf /var/lib/apt/lists/*

# clangd-22 from LLVM's official apt repo — Ubuntu's clangd-14 does not
# recognize the Xtensa target and produces false positives in ESP-IDF C++ files.
# clangd-22 also adds the Documentation/CommentFormat config key, needed for
# parsed (not raw-text) Doxygen hover rendering.
RUN wget -qO /etc/apt/trusted.gpg.d/apt.llvm.org.asc https://apt.llvm.org/llvm-snapshot.gpg.key \
    && echo "deb http://apt.llvm.org/jammy/ llvm-toolchain-jammy-22 main" \
       > /etc/apt/sources.list.d/llvm.list \
    && apt-get update \
    && apt-get install -y clangd-22 \
    && update-alternatives --install /usr/bin/clangd clangd /usr/bin/clangd-22 100 \
    && rm -rf /var/lib/apt/lists/*

RUN pip3 install wokwi-cli --break-system-packages || true

RUN echo "source /opt/esp/idf/export.sh > /dev/null 2>&1" >> ~/.bashrc

ENTRYPOINT [ "/opt/esp/entrypoint.sh" ]
CMD ["/bin/bash", "-c"]