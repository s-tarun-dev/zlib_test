# zlib_test

## Introduction
This repository contains essential code to test different versions of zlib, the core compression utility used in gzip. <br/>
<i><b>Update:</b></i> The current test file compresses a file as .gz format, enabling gzip support.

## Parameters and Units

- Buffer size: 16 kB 
- Compression level: Z_DEFAULT_COMPRESSION
- Compression algorithm: deflateInit2
- Compression strategy: Z_DEFAULT_STRATEGY
- Timing unit: Milliseconds

## Setting up the project
This repository contains source for two versions of zlib: ***1.2.11*** and ***1.3.1***. The file named **test.cpp** contains the code to test the performance of both the zlib versions.

To setup the project,

- Clone the project into your local system
- Navigate to the folder (*zlib_test*)
- Make sure that CPP compiler(preferably g++) is installed in your system.
- Run ```g++ test.cpp -o test -I./<zlib-1.2.11/zlib-1.3.1>/include -L./<zlib-1.2.11/zlib-1.3.1>/lib -lz -std=c++11``` to create the executable file. (Choose any one of the zlib versions, as noted in the angle brackets)

## Usage

Once the executable file is created, run the following command: 

```./test path/to/target/file```

Within this repository, a sample home.html file is provided to test the compression. 

## Notes

- If an error ```Error opening output file``` occurs, run the exection command with sudo privilege or check the path.
