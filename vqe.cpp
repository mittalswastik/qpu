#include <iostream>
#include <vector>
#include <omp.h>
#include <cstdlib>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <bits/stdc++.h>
#include <unistd.h>

#define N 10
#define max_threads 5


#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdio>
#include <chrono>

using namespace std;

void func(){
    cout<<"func"<<endl;
    //printf("func");
    //cout gives a linker error
    // calling the constructor outside also works - but gives a warning and then misalignment error
    // need to find a way to by pass compiler check as I am not running on a device
}

class QuantumCircuitWrapper {
public:


    std::string test;
    std::vector<std::vector<int32_t> > vec_data;
    std::vector<void*> vec_out_data;
    std::vector<int> evaluated_qubits;
    
    QuantumCircuitWrapper(int num_qubits) : num_qubits(num_qubits) {
        test = "check";
    }

    void apply_hadamard(int qubit) {
        gates += "circuit.h(" + to_string(qubit) + ")\n";
    }

    void apply_cnot(int control, int target) {
        gates += "circuit.cx(" + to_string(control) + ", " + std::to_string(target) + ")\n";
    }

    void apply_x(int qubit) {
        gates += "circuit.x(" + to_string(qubit) + ")\n";
    }

    void apply_ry(double angle, int val) {
        gates += "circuit.ry(" + std::to_string(angle) + ", "+  std::to_string(val) + ")\n";
    }

    void apply_barrier(){
        gates += "circuit.barrier()\n";
    }

    void measure(){
        gates += "circuit.measure_all()\n";
    }

    void apply_hamiltonian_qiskit(){
        gates += "smarq = supermarq.hamiltonian_simulation.HamiltonianSimulation(" + std::to_string(num_qubits) + ")\n";
        gates += "circuit = smarq.qisket_circuit()\n";
        gates += "print(circuit)\n";
    }

    void apply_ghz_qiskit(){
        gates += "smarq = supermarq.ghz.GHZ(" + std::to_string(num_qubits) + ")\n";
        gates += "circuit = smarq.qisket_circuit()\n";
        gates += "print(circuit)\n";
    }

    void execute_basic_quantum(){
        // scr += "backend_name = 'dax_code_simulator'\n";
        // scr += "backend_name = 'dax_code_printer'\n";
        // scr += "backend = DAX.get_backend(backend_name)\n";
        // scr += "backend.load_config(\"resources.toml\")\n";
        // scr += "dax_job = execute(circuit, backend, shots=100, optimization_level=0)\n";
        // scr += "client = sequre.UserClient()\n";
        // scr += "workload = dax_job.get_dax()\n";
        // scr += "print(workload)";

        scr += "backend = qiskit.Aer.get_backend('statevector_simulator')\n"; 
        scr += "counts = execute(circuit, backend, shots=100).result().get_counts()\n";
        scr += "counts = json.dumps(counts)\n";
        // scr += "work_id = client.create_work(workload=workload,device_id=backend_name, priority=\"LOW\")\n";
        // scr += "client.wait_until_done(work_id, 0.1)\n";
        // scr += "results = client.get_results(work_id)\n";
        // scr += "results = dax_job.get_result_obj(results)\n";
        // scr += "counts = results.get_counts()\n";
        // scr += "counts = json.dumps(counts)\n"; 
        scr += "print(counts)";
    }

    std::string run() {
        std::string script = generate_python_script("circuit", num_qubits, gates);
        return execute_python_script(script);
    }

    std::vector<int32_t> parseToVector(void* ptr, size_t size, std::vector<int32_t> vec){
        // std::cout<<" size of the the vector is: "<<size<<std::endl;
        intptr_t intPtr = reinterpret_cast<intptr_t> (ptr); // Cast void* to int*
        int32_t *intVal = reinterpret_cast<int32_t*>(intPtr);
        size = size/sizeof(int32_t);
        // std::cout<<"array value is"<<std::endl;
        // for(int i = 0 ; i < size ; i++){
        //     std::cout<<intVal[i]<<" "<<std::endl;
        // }

        // std::cout<<"end of array value"<<std::endl;
        vec.assign(intVal, intVal + size);   // Populate vector using a range
        return vec;
    }

private:
    int num_qubits;
    string gates;
    string scr;

