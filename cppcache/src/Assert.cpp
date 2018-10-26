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

#include "Assert.hpp"

#include <sstream>

#include <boost/stacktrace.hpp>

#include <geode/ExceptionTypes.hpp>

#include "util/Log.hpp"

namespace apache {
namespace geode {
namespace client {

void Assert::throwAssertion(const char* expressionText, const char* file,
                            int line) {
  AssertionException ae(expressionText);
  LOGERROR("AssertionException: ( %s ) at %s:%d", expressionText, file, line);
  std::stringstream ss;
  ss << boost::stacktrace::stacktrace();
  LOGERROR(ss.str().c_str());
  throw ae;
}
}  // namespace client
}  // namespace geode
}  // namespace apache