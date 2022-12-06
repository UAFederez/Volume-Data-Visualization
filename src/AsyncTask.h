#pragma once

#include <future>
#include <chrono>
#include <optional>

namespace vr
{
    template <typename T>
    struct AsyncTask
    {
        AsyncTask() = default;

        template<typename Func, typename ... Args>
        void Dispatch(Func f, Args ... args)
        {
            m_taskFuture = std::async(std::launch::async, f, args...);
            m_hasStarted = true;
        }

        bool IsInProgress() const
        {
            return m_hasStarted && !m_hasFinished;
        }

        bool HasRetrieved() const
        {
            return m_hasRetrieved;
        }

        bool CheckIfFinished() 
        { 
            if (IsInProgress() && m_taskFuture.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
            {
                m_hasFinished = true;
                m_hasStarted  = false;
                return false;
            }
            return m_hasFinished;
        }

        T GetResult()
        {
            m_hasRetrieved = true;
            return m_taskFuture.get();
        }

        std::future<T> m_taskFuture;
        bool m_hasStarted   = false;
        bool m_hasFinished  = false;
        bool m_hasRetrieved = false;
    };
}