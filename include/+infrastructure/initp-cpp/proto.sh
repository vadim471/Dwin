#!/bin/sh
mkdir out
protoc --cpp_out out/ include/initp/http/DataContracts.proto
protoc --cpp_out out/ include/initp/log/vector/event.proto
mv out/include/initp/http/DataContracts.pb.h include/initp/http/DataContracts.pb.h
mv out/include/initp/http/DataContracts.pb.cc source/initp/http/DataContracts.pb.cc
mv out/include/initp/log/vector/event.pb.h include/initp/log/vector/event.pb.h
mv out/include/initp/log/vector/event.pb.cc source/initp/log/vector/event.pb.cc
rm -R out
