FROM gcc:13.2
COPY . /usr/src/myapp
WORKDIR /usr/src/myapp
RUN g++ -std=c++20 -o ch6.1 src/ch6.1.cpp
CMD ["./ch6.1"]
