#include "queued_executor.hpp"
#include <iostream>

QueuedExecutor::QueuedExecutor() :
    mDoRun(true),
    mThread([this](){run();}) {
}

void QueuedExecutor::submit(const std::shared_ptr<ITask> &task) {
    std::lock_guard guard(mMutex);
    mQueue.push(task);
}

void QueuedExecutor::run() {
    while(mDoRun) {
        std::shared_ptr<ITask> task;
        {
            std::lock_guard lock(mMutex);
            if(mQueue.size() > 0) {
                task = mQueue.front();
                mQueue.pop();
            }
        }
        if(task) {
            auto executionResult = TaskExecutionResult::NotExecuted;
            try {
                executionResult = task->execute();
            } catch(...) {
                std::cerr << "Error during task execution" << std::endl;
            }
            if(executionResult == TaskExecutionResult::Resubmit) {
                submit(task);
            }
        } else {
            std::this_thread::yield();
        }
    }
}
