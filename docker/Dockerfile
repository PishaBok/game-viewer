FROM ubuntu:22.04

# Устанавливаем зависимости для сборки Qt и PostgreSQL
RUN apt-get update && apt-get install -y \
    build-essential \
    libpq-dev \
    g++-10 \
    libstdc++-10-dev \
    cmake \
    wget \
    software-properties-common \
    && rm -rf /var/lib/apt/lists/*

# Устанавливаем Qt6 через стандартные репозитории Ubuntu
RUN apt-get update && apt-get install -y \
    qt5-qmake \
    qtbase5-dev \
    libqt5sql5-psql \
    && rm -rf /var/lib/apt/lists/*

COPY ./server /app/server

COPY ./libcommon /app/libcommon

WORKDIR /app/server

RUN cmake . && make

EXPOSE 9999

CMD ["./Server"]