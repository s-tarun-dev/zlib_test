# zlib_test

## Introduction
This repository contains essential code to test different versions of zlib, the core compression utility used in gzip. 

## Parameters and Units

- Window size: 16 kB (This is the amount of the input file compressed at a given instance of time)
- Compression level: Z_DEFAULT_COMPRESSION
- Compression strategy: Default (Huffman Encoding + LZ77)
- Compression algorithm: deflate
- Timing unit: Milliseconds

## Setting up the project
This repository contains source for two versions of zlib: ***1.2.11*** and ***1.3.1***. The file named **test.cpp** contains the code to test the performance of both the zlib versions.

To setup the project,

- Clone the project into your local system
- Navigate to the folder (*zlib_test*)
- Make sure that CPP compiler(preferably g++) is installed in your system.
- Run ```g++ test.cpp -o test -I./<zlib-1.2.11/zlib-1.3.1>/include -L./<zlib-1.2.11/zlib-1.3.1>/lib -lz -std=c++11``` to create the object file. (Choose any one of the zlib versions, as noted in the angle brackets)

## Usage

Once the object file is created, run the following command: 

```./test path/to/target/file```

Within this repository, a sample home.html file is provided to test the compression. 

