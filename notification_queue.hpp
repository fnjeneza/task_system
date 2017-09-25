#pragma once

#include <functional>
#include <deque>
#include <mutex>
#include <condition_variable>

class notification_queue{
    public:
        bool pop(std::function<void()> & f)
        {
            std::unique_lock<std::mutex> lock(m_mutex);

            while(m_queue.empty() && !m_done)
            {
                // a notification is sent when a new task is added in the queue
                // or when we are done with the queue. Not needed anymore
                m_ready.wait(lock);
            }

            if(m_queue.empty())
            {
                return false;
            }

            f = std::move(m_queue.front());
            m_queue.pop_front();

            return true;
        }

        template<typename F>
        void push(F&& f)
        {
            {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_queue.emplace_back(std::forward<F>(f));
            }
            m_ready.notify_one();
        }

        void done()
        {
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_done = true;
            }
            m_ready.notify_all();
        }

    private:
        std::deque<std::function<void()>> m_queue;
        bool m_done{false};
        std::mutex m_mutex;
        std::condition_variable m_ready;
};
