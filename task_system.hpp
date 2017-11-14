#pragma once

#include <atomic>
#include <thread>
#include <vector>

#include "notification_queue.hpp"

class task_system{

    public:
        task_system()
        {
            for(unsigned n = 0; n != m_count; ++n)
            {
                m_threads.emplace_back([&, n] { run(n); });
            }
        }

        ~task_system()
        {
            for (auto &q : m_qs)
            {
                q.done();
            }
            for(auto & t: m_threads)
            {
                t.join();
            }
        }

        template<typename F>
        void async(F&& f)
        {
            auto i = m_index++;
            m_qs[i % m_count].push(std::forward<F>(f));
        }

    private:
      void run(unsigned i)
      {
          while (true)
          {
              std::function<void()> f;
              // retrieve the function to be run
              // if false means the queue is empty and it is done
              // so, break the loop
              if (!m_qs[i].pop(f))
              {
                  break;
              }
              f();
          }
        }

    private:
        const unsigned m_count{std::thread::hardware_concurrency()};
        std::vector<std::thread> m_threads;
        std::vector<notification_queue> m_qs{m_count};
        std::atomic<unsigned>           m_index{0};
};
