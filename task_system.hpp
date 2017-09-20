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
                m_threads.emplace_back([&, n]{ run(n); });
            }
        }

        ~task_system()
        {
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
        void run(unsigned i)
        {
            while(true)
            {
                std::function<void()> f;
                m_q.pop(f);
                f();
            }
        }

    private:
        const unsigned m_count{std::thread::hardware_concurrency()};
        std::vector<std::thread> m_threads;
        notification_queue m_q;
};
