FROM gcc:latest
WORKDIR /webserver
COPY . /webserver
RUN make clean
RUN make
CMD ["/webserver/server"]
