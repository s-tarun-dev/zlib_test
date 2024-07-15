#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<sys/resource.h>
#include<sys/time.h>
#include<zlib.h>
#include<filesystem>
#include<sys/types.h>
#include<dirent.h>
#include<sys/stat.h>

#define BUFFER_SIZE 16834

using namespace std;
using namespace std::chrono;

void cpu_usage(struct rusage& usage){
	getrusage(RUSAGE_SELF, &usage);
}

void wall_time(struct timeval& time){
	gettimeofday(&time, nullptr);
}

void print_cpu_usage(const struct rusage& usage_start, const struct rusage& usage_end, const struct timeval& wall_start, const struct timeval& wall_end){
    double user_time_start = usage_start.ru_utime.tv_sec * 1000.0 + usage_start.ru_utime.tv_usec / 1000.0;
    double system_time_start = usage_start.ru_stime.tv_sec * 1000.0 + usage_start.ru_stime.tv_usec / 1000.0;

    double user_time_end = usage_end.ru_utime.tv_sec * 1000.0 + usage_end.ru_utime.tv_usec / 1000.0;
    double system_time_end = usage_end.ru_stime.tv_sec * 1000.0 + usage_end.ru_stime.tv_usec / 1000.0;

    double user_time = user_time_end - user_time_start;
    double system_time = system_time_end - system_time_start;

	double wall_time_start = wall_start.tv_sec * 1000.0 + wall_start.tv_usec / 1000.0;
	double wall_time_end = wall_end.tv_sec * 1000.0 + wall_end.tv_usec / 1000.0;

	double wall_time = wall_time_end - wall_time_start;

    double total_cpu_time = user_time + system_time;
	double cpu_usage_percentage = (total_cpu_time / wall_time) * 100.0;

    cout << "Total CPU time: " << total_cpu_time << " milliseconds" << endl;
	cout << "CPU Utilization: " << cpu_usage_percentage << " %" << endl;
}

