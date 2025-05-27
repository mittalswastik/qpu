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
#include <json/json.h>
// //#include <llvm-project/offload/include/quantum_circuit_wrapper.h>
// #include "quantum_circuit_wrapper.h"

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
    std::vector<std::vector<double> > vec_data;
    std::vector<void*> vec_out_data;
    std::vector<int> evaluated_qubits;
    FILE* pipe_cpp;
    FILE* pipe_py;
    int num_iterations;
    pid_t pid[2];
    
    QuantumCircuitWrapper(int num_qubits) : num_qubits(num_qubits) {
        test = "check";
        inFD  = -1;   // parent writes   -> child stdin
        outFD = -1;   // parent reads    <- child stdout
        pid = -1;
    }

    void debug(){
        std::cout<<scr<<std::endl;
    }

    void write(double my_angles[num_qubits]){

    }

    void apply_hadamard(int qubit) {
        gates += "circuit.h(" + std::to_string(qubit) + ")\n";
    }
    
    void apply_cnot(int control, int target) {
        gates += "circuit.cx(" + std::to_string(control) + ", " + std::to_string(target) + ")\n";
    }
    
    void apply_x(int qubit) {
        gates += "circuit.x(" + std::to_string(qubit) + ")\n";
    }
    
    void apply_ry(double angle, int val) {
        gates += "apply_ry(" + std::to_string(angle) + ", "+  std::to_string(val) + ")\n";
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
        scr += "backend_name = 'dax_code_simulator'\n";
        scr += "backend_name = 'dax_code_printer'\n";
        scr += "backend = dax.get_backend(backend_name)\n";
        scr += "backend.load_config(\"resources.toml\")\n";
        scr += "for i in input_itr:\n";
        scr += "    response_data = json.loads(sys.stdin.readline().strip())\n"; // put execute in a loop
        scr += "    dax_job = execute(circuit, backend, shots=30, optimization_level=0)\n";
        scr += "    client = sequre.UserClient()\n";
        scr += "    workload = dax_job.get_dax()\n";
        scr += "    print(workload)\n";
    }
    
    std::vector<int32_t> parseToVector(void* ptr, size_t size, std::vector<int32_t> vec){
        std::cout<<" size of the the vector is: "<<size<<std::endl;
        intptr_t intPtr = reinterpret_cast<intptr_t> (ptr); // Cast void* to int*
        int32_t *intVal = reinterpret_cast<int32_t*>(intPtr);
        size = size/sizeof(int32_t);
        std::cout<<"array value is"<<std::endl;
        for(int i = 0 ; i < size ; i++){
            std::cout<<intVal[i]<<" "<<std::endl;
        }
    
        std::cout<<"end of array value"<<std::endl;
        vec.assign(intVal, intVal + size);   // Populate vector using a range
        return vec;
    }

    std::string run() {
        std::string script = generate_python_script("circuit", num_qubits, gates);
        return execute_python_script(script);
    }

