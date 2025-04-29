#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
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
std::condition_variable cv_not_full;
std::condition_variable cv_not_empty;
std::atomic<bool> producers_finished(false);
std::atomic<int> items_produced_count(0);
std::atomic<int> items_consumed_count(0);
std::atomic<int> active_producers(0);

void producer_task(int id);
void consumer_task(int id);

int main(int argc, char* argv[]) {
	if (argc != 3) {
		std::cerr << "Usage: " << argv[0] << " <num_producers> <num_consumers>" << std::endl;
		return 1;
	}

	int num_producers;
	int num_consumers;

	try {
		num_producers = std::stoi(argv[1]);
		num_consumers = std::stoi(argv[2]);
		if (num_producers <= 0 || num_consumers <= 0) {
			throw std::invalid_argument("Number of producers and consumers must be positive.");
		}
	} catch (const std::exception& e) {
		std::cerr << "Error parsing arguments: " << e.what() << std::endl;
		std::cerr << "Usage: " << argv[0] << " <num_producers> <num_consumers>" << std::endl;
		return 1;
	}

	std::cout << "--- Producer-Consumer Simulation ---" << std::endl;
	std::cout << "Producers: " << num_producers << std::endl;
	std::cout << "Consumers: " << num_consumers << std::endl;
	std::cout << "Buffer Size: " << BUFFER_SIZE << std::endl;
	std::cout << "Items per Producer: " << ITEMS_PER_PRODUCER << std::endl;
	std::cout << "Total Items to Produce: " << num_producers * ITEMS_PER_PRODUCER << std::endl;
	std::cout << "------------------------------------" << std::endl;

	std::vector<std::thread> producer_threads;
	std::vector<std::thread> consumer_threads;

	auto start_time = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < num_producers; ++i) {
		producer_threads.emplace_back(producer_task, i + 1);
	}

	for (int i = 0; i < num_consumers; ++i) {
		consumer_threads.emplace_back(consumer_task, i + 1);
	}

	for (auto& th : producer_threads) {
		if (th.joinable()) {
			th.join();
		}
	}
	for (auto& th : consumer_threads) {
		if (th.joinable()) {
			th.join();
		}
	}

	auto end_time = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

	std::cout << "--- Simulation Complete ---" << std::endl;
	std::cout << "Total items produced: " << items_produced_count.load() << std::endl;
	std::cout << "Total items consumed: " << items_consumed_count.load() << std::endl;
	std::cout << "Overall Turnaround Time: " << duration.count() << " ms" << std::endl;
	std::cout << "---------------------------" << std::endl;

	if (items_produced_count.load() != items_consumed_count.load()) {
		std::cerr << "Error: Mismatch between produced and consumed items!" << std::endl;
		return 1;
	}
	if (items_produced_count.load() != num_producers * ITEMS_PER_PRODUCER) {
		std::cerr << "Error: Did not produce the expected number of items!" << std::endl;
		return 1;
	}

	return 0;
}

void producer_task(int id) {
	active_producers++;
	for (int i = 0; i < ITEMS_PER_PRODUCER; ++i) {
		if (PRODUCER_SLEEP_MS > 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(PRODUCER_SLEEP_MS));
		}

		int item = items_produced_count.fetch_add(1); // Generate a unique item

		{
			std::unique_lock<std::mutex> lock(mtx);
			cv_not_full.wait(lock, [] { return buffer.size() < BUFFER_SIZE; });

			buffer.push(item);
			cv_not_empty.notify_one();
		}
	}
	if (active_producers.fetch_sub(1) == 1) {
		producers_finished.store(true);
		cv_not_empty.notify_all();
	}
}

void consumer_task(int id) {
	while (true) {
		int item;
		{
			std::unique_lock<std::mutex> lock(mtx);
			cv_not_empty.wait(lock, [] { return !buffer.empty() || producers_finished.load(); });

			if (buffer.empty() && producers_finished.load()) {
				break;
			}

			if (buffer.empty()) {
				continue;
			}

			item = buffer.front();
			buffer.pop();
			items_consumed_count++;

			cv_not_full.notify_one();
		}

		if (CONSUMER_SLEEP_MS > 0) {
			std::this_thread::sleep_for(std::chrono::milliseconds(CONSUMER_SLEEP_MS));
		}
	}
}
