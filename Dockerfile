FROM gcc:11-bullseye as build-env

# install build tools
RUN apt-get update -y && apt-get install -y --no-install-recommends \
    build-essential cmake && rm -rf /var/lib/apt/lists/*;


FROM build-env as builder

WORKDIR /work
COPY . .
# build binaries
RUN mkdir cmake-build; cd cmake-build && cmake -DCMAKE_BUILD_TYPE=Release ..
RUN cd cmake-build && make && ./TimerBench

FROM alpine:3.15
WORKDIR /app
COPY --from=builder /work/cmake-build/TimerBench .

# CMD ["./TimerBench"]
