#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include "DataProcess.h"

const int RefreshTime = 1000;//ms

namespace ThreadPool {
class ThreadPool {
 public:
  ThreadPool(size_t num_threads);
  ~ThreadPool();

  // job 을 추가한다.
  template <class F, class... Args>
  std::future<typename std::result_of<F(Args...)>::type> EnqueueJob(
    F&& f, Args&&... args);

 private:
  // 총 Worker 쓰레드의 개수.
  size_t num_threads_;
  // Worker 쓰레드를 보관하는 벡터.
  std::vector<std::thread> worker_threads_;
  // 할일들을 보관하는 job 큐.
  std::queue<std::function<void()>> jobs_;
  // 위의 job 큐를 위한 cv 와 m.
  std::condition_variable cv_job_q_;
  std::mutex m_job_q_;

  // 모든 쓰레드 종료
  bool stop_all;

  // Worker 쓰레드
  void WorkerThread();
};

ThreadPool::ThreadPool(size_t num_threads)
    : num_threads_(num_threads), stop_all(false) {
  worker_threads_.reserve(num_threads_);
  for (size_t i = 0; i < num_threads_; ++i) {
    worker_threads_.emplace_back([this]() { this->WorkerThread(); });
  }
}

void ThreadPool::WorkerThread() {
  while (true) {
    std::unique_lock<std::mutex> lock(m_job_q_);
    cv_job_q_.wait(lock, [this]() { return !this->jobs_.empty() || stop_all; });
    if (stop_all && this->jobs_.empty()) {
      return;
    }

    // 맨 앞의 job 을 뺀다.
    std::function<void()> job = std::move(jobs_.front());
    jobs_.pop();
    lock.unlock();

    // 해당 job 을 수행한다 :)
    job();
  }
}

ThreadPool::~ThreadPool() {
  stop_all = true;
  cv_job_q_.notify_all();

  for (auto& t : worker_threads_) {
    t.join();
  }
}

template <class F, class... Args>
std::future<typename std::result_of<F(Args...)>::type> ThreadPool::EnqueueJob(
  F&& f, Args&&... args) {
  if (stop_all) {
    throw std::runtime_error("ThreadPool 사용 중지됨");
  }

  using return_type = typename std::result_of<F(Args...)>::type;
  auto job = std::make_shared<std::packaged_task<return_type()>>(
    std::bind(std::forward<F>(f), std::forward<Args>(args)...));
  std::future<return_type> job_result_future = job->get_future();
  {
    std::lock_guard<std::mutex> lock(m_job_q_);
    jobs_.push([job]() { (*job)(); });
  }
  cv_job_q_.notify_one();

  return job_result_future;
}

}  // namespace ThreadPool

BYTE readByte(FILE* stream, long& pos){
  BYTE result = 0;
  fseek(stream,pos,SEEK_SET);
  fread(&result, sizeof(BYTE), 1, stream);
  return result;
}
BYTES readBytes(FILE* stream, long& pos){
  BYTES result = 0;
  fseek(stream,pos,SEEK_SET);
  fread(&result, sizeof(BYTES), 1, stream);
  return result;
}
float readFloat(FILE* stream, long& pos){
  float result  = 0;
  fseek(stream,pos,SEEK_SET);
  fread(&result, sizeof(float), 1, stream);
  return result;
}

void writeByte(FILE* stream, long& pos, BYTE& sender){
  fseek(stream,pos,SEEK_SET);
  fwrite(&sender, sizeof(BYTE), 1, stream);
}
void writeBytes(FILE* stream, long& pos, BYTES& sender){
  fseek(stream,pos,SEEK_SET);
  fwrite(&sender, sizeof(BYTES), 1, stream);
}
void writeFloat(FILE* stream, long& pos, float& sender){
  fseek(stream,pos,SEEK_SET);
  fwrite(&sender, sizeof(float), 1, stream);
}

ThreadPool::ThreadPool pool(3);

void ffread(FILE* stream, long& pos, BYTE& sender){
  std::future<BYTE> future;
  future = pool.EnqueueJob(readByte, stream, pos);
  sender = future.get();
}
void ffread(FILE* stream, long& pos, BYTES& sender){
  std::future<BYTES> future;
  future = pool.EnqueueJob(readBytes, stream, pos);
  sender = future.get();
}
void ffread(FILE* stream, long& pos, float& sender){
  std::future<float> future;
  future = pool.EnqueueJob(readFloat, stream, pos);
  sender = future.get();
}

void ffwrite(FILE* stream, long& pos, BYTE& sender){
  std::future<void> future;
  future = pool.EnqueueJob(writeByte, stream, pos, sender);
  future.wait();
}
void ffwrite(FILE* stream, long& pos, BYTES& sender){
  std::future<void> future;
  future = pool.EnqueueJob(writeBytes, stream, pos, sender);
  future.wait();
}
void ffwrite(FILE* stream, long& pos, float& sender){
  std::future<void> future;
  future = pool.EnqueueJob(writeFloat, stream, pos, sender);
  future.wait();
}

void CycleThread() {
	thread([&]
	{
		while (true)
		{
			this_thread::sleep_for(chrono::milliseconds(RefreshTime));
			UnloadProcess();
			ShowProcess();
		}
	}).detach();
}