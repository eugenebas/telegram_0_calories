#ifndef __TZC_QUEUED_EXECUTOR_HPP__
#define __TZC_QUEUED_EXECUTOR_HPP__

#include <atomic>
#include <functional>
#include <mutex>
#include <thread>
#include <queue>

enum class TaskExecutionResult {
    NotExecuted,
    Resubmit,
    Done
};

struct ITask {
    virtual ~ITask() = default;
    virtual TaskExecutionResult execute() = 0;
};

class QueuedExecutor final {
public:
    QueuedExecutor();
    QueuedExecutor(QueuedExecutor&&) = default;
    QueuedExecutor(const QueuedExecutor&) = delete;
    ~QueuedExecutor() = default;
    void submit(const std::shared_ptr<ITask>& task);
private:
    void run();

    std::atomic<bool> mDoRun;
    std::jthread mThread;
    std::queue<std::shared_ptr<ITask>> mQueue;
    std::mutex mMutex;
};

#endif // __TZC_QUEUED_EXECUTOR_HPP__
