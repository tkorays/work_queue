#pragma once
#include "workqueue.h"
#include "workitem.h"
#include "timerqueue.h"
#include <thread>
#include <deque>
#include <iostream>

namespace zrtc {

class DefaultTaskQueue : public WorkQueue, public std::enable_shared_from_this<DefaultTaskQueue> {

public:
    DefaultTaskQueue(std::string name);
    ~DefaultTaskQueue();

    void Start() override;
    void Stop() override;
    void AddWork(Work task, std::optional<Promise> promise) override;
    void AddScheduledWork(uint32_t interval_ms, Work task, std::optional<Promise> promise) override;
    uint32_t AddPeriodicWork(uint32_t interval_ms, Work task) override;
    void CancelPeriodicWork(uint32_t id) override;

private:
    std::deque<WorkItem> works_;
    std::string name_;
    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable cv_;
    bool running_ = false;
    bool need_be_notified_ = false;
};
}
