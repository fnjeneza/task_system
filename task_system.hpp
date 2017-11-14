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
            for (unsigned n = 0; n != m_count * 48; ++n)
            {
                if (m_qs[(i + n) % m_count].try_push(std::forward<F>(f)))
                {
                    return;
                }
            }
            m_qs[i % m_count].push(std::forward<F>(f));
        }

    private:
      void run(unsigned i)
      {
          while (true)
          {
              std::function<void()> f;
              for (unsigned n = 0; n != m_count; ++n)
              {
                  if (!m_qs[n % m_count].try_pop(f))
                  {
                      break;
                  }
              }

              if (!f && !m_qs[i].pop(f))
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
