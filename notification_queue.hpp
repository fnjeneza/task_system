#pragma once

#include <functional>
#include <deque>
#include <mutex>

class notification_queue{
    public:
        void pop(std::function<void()> & f)
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            while(m_queue.empty())
            {
                m_ready.wait(lock);
            }
            f = std::move(m_queue.front());
            m_queue.pop_front();
        }

        template<typename F>
        void push(F&& f)
        {
            {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_queue.emplace_back(std::forward<F>(f));
            }
            m_ready.notify_one()
        }

        void done()
        {
            {
                std::unique_ptr<std::mutex> lock(m_mutex);
                m_done = true
            }
            m_ready.notify_all();
        }

    private:
        std::deque<function<void()> m_queue;
        bool m_done;
        std::mutex m_mutex;
        std::condition_variable m_ready;
};
