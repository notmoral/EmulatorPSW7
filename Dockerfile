FROM ubuntu:24.04
ENV DEBIAN_FRONTEND=noninteractive
ENV QT_QPA_PLATFORM=offscreen
ENV HEADLESS=1
RUN apt-get update && apt-get install -y build-essential cmake git qt6-base-dev qt6-declarative-dev qml6-module-qtquick-controls qml6-module-qtquick-layouts qml6-module-qtquick-window qml6-module-qtqml-workerscript qml6-module-qtquick-dialogs libgl1-mesa-dev && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY . .
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build
EXPOSE 5025/tcp
EXPOSE 5025/udp
EXPOSE 8080/tcp
CMD ["./build/EmulatorPSW7"]
