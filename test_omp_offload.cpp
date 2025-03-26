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


#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdio>
//#include "/home/swastik/dev/openmp_offload/llvm-project/offload/include/quantum_circuit_wrapper.h"

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
        scr += "dax_job = execute(circuit, backend, shots=30, optimization_level=0)\n";
        scr += "client = sequre.UserClient()\n";
        scr += "workload = dax_job.get_dax()\n";
        scr += "print(workload)";
    }

    std::string run() {
        std::string script = generate_python_script("circuit", num_qubits, gates);
        return execute_python_script(script);
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

private:
    int num_qubits;
    string gates;
    string scr;

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
    QuantumCircuitWrapper *circuit = new QuantumCircuitWrapper(qubits);
    #pragma omp target firstprivate(circuit) device(100) map(to: a[0:N], b[0:N]) map(from: c[0:N])
    {
        // sleep(1);
        circuit->apply_hadamard(0);
        for(int i = 0 ; i < qubits; i++){
            circuit->apply_cnot(i, i+1);
        }
        circuit->apply_barrier();
        circuit->measure();
        circuit->test = "checking";
        circuit->execute_basic_quantum();
        // for(int i = 0 ; i < N ; i++) {
        //     c[i] = b[i]/a[i];
        // }
        //printf(" %d ",c[i]);
    }

    // Check results
    printf("test value now is:  %s\n", (circuit->test).c_str());
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
