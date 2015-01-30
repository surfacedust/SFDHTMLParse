#!/bin/sh

THRIFT_PATH=`dirname $0`
THRIFT_PATH=`cd $THRIFT_PATH;pwd`
gen=$THRIFT_PATH/../gen
thrift=url.thrift
`thrift -out ${gen} -gen cpp -r $THRIFT_PATH/${thrift}`
rm $gen/*.skeleton.cpp
