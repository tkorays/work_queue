#pragma once
#include <cstdint>
#include <future>
#include <optional>

namespace zrtc {

class WorkQueue {
public:
    typedef std::promise<void> Promise;
    typedef std::function<void()> Work;

    virtual void Start() = 0;
    virtual void Stop() = 0;
    virtual void AddWork(Work work, std::optional<Promise> promise) = 0;
    virtual void AddScheduledWork(uint32_t interval_ms, Work work, std::optional<Promise> promise) = 0;
    virtual uint32_t AddPeriodicWork(uint32_t interval_ms, Work work) = 0;
    virtual void CancelPeriodicWork(uint32_t id) = 0;
};

}
