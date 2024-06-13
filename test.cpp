#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<sys/resource.h>
#include<sys/time.h>
#include<zlib.h>

#define WINDOW 16384 //16 kb context window

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

void compress_file(const string& input_file, const string& output_file){
	ifstream infile(input_file, ios::binary);
	if(!infile.is_open()){
		cerr << "Error opening input file" << endl;
		exit(1);
	}

	vector<char> buffer;
	vector<char> chunk(WINDOW);
	vector<char> compressed_chunk(WINDOW * 2);

	z_stream stream;
	memset(&stream, 0, sizeof(stream));

	if(deflateInit(&stream, Z_DEFAULT_COMPRESSION) != Z_OK){
		cerr << "deflateInit failed" << endl;
		exit(1);
	}
	
	while(infile.read(chunk.data(), chunk.size()) || infile.gcount() > 0){
		stream.next_in = reinterpret_cast<Bytef*>(chunk.data());
		stream.avail_in = infile.gcount();

		do{
			stream.next_out = reinterpret_cast<Bytef*>(compressed_chunk.data());
			stream.avail_out = compressed_chunk.size();

			int ret = deflate(&stream, infile.eof() ? Z_FINISH : Z_NO_FLUSH);

			if(ret == Z_STREAM_ERROR){
				cerr << "deflate failed";
				deflateEnd(&stream);
				exit(1);
			}
			buffer.insert(buffer.end(), compressed_chunk.data(), compressed_chunk.data() + compressed_chunk.size() - stream.avail_out);
		}while(stream.avail_out == 0);
	}

	deflateEnd(&stream);
	infile.close();

	ofstream outfile(output_file, ios::binary | ios::ate);
	if(!outfile.is_open()){
		cerr << "Error opening output file" << endl;
		exit(1);
	}

	if(!outfile.write(buffer.data(), buffer.size())){
		cerr << "Error writing to output file";
		exit(1);
	}

	outfile.close();
}

void decompress_file(const string& input_file, const string& output_file){
	ifstream infile(input_file, ios::binary | ios::ate);
	if(!infile.is_open()){
		cerr << "Error opening input file" << endl;
		exit(1);
	}

	streamsize compressed_size = infile.tellg();
	infile.seekg(0, ios::beg);

	vector<char> compressed_data(compressed_size);
	if(!infile.read(compressed_data.data(), compressed_size)){
		cerr << "Error reading input file" << endl;
		exit(1);
	}

	infile.close();

	vector<char> decompressed_data(compressed_size * 2);

	z_stream stream;
	memset(&stream, 0, sizeof(stream));

	if(inflateInit(&stream) != Z_OK){
		cerr << "inflateInit failed" << endl;
		exit(1);
	}

	stream.next_in = reinterpret_cast<Bytef*>(compressed_data.data());
	stream.avail_in = compressed_data.size();

	int result;

	do{
		stream.next_out = reinterpret_cast<Bytef*>(decompressed_data.data() + stream.total_out);
		stream.avail_out = decompressed_data.size() - stream.total_out;

		result = inflate(&stream, Z_FINISH);

		if(result == Z_STREAM_ERROR || result == Z_DATA_ERROR || result == Z_MEM_ERROR){
			cerr << "inflate failed with error code: " << result << endl;
			exit(1);
		}

		if(stream.avail_out == 0){
			decompressed_data.resize(decompressed_data.size() * 2);
		}
	}while(result != Z_STREAM_END);
	
	decompressed_data.resize(stream.total_out);
	inflateEnd(&stream);

	ofstream outfile(output_file, ios::binary);

	if(!outfile.is_open()){
		cerr << "Error opening output file" << endl;
		exit(1);
	}
	if(!outfile.write(decompressed_data.data(), decompressed_data.size())){
		cerr << "Error writing to output file " << endl;
		exit(1);
	}
	outfile.close();

}

int main(int argc, char* argv[]){
	/*if(argc != 4){
		cout << "Usage: " << argv[0] << "<compress|decompress> <input_file> <output_file>" << endl;
		return 1;
	}

	string input_file = argv[2];
	string output_file = argv[3];
	string mode = argv[1];

	if(mode == "compress"){
		auto start = high_resolution_clock::now();
		compress_file(input_file, output_file);
		auto stop = high_resolution_clock::now();

		auto delta = duration_cast<milliseconds> (stop - start);
		cout << "------------" << endl;
		cout << "Time taken to compress: " << delta.count() << " ms" << endl;
		cout << "------------" << endl;
	}
	else if(mode == "decompress"){
		decompress_file(input_file, output_file);
	}
	else{
		cerr << "Invalid mode of operation" << endl;
		exit(1);
	} */

	// Proceeding only with compressinon

	if(argc != 2){
		cout << "Usage: " << argv[0] << "<input_file>" << endl;
	}

	struct rusage usage_start, usage_end;
	struct timeval wall_start, wall_end;

	string input_file = argv[1];
	int found = input_file.find('.');
	string output_file = (input_file.substr(0, input_file.size() - found - 1)).append("_compressed").append(".zlib");

	auto start = high_resolution_clock::now();
	cpu_usage(usage_start);
	wall_time(wall_start);
	compress_file(input_file, output_file);
	auto stop = high_resolution_clock::now();
	cpu_usage(usage_end);
	wall_time(wall_end);

	print_cpu_usage(usage_start, usage_end, wall_start, wall_end);

	auto delta = duration_cast<milliseconds> (stop - start);
	cout << "------------" << endl;
	cout << "Time taken to compress: " << delta.count() << " milliseconds" << endl;
	cout << "------------" << endl << endl;

	// cout << "Compressed " << input_file << " as " << output_file << " in the current directory" << endl;

	return 0;
}