    std::string generate_python_script(const std::string& circuit_name, int num_qubits, const std::string& gates) {
        std::ostringstream script;
        script << "import sys\n";
        script << "import json\n";
        //script << "import supermarq\n";
        script << "import qiskit\n";
        script << "from qiskit import QuantumCircuit, execute\n";
        // script << "from qiskit.providers.dax import DAX\n";
        // script << "import sequre\n";
        //processong function
        script << "def process_data(data):\n";
        script << "    # Example processing: square each number\n";
        script << "    return [[x * x for x in row] for row in data]\n\n";

        // Read JSON data from command line argument
        script << "if __name__ == \"__main__\":\n";
        script << "    if len(sys.argv) < 2:\n";
        script << "        print('Error: No input data provided')\n";
        script << "        sys.exit(1)\n\n";
        script << "    input_data = json.loads(sys.argv[1])\n";
        script << "    circuit = QuantumCircuit(" << num_qubits << "," <<num_qubits << ")\n";
        //script << "    processed_data = process_data(input_data)\n";
        std::string line;
        std::istringstream ss(gates);
        while(std::getline(ss, line)) {
            script << "    " << line << "\n"; // Adds indentation to each line
        }

        std::string line2;
        std::istringstream ss2(scr);
        // while(std::getline(ss2, line2)) {
        //     std::cout<<line2<<std::endl;    
        //     script << "    " << line2 << "\n"; // Adds indentation to each line
        // }
        //script << gates;
        // script << "    backend_name = 'dax_code_simulator'\n";
        // script << "    backend_name = 'dax_code_printer'\n";
        // script << "    backend = dax.get_backend(backend_name)\n";
        // script << "    backend.load_config("<<"\"resources.toml\""<<")\n";
        // script << "    dax_job = execute(circuit, backend, shots=30, optimization_level=0)\n";
        // script << "    client = sequre.UserClient()\n";
        // script << "    workload = dax_job.get_dax()\n";
        // script << "    print(workload)";
        return script.str();
    }

    // std::vector<std::vector<int> > QuantumCircuitWrapper::readQubits(std::string result){
    //     Json::Value root;
    //     Json::CharReaderBuilder reader;
    //     std::string errs;
    //     std::istringstream s(result);
    //     if (!Json::parseFromStream(reader, s, &root, &errs)) {
    //         std::cerr << "Failed to parse JSON: " << errs << std::endl;
    //         return {};
    //     }

    //     // Convert JSON to std::vector<std::vector<int>>
    //     std::vector<std::vector<int>> qubit_results;
    //     for (const auto& row : root) {
    //         std::vector<int> qubits;
    //         for (const auto& val : row) {
    //             qubits.push_back(val.asInt());
    //         }
    //         qubit_results.push_back(qubits);
    //     }

    //     return qubit_results;
    // }

    std::string execute_python_script(const std::string& script) {
        std::string json_data = "[";
        for (int32_t i = 0; i < vec_data.size(); ++i) {
            json_data += "[";
            for(int32_t j = 0; j < vec_data[i].size(); ++j){
            json_data += std::to_string(vec_data[i][j]);
            if (j < vec_data[i].size() - 1) {
                json_data += ",";
            }
            }

            json_data += "]";
        }

        json_data += "]";
        
        // Write the script to a temporary file
        std::ofstream file("temp_script.py");
        file << script;
        file.close();

        // Run the script and capture the output
        std::string command = "python3 temp_script.py "+json_data;
        char buffer[4000];
        std::string result;
        FILE* pipe = popen(command.c_str(), "r");
        //if (!pipe) throw std::runtime_error("popen() failed!");
        
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
        pclose(pipe);

        return result;
    }
};

// std::vector<double> evaluate_new_angles(const std::vector<std::vector<int>>& results) {
//     std::vector<double> new_angles;
//     new_angles.reserve(results.size());

//     for (const auto& bitstring : results) {
//         int sum_bits = std::accumulate(bitstring.begin(), bitstring.end(), 0);
//         size_t num_qubits = bitstring.size();
//         double angle = (static_cast<double>(sum_bits) * M_PI) / num_qubits;
//         new_angles.push_back(angle);
//     }

//     return new_angles;
// }


