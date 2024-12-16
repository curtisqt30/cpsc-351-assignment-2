#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <string>
#include <iomanip>

// Process structure
struct Process {
    int unique_id;
    int arrival_time;
    int lifetime;
    std::vector<int> address_space;
    int completion_time = -1; // for turnaround calculation
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
int current_time = 0; // Virtual clock

// Initialize memory blocks
void initialize_memory() {
    memory_map.clear();
    memory_map.push_back({0, memory_size - 1, -1, -1});
}

// Read input processes with validation
std::vector<Process> read_input(const std::string &file_path) {
    std::ifstream infile(file_path);
    if (!infile.is_open()) {
        std::cerr << "Error: Cannot open input file: " << file_path << std::endl;
        exit(1);
    }

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

// Print Input Queue for debugging
void print_input_queue(const std::queue<Process>& q) {
    std::cout << "\tInput Queue: [";
    std::queue<Process> temp = q;
    while (!temp.empty()) {
        std::cout << temp.front().unique_id;
        temp.pop();
        if (!temp.empty()) std::cout << " ";
    }
    std::cout << "]\n";
}

// Print the memory map for debugging
void print_memory_map() {
    std::cout << "\tMemory Map:\n";
    for (const auto &block : memory_map) {
        if (block.process_id == -1) {
            std::cout << "\t\t" << block.start << "-" << block.end << ": Free frame(s)\n";
        } else {
            std::cout << "\t\t" << block.start << "-" << block.end << ": Process " 
                      << block.process_id << ", Page " << block.page_number << "\n";
        }
    }
}

// Release memory of completed processes safely
void release_memory(int process_id) {
    for (auto &block : memory_map) {
        if (block.process_id == process_id) {
            block.process_id = -1;
            block.page_number = -1;
        }
    }
}

// Allocate memory for a process safely
bool allocate_memory(const Process &process) {
    int page_number = 1;
    std::vector<MemoryBlock> new_blocks;

    for (int size : process.address_space) {
        int pages_required = (size + page_size - 1) / page_size;
        int block_size = pages_required * page_size;
        bool allocated = false;

        for (auto &block : memory_map) {
            if (block.process_id == -1 && (block.end - block.start + 1) >= block_size) {
                int start = block.start;
                int end = start + block_size - 1;

                // Ensure bounds are valid
                if (end > block.end) continue;

                new_blocks.push_back({start, end, process.unique_id, page_number++});
                block.start = end + 1;

                if (block.start > block.end) {
                    block.start = block.end + 1; // Avoid invalid range
                }

                allocated = true;
                break;
            }
        }

        if (!allocated) return false; // Memory allocation failed for this process
    }

    // Add new blocks after iteration to avoid invalidating iterators
    memory_map.insert(memory_map.end(), new_blocks.begin(), new_blocks.end());
    return true;
}

// Simulate memory allocation and process management
void simulate(std::vector<Process> &processes) {
    std::queue<Process> input_queue;

    while (current_time <= 100000) {
        // Check for new arrivals
        for (auto &process : processes) {
            if (process.arrival_time == current_time) {
                std::cout << "t = " << current_time << ": Process " 
                          << process.unique_id << " arrives\n";
                input_queue.push(process);
                print_input_queue(input_queue);
            }
        }

        // Release memory for completed processes and print memory map
        for (auto &process : processes) {
            if (process.completion_time == current_time) {
                std::cout << "t = " << current_time << ": Process " 
                          << process.unique_id << " completes\n";
                release_memory(process.unique_id);
                print_memory_map();
            }
        }

        // Try to allocate memory for processes in the input queue safely
        std::queue<Process> temp_queue;
        while (!input_queue.empty()) {
            Process current = input_queue.front();
            input_queue.pop();

            if (allocate_memory(current)) {
                // Update completion time **in the processes vector**
                for (auto &p : processes) {
                    if (p.unique_id == current.unique_id) {
                        p.completion_time = current_time + current.lifetime;
                        break;
                    }
                }
                std::cout << "\tMM moves Process " << current.unique_id << " to memory\n";
                print_input_queue(input_queue);
                print_memory_map();
            } else {
                temp_queue.push(current); // Keep the process in the queue
            }
        }
        input_queue = temp_queue;

        // Check if all processes are done
        bool all_completed = true;
        for (const auto &process : processes) {
            if (process.completion_time == -1) {
                all_completed = false;
                break;
            }
        }
        if (all_completed && input_queue.empty()) break;

        ++current_time; // Increment the virtual clock to process next event
    }

    // Calculate and print average turnaround time for all processes
    double total_turnaround_time = 0;
    for (const auto &process : processes) {
        total_turnaround_time += process.completion_time - process.arrival_time;
    }
    std::cout << "\nAverage Turnaround Time: " 
              << total_turnaround_time / processes.size() << "\n";
}

int main() {
    std::cout << "Enter memory size: ";
    std::cin >> memory_size;
    std::cout << "Enter page size: ";
    std::cin >> page_size;

    initialize_memory();

    std::vector<Process> processes = read_input("in1.txt");

    simulate(processes);

    return 0;
}