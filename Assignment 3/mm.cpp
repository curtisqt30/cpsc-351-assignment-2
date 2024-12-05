#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

// Each process has a unique id, arrival time, life time in memory, and address spaces
struct Process {
	int unique_id;
	int arrival_time;
	int lifetime;
	int address_space;
	std::vector<int> address_space;
};

// Function to read the input file.
std::vector<Process> read_input(const std::string& file_path) {
	std::ifstream infile(file_path);
	std::vector<Process> processes;
	
	// First line in input file tells us how many processes to read: 8.
	int num_processes;
	infile >> num_processes;
	
	for (int i = 0; i < num_processes; ++i) {
		
	}
	
	infile.close();
	return processes;
}

int main() {
	return 0;
}
