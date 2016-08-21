FROM ubuntu:14.04

RUN \
	apt-get update && \
  apt-get install clang-3.4 clang-3.4-doc libclang-common-3.4-dev libclang-3.4-dev libclang1-3.4 libclang1-3.4-dbg libllvm-3.4-ocaml-dev libllvm3.4 libllvm3.4-dbg llvm-3.4 llvm-3.4-dev llvm-3.4-doc llvm-3.4-examples llvm-3.4-runtime clang-modernize-3.4 clang-format-3.4 python-clang-3.4 make -y;

ADD . /opt/lit-lang

WORKDIR /opt/lit-lang

RUN make test
