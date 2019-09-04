/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>

#include <geode/ProxyResultCollector.hpp>

namespace apache {
namespace geode {
namespace client {

ProxyResultCollector::ProxyResultCollector() {}

ProxyResultCollector::ProxyResultCollector(
    std::future<std::shared_ptr<ResultCollector>> inFutureRc)
    : futureRc(std::move(inFutureRc)) {}

ProxyResultCollector::~ProxyResultCollector() noexcept {}

std::shared_ptr<CacheableVector> ProxyResultCollector::getResult(
    std::chrono::milliseconds timeout) {
  if (!futureRc.valid()) {
    return nullptr;
  }
  if (timeout > std::chrono::milliseconds(0)) {
    std::future_status status = futureRc.wait_for(timeout);
    if (std::future_status::ready == status) {
      return futureRc.get()->getResult();
    }
    return nullptr;
  }
  return futureRc.get()->getResult();
}

void ProxyResultCollector::addResult(const std::shared_ptr<Cacheable>& result) {
}

void ProxyResultCollector::endResults() {}

void ProxyResultCollector::clearResults() {}

}  // namespace client

}  // namespace geode
}  // namespace apache
