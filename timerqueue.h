#pragma once
#include "workitem.h"
#include <map>
#include <chrono>

namespace zrtc {

class TimerQueue {
public:
    TimerQueue();
    ~TimerQueue();
    void Start();
    void Stop(); 

    uint32_t AddScheduledWork(WorkItem&& item);
    uint32_t AddPeriodicWork(WorkItem&& item);
    void CancelPeriodicWork(uint32_t id);
private:
    uint32_t interval_ms_ = 5;
    uint32_t task_id_ = 0;
    std::thread thread_;

    std::mutex mutex_;
    bool running_ = false;
    std::multimap<std::chrono::steady_clock::time_point, WorkItem> tasks_;
    std::map<uint32_t, WorkItem> repeat_tasks_;
};

TimerQueue& ZRTCGetGlobalTimerQueue();

}
