#include <iostream>
#include <fstream>
#include <thread>
#include <mutex>
#include <vector>

using namespace std;

// Function to check if a number is prime
bool is_prime(int n){
    if (n <= 1) {
        return false;
    }
    for (int i = 2; i <= n/2; i++){
        if (n % i == 0) {
            return false;
        }
    }
    return true;
}

// Mutex to ensure thread safety when writing to the file
mutex file_mutex;

// A function to be run by each thread
void find_primes(int start, int end, int& count){
    count = 0;
    for (int i = start; i <= end; i++){
        if (is_prime(i)){
            // Write the prime number to the file
            lock_guard<mutex> lock(file_mutex);
            ofstream file("primes.dat", ios::app);
            if (file.is_open()){
                file << i << "\n";
                file.close();
            }
            count++;
        }
    }
}

int main(){
    const int num_threads = 4;
    vector<thread> threads(num_threads);
    int primes_per_thread[num_threads] = {0};

    // Divide the range of numbers to be checked among the threads
    int start = 2;
    int end = 1000000;
    int range_per_thread = (end - start + 1) / num_threads;
    for (int i = 0; i < num_threads; i++) {
        int thread_start = start + i*range_per_thread;
        int thread_end = (i == num_threads - 1) ? end : thread_start + range_per_thread - 1;
        threads[i] = thread(find_primes, thread_start, thread_end, ref(primes_per_thread[i]));
    }

    // Wait for all threads to finish
    for (auto& thread : threads) {
        thread.join();
    }

    // Print the total number of primes found
    int total = 0;
    for (int i = 0; i < num_threads; i++){
        total += primes_per_thread[i];
    }
    cout << "Total number of primes found: " << total<< "\n";

    // Print the number of primes found by each thread
    for (int i = 0; i < num_threads; i++){
        cout << "Thread " << i+1 << " found " << primes_per_thread[i] << " primes.\n";
    }
    return 0;
}