private:
    int num_qubits;
    std::string gates;
    std::string scr;

    std::string generate_python_script(const std::string& circuit_name, int num_qubits, const std::string& gates) {
        std::ostringstream script;
        script << "import sys\n";
        script << "import json\n";
        script << "import supermarq\n";
        script << "import qiskit\n";
        script << "import matplotlib.pyplot as plt\n";
        script << "import numpy as np\n";
        script << "from qiskit import QuantumCircuit, execute\n";
        script << "from qiskit.providers.dax import DAX\n";
        script << "import sequre\n";
        //processong function
        script << "def process_data(data):\n";
        script << "    # Example processing: square each number\n";
        script << "    return [[x * x for x in row] for row in data]\n\n";
    
        // Read JSON data from command line argument
        script << "if __name__ == \"__main__\":\n";
        script << "    if len(sys.argv) < 2:\n";
        script << "        print('Error: No input data provided')\n";
        script << "        sys.exit(1)\n\n";
        script << "    input_itr = json.loads(sys.argv[2])\n";
        script << "    circuit = QuantumCircuit(" << num_qubits << "," <<num_qubits << ")\n";
        //script << "    processed_data = process_data(input_data)\n";
        std::string line;
        std::istringstream ss(gates);
        while(std::getline(ss, line)) {
            script << "    " << line << "\n"; // Adds indentation to each line
        }
    
        std::string line2;
        std::istringstream ss2(scr);
        while(std::getline(ss2, line2)) {
            std::cout<<line2<<std::endl;  
            script << "    " << line2 << "\n"; // Adds indentation to each line
        }
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

    std::vector<int> readQubits(const std::string& result) {
        Json::Value root;
        Json::CharReaderBuilder reader;
        std::string errs;
        std::istringstream s(result);
        if (!Json::parseFromStream(reader, s, &root, &errs)) {
            std::cerr << "Failed to parse JSON: " << errs << std::endl;
            return {};
        }
    
        // Determine the number of possible states (bitstrings)
        size_t num_states = root.size();
        std::vector<int> qubit_results(num_states, 0);
    
        // Parse JSON into vector<int> where index represents the bitstring
        for (Json::Value::const_iterator it = root.begin(); it != root.end(); ++it) {
            std::string bitstring = it.key().asString();
            int decimal_index = std::stoi(bitstring, nullptr, 2); // Convert "000", "001" -> 0, 1, 2...
            qubit_results[decimal_index] = it->asInt(); // Store the count at the correct index
        }
    
        return qubit_results;
    }

    std::string returnJsonString(){
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

        return json_data;
    }
    
    
    std::string execute_python_script(const std::string& script) {
        std::string json_data = returnJsonString();
        
        // Write the script to a temporary file
        std::ofstream file("temp_script.py");
        file << script;
        file.close();
    
        // Run the script and capture the output
        std::string command = "python3 temp_script.py "+json_data+" "+(char) num_iterations;
        char buffer[4000];
        std::string result;
        pipe = popen(command.c_str(), "w+");
        //if (!pipe) throw std::runtime_error("popen() failed!");
        
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
    
        evaluated_qubits = readQubits(result);
    
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

std::vector<double> evaluate_new_angles_from_freq_array(std::vector<int> frequencies) {
    int num_entries = frequencies.size();
    if (num_entries == 0) return {};

    int num_qubits = 0;
    while ((1 << num_qubits) < num_entries) ++num_qubits;

    std::vector<int> one_counts(num_qubits, 0);
    int total_counts = 0;

    for (int i = 0; i < num_entries; ++i) {
        int freq = frequencies[i];
        total_counts += freq;

        for (int q = 0; q < num_qubits; ++q) {
            if ((i >> (num_qubits - q - 1)) & 1) {
                one_counts[q] += freq;
            }
        }
    }

    std::vector<double> angles(num_qubits, 0.0);
    if (total_counts == 0) return angles;

    for (int q = 0; q < num_qubits; ++q) {
        double freq_q = static_cast<double>(one_counts[q]) / total_counts;
        angles[q] = freq_q * M_PI;
    }

    return angles;
}

void* thread_func(void* arg) {
    QuantumCircuitWrapper* obj = static_cast<QuantumCircuitWrapper*>(arg);
    obj->run();  // Call a member function
    return nullptr;
}

// Function to execute a command and get the output
int main() {
    // Check for the number of devices available
    // int num_devices = omp_get_num_devices();
    // printf("Number of available devices: %d\n", num_devices);
    
    // // If no device is available, exit
    // if (num_devices < 1) {
    //     printf("No devices available for offloading. Exiting.\n");
    //     return 1;
    // }

    // Arrays for computation
    int a[N], b[N], c[N];

    // Initialize arrays
    for (int i = 0; i < N; i++) {
        a[i] = i+1;
        b[i] = 2 * a[i];
        c[i] = 0;
    }

    // Offload computation to GPU (assuming device 0)
    int qubits = 4;
    vector<int> qubits_freq(qubits);
    for(int j = 0 ; j < qubits ; j++){
        qubits_freq[j] = 0;
    }

    std::vector<double> angles = evaluate_new_angles_from_freq_array(qubits_freq);
    omp_set_num_threads(2);
    //QuantumCircuitWrapper *circuit = QuantumCircuitWrapper_create(qubits);
    std::vector< std::vector<double> > angle_threads(max_threads, angles);
    #pragma omp parallel shared(angle_threads)
    {
        cout<<"angle threads size is: "<<angle_threads.size()<<endl;
        //for(int k = 0 ; k < 5 ; k++){
            cout<<"testing"<<endl;
            QuantumCircuitWrapper *circuit = new QuantumCircuitWrapper(qubits);
            double my_angles[qubits];
            for(int i = 0 ; i < qubits ; i++){
                my_angles[i] = angle_threads[omp_get_thread_num()][i];
            }

            for(int i = 0 ; i < qubits; i++){
                //circuit->apply_cnot(i, i+1);
                circuit->apply_ry(my_angles[i], i);
                //QuantumCircuitWrapper_apply_cnot(circuit,i,i+1);
            }

            for(int i = 0 ; i < qubits-1; i++){
                circuit->apply_cnot(i, i+1);
                //QuantumCircuitWrapper_apply_cnot(circuit,i,i+1);
            }

            for(int i = 0 ; i < qubits-1; i++){
                circuit->apply_ry(my_angles[i+1], i+1);
                //QuantumCircuitWrapper_apply_cnot(circuit,i,i+1);
            }
            //circuit->apply_barrier();
            //QuantumCircuitWrapper_apply_barrier(circuit); 
            circuit->measure();
            circuit->execute_basic_quantum();

            #pragma omp target firstprivate(circuit) iteration(5) device(100) map(to: my_angles[0:qubits], b[0:N]) map(from: c[0:N])
            {
                for(int i = 0 ; i < 5 ; i++){
                    circuit->write(my_angles);
                    cout<<"evaluated qubits freq is: "<<endl;
                    for(int z = 0 ; z < circuit->evaluated_qubits.size() ; z++){
                        cout<<circuit->evaluated_qubits[z]<<" ";
                    }

                    cout<<endl;
                    cout<<"angle threads size changed to: " <<angle_threads.size()<<endl;
                    std::vector<double> temp_angles = evaluate_new_angles_from_freq_array(circuit->evaluated_qubits);
                    for(int i = 0 ; i < qubits ; i++){
                        my_angles[i] = temp_angles[i];
                    }
                }
            }

            delete circuit;
            // eventually make it memory efficient - call a dstructor here and push object creation outside loop - no need to call construnctor n number of times
        //}
    }

    // Check results
    // printf("test value now is:  %s\n", (circuit->test).c_str());
    int errors = 0;

    printf("\n");

    if (errors == 0) {
        printf("Success! All values are correct. Printing c value below\n");
        for(int i = 0 ; i < N ; i++){
            printf("%d",c[i]);
        }
    } else {
        printf("Failed with %d errors.\n", errors);
    }

    return 0;
}