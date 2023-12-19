#pragma once

#include <vector>
#include <thread>
#include <iostream>

class ThreadPool {
    private:
        std::vector<std::thread> threads_;
        int num_threads_;
        const static int MAX_THREADS = 20;

    public:
        ThreadPool();

        void init(int);

        template <typename Callable>
        void createWorkerThreads(Callable &&);

        void joinWorkerThreads();
};


/*
* Source implementation
*/
ThreadPool::ThreadPool() {
    num_threads_ = MAX_THREADS;
}

void ThreadPool::init(int num_threads) {
    num_threads_ = num_threads > MAX_THREADS? MAX_THREADS: num_threads;
}

template <typename Callable>
void ThreadPool::createWorkerThreads(Callable && function) {
    std::cout << std::endl << " ---------- Creating worker threads for the server ----------- " << std::endl;
    for (int i=0; i<num_threads_; i++) {
        threads_.emplace_back(std::thread(std::forward<Callable>(function)));
    }

    std::cout << " ---------- Created worker threads for the server ----------- " << std::endl;
}

void ThreadPool::joinWorkerThreads() {
    for (int i=0; i<threads_.size(); i++) {
        threads_.at(i).join();
    }
    std::cout << "All threads completed" << std::endl;
}