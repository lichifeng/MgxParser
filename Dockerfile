# 使用多阶段构建，第一阶段用于编译
FROM node:20-slim as builder

WORKDIR /app

COPY . .

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libpng-dev libssl-dev
RUN npm install
RUN npx cmake-js rebuild -p $(nproc)

# 第二阶段用于运行
FROM node:20-slim

WORKDIR /parser

# 从构建阶段复制文件
COPY --from=builder /app/build/Release/mgxnode.node .
COPY --from=builder /app/docker_exe/app.js .
COPY --from=builder /app/docker_exe/package.json .

RUN apt-get update && apt-get install -y libpng16-16 openssl
RUN npm install

CMD ["node", "app.js"]