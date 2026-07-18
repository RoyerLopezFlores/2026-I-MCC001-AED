#include <queue>
#include <mutex>
#include <condition_variable>



template<typename T>
class BlockingQueue{
private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_finished = false;
public:
    void push(T&& value){
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_queue.push(std::move(value));
        }
        m_cv.notify_one();
    }

    bool pop(T& value)
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        m_cv.wait(lock, [&] {
            return m_finished || !m_queue.empty();
        });

        if (m_queue.empty())
            return false;

        value = std::move(m_queue.front());
        m_queue.pop();

        return true;
    }

    void finish()
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_finished = true;
        }

        m_cv.notify_all();
    }


};