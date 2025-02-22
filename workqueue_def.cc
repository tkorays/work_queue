#include "workqueue_def.h"

namespace zrtc {

DefaultTaskQueue::DefaultTaskQueue(std::string name)
    : name_(name) {

}

void DefaultTaskQueue::Start() {
    running_ = true;
    thread_ = std::thread([this]{
        std::cout << "queue start" << std::endl;
        while (running_) {
            WorkItem task;
            {
                std::unique_lock lk(mutex_);
                if (!works_.empty()) {
                    task = std::move(works_.front());
                    works_.pop_front();
                } else {
                    cv_.wait_for(lk, std::chrono::milliseconds(100), [this] { return need_be_notified_; });
                }
                lk.unlock();
                if (!running_) {
                    std::cout << "exit queue" << std::endl;
                    break;
                }
            }
            if (task.valid()) {
                task.work();
                if (task.promise.has_value()) {
                    task.promise->set_value();
                }
            }
        }
        std::cout << "queue stop" << std::endl;
    });
}

void DefaultTaskQueue::Stop() {
    running_ = false;
    {
        std::lock_guard lg(mutex_);
        need_be_notified_ = true;
        cv_.notify_one();
    }
    thread_.join();
}

void DefaultTaskQueue::AddWork(std::function<void()> task, std::optional<Promise> promise) {
    std::lock_guard lg(mutex_);
    WorkItem item(task, std::move(promise));
    works_.emplace_back(std::move(item));
    need_be_notified_ = true;
    cv_.notify_one();
}

void DefaultTaskQueue::AddScheduledWork(uint32_t interval_ms, Work task, std::optional<Promise> promise) {
    WorkItem item;
    item.work = task;
    item.work_queue = shared_from_this();
    item.interval_ms = true;
    item.promise = std::move(promise);
    ZRTCGetGlobalTimerQueue().AddScheduledWork(std::move(item));
}

uint32_t DefaultTaskQueue::AddPeriodicWork(uint32_t interval_ms, Work task) {
    WorkItem item;
    item.work = task;
    item.work_queue = shared_from_this();
    item.interval_ms = true;
    item.repeat = true;
    return ZRTCGetGlobalTimerQueue().AddPeriodicWork(std::move(item));
}

void DefaultTaskQueue::CancelPeriodicWork(uint32_t id) {
    ZRTCGetGlobalTimerQueue().CancelPeriodicWork(id);
}

}