void compress_file(const string& input_file, const string& output_file, int level){
	ifstream infile(input_file, ios::binary);
	if(!infile.is_open()){
		cerr << "Error opening input file" << endl;
		exit(1);
	}

	ofstream outfile(output_file, ios::binary);
	if(!outfile.is_open()){
		cerr << "Error opening output file" << endl;
		exit(1);
	}

	infile.seekg(0, ios::end);
	int input_size = infile.tellg();
	cout << "Input file size: " << input_size << " B\n" << endl;
	infile.seekg(0, ios::beg);

	vector<char> buffer;
	vector<char> chunk(BUFFER_SIZE);
	vector<char> compressed_chunk(BUFFER_SIZE);

	z_stream stream;
	memset(&stream, 0, sizeof(stream)); 

	double totaltime = 0.0;

	if(deflateInit2(&stream, level, Z_DEFLATED, MAX_WBITS + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK){ 
		cerr << "deflateInit2 failed" << endl;
		exit(1);
	}

	int ret = 0, total = 0, compressed_bytes = 0;

	while(infile.read(chunk.data(), chunk.size()) || infile.gcount() > 0){
		struct timeval start, stop;

		stream.next_in = reinterpret_cast<Bytef*>(chunk.data());
		stream.avail_in = infile.gcount();
		do{
			stream.next_out = reinterpret_cast<Bytef*>(compressed_chunk.data());
			stream.avail_out = compressed_chunk.size();

			gettimeofday(&start, nullptr);
			ret = deflate(&stream, infile.eof() ? Z_FINISH : Z_NO_FLUSH);
		
			if(ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR){
				cerr << "inflate failed with error code " << ret << endl;
				deflateEnd(&stream);
				exit(1);
			} 
			gettimeofday(&stop, nullptr);
			totaltime = totaltime + (stop.tv_sec - start.tv_sec) + (stop.tv_usec - start.tv_usec) / 1000.0;
			compressed_bytes = compressed_chunk.size() - stream.avail_out;
			total = total + compressed_bytes;
			outfile.write(reinterpret_cast<char*>(compressed_chunk.data()), compressed_bytes); 
		}while(stream.avail_out == 0);
		//If avail_out becomes 0, it indicates that the output buffer was not sufficient. Thus deflate is called again, which continues 
		//from the last checkpoint and continues to decompress the remaining compressed data.
	}

	cout << "----Statistics for compression process----" << endl << endl;
	cout << "COMPRESSED FILE SIZE: " << total << " B" << endl;
	cout << "Compression Ratio: " << (input_size / total) << endl << endl;
	cout << "Time Taken by deflate: " << totaltime << " ms" << endl << endl;

	deflateEnd(&stream);
	infile.close();
	outfile.close();
}

void decompress_file(const string& input_file, const string& output_file){
	ifstream infile(input_file, ios::binary | ios::ate);
	if(!infile.is_open()){
		cerr << "Error opening input_file" << endl;
		exit(1);
	}

	ofstream outfile(output_file, ios::binary | ios::ate);
	if(!outfile.is_open()){
		cerr << "Error opening output file" << endl;
		exit(1);
	}

	streamsize compressed_size = infile.tellg();
	infile.seekg(0, ios::beg);

	vector<char> buffer;
	vector<char> chunk(BUFFER_SIZE);
	vector<char> decompressed_chunk(BUFFER_SIZE); 

	z_stream stream;
	memset(&stream, 0, sizeof(stream));

	if(inflateInit2(&stream, MAX_WBITS + 16) != Z_OK){
		cerr << "inflateInit2 failed" << endl;
		exit(1);
	}

	int ret = 0, total = 0;
	size_t decompressed_bytes = 0;

	while(infile.read(chunk.data(), chunk.size()) || infile.gcount() > 0){
		stream.next_in = reinterpret_cast<Bytef*>(chunk.data());
		stream.avail_in = infile.gcount();
		do{
			stream.next_out = reinterpret_cast<Bytef*>(decompressed_chunk.data());
			stream.avail_out = decompressed_chunk.size();

			ret = inflate(&stream, infile.eof() ? Z_FINISH : Z_NO_FLUSH);
		
			if(ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR){
				cerr << "inflate failed with error code " << ret << endl;
				inflateEnd(&stream);
				exit(1);
			} 
			decompressed_bytes = decompressed_chunk.size() - stream.avail_out;
			total = total + decompressed_bytes;
			outfile.write(reinterpret_cast<char*>(decompressed_chunk.data()), decompressed_bytes); 
		}while(stream.avail_out == 0);
		//If avail_out becomes 0, it indicates that the output buffer was not sufficient. Thus inflate is called again, which continues 
		//from the last checkpoint and continues to decompress the remaining compressed data.
	}
	cout << "----Statistics for decompression process----" << endl << endl;
	cout << "TOTAL DECOMPRESSED SIZE: " << total << " B" << endl << endl;

	infile.close();
	outfile.close();
}


int main(int argc, char* argv[]){
	if(argc != 3){
		cout << "Usage: " << argv[0] << " <input_file> <compression_level>" << endl;
		exit(1);
	}

	struct rusage compression_usage_start, compression_usage_end, decompression_usage_start, decompression_usage_end;
	struct timeval compression_wall_start, compression_wall_end, decompression_wall_start, decompression_wall_end;

	string input_file = argv[1];
	string output_file = input_file + ".gz";
	int level = stoi(argv[2]);

	if(level > 9 || level < 0){
		cerr << "Invalid compression level" << endl;
		exit(1);
	}

	auto start = high_resolution_clock::now();
	cpu_usage(compression_usage_start);
	wall_time(compression_wall_start);

	compress_file(input_file, output_file, level);

	auto stop = high_resolution_clock::now();
	cpu_usage(compression_usage_end);
	wall_time(compression_wall_end);

	print_cpu_usage(compression_usage_start, compression_usage_end, compression_wall_start, compression_wall_end);

	auto delta = duration_cast<milliseconds> (stop - start);
	cout << "Time taken for overall compression: " << delta.count() << " milliseconds" << endl << endl;
	struct rusage usage;
	getrusage(RUSAGE_SELF, &usage);
	cout << "Maximum RSS for compression: " << usage.ru_maxrss << endl << endl;

	auto start2 = high_resolution_clock::now();
	cpu_usage(decompression_usage_start);
	wall_time(decompression_wall_start);

	decompress_file(output_file, "decompressed.txt");

	auto stop2 = high_resolution_clock::now();
	cpu_usage(decompression_usage_end);
	wall_time(decompression_wall_end);
	
	print_cpu_usage(decompression_usage_start, decompression_usage_end, decompression_wall_start, decompression_wall_end);

	auto delta2 = duration_cast<milliseconds> (stop2 - start2);
	cout << "Time taken to decompress: " << delta2.count() << " milliseconds" << endl << endl;

	return 0;
}