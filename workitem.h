#pragma once
#include "workqueue.h"

namespace zrtc {

struct WorkItem {
    // work id
    uint32_t work_id = 0;
    // work that will be executed
    WorkQueue::Work work;
    // promise of this work
    std::optional<WorkQueue::Promise> promise;
    // which queue should be executed, null for this thread
    std::shared_ptr<WorkQueue> work_queue;
    // wait interval or repeat interval
    uint32_t interval_ms = 0;
    // identify this work is a repeated task
    bool repeat = false;

    WorkItem() = default;
    WorkItem(const WorkItem& other) = delete;
    WorkItem& operator=(const WorkItem&) = delete;

    WorkItem(WorkQueue::Work work,std::optional<WorkQueue::Promise> promise)
        : work(work), promise(std::move(promise)) {}

    WorkItem(WorkItem&& other) {
        work_id = other.work_id;
        work = other.work;
        promise = std::move(other.promise);
        work_queue = other.work_queue;
        interval_ms = other.interval_ms;
        repeat = other.repeat;
    }

    WorkItem& operator=(WorkItem&& other) {
        work_id = other.work_id;
        work = other.work;
        promise = std::move(other.promise);
        work_queue = other.work_queue;
        repeat = other.repeat;
        interval_ms = other.interval_ms;
        return *this;
    }

    WorkItem copy() {
        WorkItem item;
        item.work = work;
        item.work_queue = work_queue;
        item.interval_ms = interval_ms;
        item.work_id = work_id;
        item.promise = std::nullopt;
        item.repeat = repeat;
        return item;
    }

    bool valid() const { return work.operator bool(); }
};
}
