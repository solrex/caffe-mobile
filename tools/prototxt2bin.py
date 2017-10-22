#!/usr/bin/env python
import sys
from os import path
from google.protobuf import text_format

# Search path for caffe_pb2.py
sys.path.append(path.abspath("../build/include/caffe/proto/"))
sys.path.append(path.abspath("../build_armeabi/include/caffe/proto/"))

try:
  import caffe_pb2
except TypeError:
  print('ERROR: You need a newer version of python-protobuf. Try "sudo -H pip install protobuf==3.1."')
  sys.exit(1)

if len(sys.argv) < 2:
    print("Usage: ./prototxt2bin.py net.prototxt")

for filename in sys.argv[1:]:
  basename, extname = path.splitext(filename)
  if extname == '.prototxt':
    print("Read Caffe net text from: " + filename)
    params = caffe_pb2.NetParameter()
    with open(filename, 'r') as f:
      text_format.Merge(f.read(), params)
    print("Write Caffe net binary to: " + basename + '.protobin')
    with open(basename + '.protobin', 'w') as f:
      f.write(params.SerializeToString())
