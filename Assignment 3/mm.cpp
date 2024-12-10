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
	std::vector<int> address_space;
};

// Blocks hold either a process or free spcae
struct MemoryBlock {
	int start;
	int end;
	int process_id;
};

std::vector<MemoryBlock> memory_map;

// Function to read the input file.
std::vector<Process> read_input(const std::string& file_path) {
	std::ifstream infile(file_path);
	std::vector<Process> processes;
	
	// First line in input file tells us how many processes to read: 8.
	int num_processes;
	infile >> num_processes;
	
	for (int i = 0; i < num_processes; ++i) {
		Process p;
		infile >> p.unique_id >> p.arrival_time >> p.lifetime;
		int num_pages;
		infile >> num_pages;
		p.address_space.resize(num_pages);
		for (int j = 0; j < num_pages; ++j) {
			infile >> p.address_space[j];
		}
		processes.push_back(p);
	}
	
	infile.close();
	return processes;
}

// Function to simulate memory mapping
void simulate_memory_map(std::vector<Process>& processes) {}


// Function to print the memory map 
void print_memory_map() {
	std::cout << "Memory map:\n";
	for (const auto& block : memory_map) {
		if (block.process_id == -1) {
			std::cout << "\t" << block.start << "-" << block.end << ": Free Frames(s)\n";
		} else {
			std::cout << "\t" << block.start << "-" << block.end << ": Process " << block.process_id << "\n";
		}
	}
}

int main() {
	std::vector<Process> processes = read_input("in1.txt");
	simulate_memory_map(processes);
	return 0;
}
