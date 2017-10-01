#pragma once

#include <vector>
#include <thread>

#include "notification_queue.hpp"

class task_system{

    public:
        task_system()
        {
            for(unsigned n = 0; n != m_count; ++n)
            {
                m_threads.emplace_back([&]{ run(); });
            }
        }

        ~task_system()
        {
            m_q.done();
            for(auto & t: m_threads)
            {
                t.join();
            }
        }

        template<typename F>
        void async(F&& f)
        {
            m_q.push(std::forward<F> (f));
        }

    private:
        void run()
        {
            while(true)
            {
                std::function<void()> f;
                // retrieve the function to be run
                // if false means the queue is empty and it is done
                // so, break the loop
                if(!m_q.pop(f)) break;
                f();
            }
        }

    private:
        const unsigned m_count{std::thread::hardware_concurrency()};
        std::vector<std::thread> m_threads;
        notification_queue m_q;
};
