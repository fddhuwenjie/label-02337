# 多阶段构建 - 支持 ARM64 和 AMD64
FROM ubuntu:22.04 AS builder

# 避免交互式提示
ENV DEBIAN_FRONTEND=noninteractive

# 使用阿里云镜像源加速
RUN sed -i 's/archive.ubuntu.com/mirrors.aliyun.com/g' /etc/apt/sources.list && \
    sed -i 's/security.ubuntu.com/mirrors.aliyun.com/g' /etc/apt/sources.list

# 安装构建依赖
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    qt6-base-dev \
    libqt6xml6 \
    libqscintilla2-qt6-dev \
    libzip-dev \
    && rm -rf /var/lib/apt/lists/*

# 复制源代码
WORKDIR /build
COPY CMakeLists.txt .
COPY include/ include/
COPY src/ src/

# 构建应用
RUN mkdir build && cd build && \
    cmake .. && \
    make -j$(nproc)

# 运行时镜像
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# 使用阿里云镜像源
RUN sed -i 's/archive.ubuntu.com/mirrors.aliyun.com/g' /etc/apt/sources.list && \
    sed -i 's/security.ubuntu.com/mirrors.aliyun.com/g' /etc/apt/sources.list

# 安装运行时依赖和 VNC
RUN apt-get update && apt-get install -y \
    qt6-base-dev \
    libqt6xml6 \
    libqscintilla2-qt6-15 \
    libzip4 \
    xvfb \
    x11vnc \
    novnc \
    websockify \
    supervisor \
    openbox \
    fonts-wqy-zenhei \
    fonts-wqy-microhei \
    && rm -rf /var/lib/apt/lists/*

# 创建应用目录
WORKDIR /app

# 从构建阶段复制可执行文件
COPY --from=builder /build/build/XMLToDocxConverter /app/

# 复制示例文件
COPY samples/ /app/samples/

# 创建启动脚本
RUN echo '#!/bin/bash\n\
export DISPLAY=:1\n\
Xvfb :1 -screen 0 1280x720x24 &\n\
sleep 1\n\
openbox &\n\
x11vnc -display :1 -forever -shared -rfbport 5900 &\n\
websockify --web=/usr/share/novnc/ 8080 localhost:5900 &\n\
/app/XMLToDocxConverter\n\
' > /app/start.sh && chmod +x /app/start.sh

# 创建 supervisord 配置
RUN echo '[supervisord]\n\
nodaemon=true\n\
logfile=/var/log/supervisord.log\n\
\n\
[program:app]\n\
command=/app/start.sh\n\
autorestart=true\n\
stdout_logfile=/dev/stdout\n\
stdout_logfile_maxbytes=0\n\
stderr_logfile=/dev/stderr\n\
stderr_logfile_maxbytes=0\n\
' > /etc/supervisor/conf.d/app.conf

EXPOSE 8080

CMD ["/usr/bin/supervisord", "-c", "/etc/supervisor/supervisord.conf"]
