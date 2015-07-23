/*
 *
 * Copyright 2015, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef GRPCXX_DYNAMIC_THREAD_POOL_H
#define GRPCXX_DYNAMIC_THREAD_POOL_H

#include <grpc++/config.h>

#include <grpc++/impl/sync.h>
#include <grpc++/impl/thd.h>
#include <grpc++/thread_pool_interface.h>

#include <list>
#include <queue>

namespace grpc {

class DynamicThreadPool GRPC_FINAL : public ThreadPoolInterface {
 public:
  explicit DynamicThreadPool(int reserve_threads);
  ~DynamicThreadPool();

  void Add(const std::function<void()>& callback) GRPC_OVERRIDE;

 private:
  class DynamicThread {
  public:
    DynamicThread(DynamicThreadPool *pool);
    ~DynamicThread();
  private:
    DynamicThreadPool *pool_;
    std::unique_ptr<grpc::thread> thd_;
    void ThreadFunc();
  };
  grpc::mutex mu_;
  grpc::condition_variable cv_;
  grpc::condition_variable shutdown_cv_;
  bool shutdown_;
  std::queue<std::function<void()>> callbacks_;
  int reserve_threads_;
  int nthreads_;
  int threads_waiting_;
  std::list<DynamicThread*> live_threads_;
  std::list<DynamicThread*> dead_threads_;

  void ThreadFunc();
  static void ReapThreads(std::list<DynamicThread*>* tlist);
};

}  // namespace grpc

#endif  // GRPCXX_DYNAMIC_THREAD_POOL_H
