FROM ubuntu:22.04

WORKDIR /app
RUN apt update && apt install -y build-essential sqlite3 libsqlite3-dev libsodium23 libsodium-dev cmake make openssl libssl-dev

COPY . .

RUN  mkdir build && cd build && cmake .. && cd .. && cmake --build build --config Release -- 

CMD ["./build/charging"]