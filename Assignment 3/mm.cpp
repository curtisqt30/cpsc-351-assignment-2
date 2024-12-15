#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <string>

// Process structure
struct Process {
    int unique_id;
    int arrival_time;
    int lifetime;
    std::vector <int> address_space;
};

// Memory block structure
struct MemoryBlock {
    int start;
    int end;
    int process_id;  
    int page_number; 
};

// Globals variables
std::vector<MemoryBlock> memory_map;
int memory_size;
int page_size;

// Initialize memory
void initialize_memory() {
    memory_map.clear();
    memory_map.push_back({0, memory_size - 1, -1, -1});
}

// Read input processes
std::vector<Process> read_input(const std::string &file_path) {
    std::ifstream infile(file_path);
    std::vector<Process> processes;

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

// Print the memory map
void print_memory_map() {
    std::cout << "\tMemory map:\n";
    for (const auto &block : memory_map) {
        if (block.process_id == -1) {
            std::cout << "\t\t" << block.start << "-" << block.end << ": Free frame(s)\n";
        } else {
            std::cout << "\t\t" << block.start << "-" << block.end << ": Process " << block.process_id
                      << ", Page " << block.page_number << "\n";
        }
    }
}

// Allocate memory for a process
bool allocate_memory(const Process &process) {
    int page_number = 1;
    bool allocated = false;

    for (int size : process.address_space) {
        int pages_required = (size + page_size - 1) / page_size;
        int block_size = pages_required * page_size;

        // Find a suitable block
        for (auto &block : memory_map) {
            if (block.process_id == -1 && (block.end - block.start + 1) >= block_size) {
                // Allocate pages in this block
                int start = block.start;
                int end = start + block_size - 1;

                // Create memory blocks for the process pages
                memory_map.push_back({start, end, process.unique_id, page_number++});
                block.start = end + 1;
                if (block.start > block.end) {
                    block.process_id = process.unique_id;
                    block.page_number = page_number - 1;
                }

                allocated = true;
                break;
            }
        }

        if (!allocated) {
            return false;
        }
    }

    return true;
}

// Simulate memory allocation
void simulate(std::vector<Process> &processes) {
    std::queue<Process> input_queue;

    // Process each arriving process
    for (const auto &process : processes) {
        std::cout << "t = " << process.arrival_time << ":Process" << process.unique_id << "arrives\n";
        input_queue.push(process);

        std::cout << "\tInput Queue:[";
        auto temp_queue = input_queue;
        while (!temp_queue.empty()) {
            std::cout << temp_queue.front().unique_id << " ";
            temp_queue.pop();
        }
        std::cout << "]\n";

        // Try to allocate processes from the input queue
        while (!input_queue.empty()) {
            const Process &current = input_queue.front();

            if (allocate_memory(current)) {
                std::cout << "\tMM moves Process" << current.unique_id << "to memory\n";
                input_queue.pop();
                print_memory_map();
            } else {
                break;
            }
        }
    }
}

int main() {
    std::cout << "Enter memory size:";
    std::cin >> memory_size;
    std::cout << "Enter page size:";
    std::cin >> page_size;

    initialize_memory();

    std::vector<Process> processes = read_input("in1.txt");

    simulate(processes);

    return 0;
}
