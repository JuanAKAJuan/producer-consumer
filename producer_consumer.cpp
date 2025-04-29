#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <numeric>
#include <queue>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

const int BUFFER_SIZE = 10;
const int ITEMS_PER_PRODUCER = 50;
const int PRODUCER_SLEEP_MS = 10;
const int CONSUMER_SLEEP_MS = 20;

std::queue<int> buffer;
std::mutex mtx;
std::condition_variable cvNotFull;
std::condition_variable cvNotEmpty;
std::atomic<bool> producersFinished(false);
std::atomic<int> itemsProducedCount(0);
std::atomic<int> itemsConsumedCount(0);
std::atomic<int> activeProducers(0);

void producerTask
