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
#include <cstdlib>
#include <thread>
#include <chrono>

#include <geode/CacheFactory.hpp>
#include <geode/CacheTransactionManager.hpp>
#include <geode/PoolManager.hpp>
#include <geode/RegionFactory.hpp>
#include <geode/RegionShortcut.hpp>

using namespace apache::geode::client;

class OverloaderTask {
  public:

  OverloaderTask() {
    stop = false;
    pause_ = false;
    processedOps = 0;
    failedOps = 0;
    sentOps = 0;
    thread = NULL;
  }

  void putFunction(const std::shared_ptr<Region> region, int entries) {
	  while(true) {
  		  populateRegion(region,entries);
  		  std::cout << "populateRegion loop"<< std::endl;
  	  }
  }

  void getFunction(const std::shared_ptr<Region> region, int entries, const std::shared_ptr<CacheTransactionManager> transactionManager) {
	  auto sleepTime = (rand() % 5 ) + 1;
	  auto start = std::chrono::system_clock::now();
	  std::this_thread::sleep_for(std::chrono::seconds(sleepTime));
	  auto end = std::chrono::system_clock::now();
	  std::chrono::duration<float,std::milli> duration = end - start;

	  while(true) {
		  for (int j = 0; j < entries; j++) {
			  try{
				  transactionManager->begin();
				  region->get(j);
				  transactionManager->commit();
			  }catch ( ... ){
				  transactionManager->rollback();
			  }
		  }
	  }
  }

  void setStop(bool stop) {
    this->stop = stop;
  }

  void pause() {
    pause_ = true;
  }

  void resume() {
    pause_ = false;
  }

  int getProcessedOps() {
    return processedOps;
  }

  int getSentOps() {
    return sentOps;
  }

  int getFailedOps() {
    return failedOps;
  }

  std::shared_ptr<std::thread> getThread() {
    return thread;
  }

  void setThread(std::shared_ptr<std::thread> thread) {
    this->thread = thread;
  }

  void populateRegion(const std::shared_ptr<Region> region, int entries) {
  	for (int i = 0; i < entries; i++) {
      auto value = std::to_string(rand());
  	  region->put(i, value);
  	}
  }

  private:
    std::atomic<bool> stop;
    std::atomic<bool> pause_;
    int processedOps;
    int failedOps;
    int sentOps;
    std::shared_ptr<std::thread> thread;
}; //class OverloaderTask



void cleanRegion(const std::shared_ptr<Region> region, int entries) {
	std::cout << "Cleaning region" << std::endl;
	for (int i = 0; i < entries; i++) {
		auto value = std::to_string(i);
		region->remove(i);
	}
}

void printUsage(){
    std::cout << "Usage:"<< std::endl;
    std::cout << "    $> ./cpp-putgetremove <entries> <threads>" << std::endl;
    std::cout << "where"<<std::endl;
    std::cout << "   <entries> = number of entries to be populated" << std::endl;
    std::cout << "   <threads> = threads to be used. One of them will be used to populate, the rest for run transactions." << std::endl;
    std::cout << std::endl;
}


int main(int argc, char** argv) {

    if (argc != 3) {
        printUsage();
    }

    auto entries = atoi(argv[1]);
    auto threads = atoi(argv[2]);

    auto cacheFactory = CacheFactory();
    cacheFactory.set("log-level", "none");
    auto cache = cacheFactory.create();
    auto poolFactory = cache.getPoolManager().createFactory();

    poolFactory.addLocator("localhost", 10334);
    auto pool = poolFactory.create("pool");
    auto regionFactory = cache.createRegionFactory(RegionShortcut::PROXY);
    auto region = regionFactory.setPoolName("pool").create("example_userinfo");

    auto transactionManager = cache.getCacheTransactionManager();
    std::vector<std::shared_ptr<OverloaderTask>> tasks;
    for (int i = 0; i<threads; i++) {
        auto task = std::make_shared<OverloaderTask>();
        std::shared_ptr<std::thread> threadAux;
        if (i==0){
            std::cout << "Creating 'put' thread..." << std::endl;
            threadAux = std::make_shared<std::thread>(&OverloaderTask::putFunction, task, region, entries);
        }else{
            std::cout << "Creating 'get' thread #" << i << "..." << std::endl;
            threadAux = std::make_shared<std::thread>(&OverloaderTask::getFunction, task, region, entries, transactionManager);
        }
        task->setThread(threadAux);
        tasks.push_back(task);
    }

    std::this_thread::sleep_for(std::chrono::seconds(10));
    for (int i = 0; i<tasks.size(); i++) {
        std::cout << "Waiting: " << i << std::endl;
        tasks[i]->setStop(true);
        tasks[i]->getThread()->join();
    }

    cleanRegion(region,entries);
    cache.close();
}//main