int main() {
    // --- 1. File Reading and Data Loading into Map ---
    std::string filename = "all_runs_cost_history.txt";
    std::map<int, std::vector<std::vector<double>>> data_by_run;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file '" << filename << "'" << std::endl;
        return 1;
    }

    std::string line;
    // Skip header
    if (!std::getline(file, line)) {
        std::cerr << "Error: File empty or cannot read header." << std::endl;
        file.close(); return 1;
    }
    std::cout << "Skipped header: " << line << std::endl;

    int line_number = 1;
    while (std::getline(file, line)) {
        line_number++;
        if (line.empty() || line.find_first_not_of(' ') == std::string::npos) continue;

        std::vector<double> current_row_values;
        std::stringstream ss(line);
        std::string token;
        while (std::getline(ss, token, ',')) {
            try {
                current_row_values.push_back(std::stod(token));
            } catch (...) { // Catch potential conversion errors broadly
                 std::cerr << "Warning [Line " << line_number << "]: Problem converting token '" << token << "'. Skipping value." << std::endl;
            }
        }

        if (!current_row_values.empty()) {
            try {
                double run_id_double = current_row_values[0];
                 if (run_id_double < std::numeric_limits<int>::min() || run_id_double > std::numeric_limits<int>::max() || run_id_double != static_cast<int>(run_id_double)) {
                     throw std::out_of_range("Run ID value not representable as integer");
                 }
                int run_id = static_cast<int>(run_id_double);
                data_by_run[run_id].push_back(current_row_values);
            } catch (const std::exception& e) {
                 std::cerr << "Warning [Line " << line_number << "]: Invalid Run ID '" << current_row_values[0] << "' (" << e.what() << "). Skipping row." << std::endl;
            }
        }
    }
    file.close();
    std::cout << "\nFinished reading data. Found " << data_by_run.size() << " unique runs." << std::endl;

    // --- 2. Prepare for OpenMP: Extract Run IDs ---
    // Create a vector containing the keys (run IDs) from the map.
    // This vector will be used to distribute work in the parallel loop.
    std::vector<int> run_ids_vector;
    run_ids_vector.reserve(data_by_run.size()); // Reserve space for efficiency
    for (const auto& pair : data_by_run) {
        run_ids_vector.push_back(pair.first);
    }
    // Optional: Sort the run IDs if processing order matters, although OpenMP
    // scheduling might still execute iterations out of order unless specified.
    // std::sort(run_ids_vector.begin(), run_ids_vector.end());


    // --- 3. OpenMP Parallel Processing ---
    // std::cout << "\nStarting parallel processing using OpenMP..." << std::endl;

    // Use omp parallel for to distribute loop iterations across threads
    // Each iteration 'i' corresponds to processing one run_id from run_ids_vector.

    int qubits = 7;
    omp_set_num_threads(6);


    auto start = chrono::steady_clock::now();
    #pragma omp parallel shared(data_by_run)
    {
        // cout<<"angle threads size is: "<<.size()<<endl;
        // for(int k = 0 ; k < 5 ; k++){
            // cout<<"testing"<<endl;
            double my_angles[2*qubits];

            for (int iteration = 0; iteration < 80; iteration++){
                QuantumCircuitWrapper *circuit = new QuantumCircuitWrapper(qubits);

                for(int i = 0 ; i < 2*qubits ; i++){
                    my_angles[i] = data_by_run[omp_get_thread_num()][iteration][i];
                }

                #pragma omp target firstprivate(circuit) device(100) map(to: my_angles[0:qubits])
                {

                    for(int i = 0 ; i < qubits; i++){
                        circuit->apply_ry(my_angles[i], i);
                    }

                    for(int i = 0 ; i < qubits-1; i++){
                        circuit->apply_cnot(i, i+1);
                    }

                    for(int i = 0 ; i < qubits; i++){
                        circuit->apply_ry(my_angles[qubits+i], i);
                    }
                    circuit->measure();
                    circuit->execute_basic_quantum();
                }

                // cout<<"evaluated qubits freq is: "<<endl;
                // for(int z = 0 ; z < circuit->evaluated_qubits.size() ; z++){
                //     cout<<circuit->evaluated_qubits[z]<<" ";
                // }

            // cout<<endl;
            // cout<<"angle threads size changed to: " <<angle_threads.size()<<endl;
            // data_by_run[omp_get_thread_num()] = evaluate_new_angles_from_freq_array(circuit->evaluated_qubits);
            delete circuit;
            }
            // eventually make it memory efficient - call a dstructor here and push object creation outside loop - no need to call construnctor n number of times
    }
    auto end = chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    cout << "\nParallel processing finished in " << diff.count() << "ms" << endl;

    return 0; // Indicate successful execution
}
