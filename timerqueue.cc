#include "timerqueue.h"
#include <thread>

namespace zrtc {

TimerQueue::TimerQueue() {
    Start();
}

TimerQueue::~TimerQueue() {
    Stop();
}

void TimerQueue::Start() {
    running_ = true;
    thread_ = std::thread([this]{
        while (running_) {
            std::this_thread::sleep_for( std::chrono::milliseconds(interval_ms_));
            WorkItem item;
            {
                std::unique_lock lk(mutex_);
                if (!tasks_.empty()) {
                    if (tasks_.begin()->first <= std::chrono::steady_clock::now()) {
                        item = std::move(tasks_.begin()->second);
                        tasks_.erase(tasks_.begin());
                    }
                }
                lk.unlock();
            }
            if (item.repeat) {
                std::lock_guard lg(mutex_);
                tasks_.emplace(std::chrono::steady_clock::now() + std::chrono::milliseconds(item.interval_ms),
                               std::move(repeat_tasks_[item.work_id].copy()));
            }

            if (item.work_queue) {
                item.work_queue->AddWork(item.work, std::move(item.promise));
            }
        }
    });
}

void TimerQueue::Stop() {
    running_ = false;
    thread_.join();
}

uint32_t TimerQueue::AddScheduledWork(WorkItem&& item) {
    std::lock_guard lg(mutex_);
    auto now = std::chrono::steady_clock::now();
    tasks_.emplace(now + std::chrono::milliseconds(item.interval_ms), std::move(item));
    return 0;
}

uint32_t TimerQueue::AddPeriodicWork(WorkItem&& item) {
    std::lock_guard lg(mutex_);
    item.work_id = ++task_id_;
    auto now = std::chrono::steady_clock::now();
    tasks_.emplace(now + std::chrono::milliseconds(item.interval_ms), std::move(item.copy()));

    repeat_tasks_.emplace(task_id_, std::move(item));
    return task_id_;
}

void TimerQueue::CancelPeriodicWork(uint32_t id) {
    std::lock_guard lg(mutex_);
    repeat_tasks_.erase(id);
}

TimerQueue& ZRTCGetGlobalTimerQueue() {
    static TimerQueue queue;
    return queue;
}

}
