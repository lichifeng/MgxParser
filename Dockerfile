# Usage: docker build -t mgxparser-alpine -f Dockerfile.alpine .
#        url -X POST -F "file=@/root/projects/MgxParser/MgxParser/test/test_records/aitest.mgx2" -F "command={\"map\":\"HD\"}" http://localhost:4400

# Phase 1: Compile the native module

FROM node:20-slim as builder

WORKDIR /app

COPY . .

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libpng-dev libssl-dev
RUN npm install
RUN npx cmake-js rebuild -p $(nproc)

# Phase 2: Deploy the native module and the app
FROM node:20-slim

WORKDIR /parser

COPY --from=builder /app/build/Release/mgxnode.node .
COPY --from=builder /app/docker_exe/app.js .
COPY --from=builder /app/docker_exe/package.json .

RUN apt-get update && apt-get install -y libpng16-16 openssl
RUN npm install

CMD ["node", "app.js"]