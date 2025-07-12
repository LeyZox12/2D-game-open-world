#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <thread>
#include <atomic>
#include <vector>

using namespace std;

class ThreadPool
{
    public:

        ThreadPool(int maxThreads);
        ThreadPool();
        void setMaxThreads(int maxThreads);
        void addToQueue(thread t);
        void update();

    protected:

    private:

        struct Worker
        {
            atomic<bool> running{false};
            Worker(thread t)
            {
                this -> t = thread(move(t));
            }
            void start()
            {
                running.store(true);
            }
            thread t;
        };
        int maxThreads = 10;
        vector<Worker> pool;
        vector<thread> threadQueue;

};

#endif // THREADPOOL_H
