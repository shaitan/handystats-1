#include <vector>
#include <utility>
#include <cstdlib>
#include <thread>
#include <queue>
#include <string>
#include <map>
#include <memory>

#include <gtest/gtest.h>

#include <handystats/chrono.hpp>
#include <handystats/operation.hpp>
#include <handystats/measuring_points.hpp>
#include <handystats/json_dump.hpp>

#include "events/event_message_impl.hpp"
#include "message_queue_impl.hpp"
#include "internal_metrics_impl.hpp"
#include "internal_metrics/internal_timer_impl.hpp"

#include "message_queue_helper.hpp"

namespace handystats { namespace internal {

extern std::map<std::string, internal_metric> internal_metrics;

}} // namespace handystats::internal


class HandyScopedTimerTest : public ::testing::Test {
protected:
	virtual void SetUp() {
		HANDY_INIT();
	}
	virtual void TearDown() {
		HANDY_FINALIZE();
	}
};

TEST_F(HandyScopedTimerTest, TestSingleInstanceScopedTimer) {
	const int COUNT = 5;
	auto sleep_time = std::chrono::milliseconds(10);

	for (int step = 0; step < COUNT; ++step) {
		HANDY_TIMER_SCOPE("sleep.time");
		std::this_thread::sleep_for(sleep_time);
	}

	handystats::message_queue::wait_until_empty();
	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	ASSERT_TRUE(
			boost::get<handystats::internal::internal_timer*>(handystats::internal::internal_metrics["sleep.time"])
			->base_timer
			->instances.empty()
			);

	auto agg_stats =
		boost::get<handystats::internal::internal_timer*>(handystats::internal::internal_metrics["sleep.time"])
		->base_timer
		->values;

	ASSERT_EQ(agg_stats.count(), COUNT);
	ASSERT_GE(agg_stats.min(), handystats::chrono::duration_cast<handystats::chrono::time_duration>(sleep_time).count());

	std::cout << *HANDY_JSON_DUMP() << std::endl;
}

TEST_F(HandyScopedTimerTest, TestMultiInstanceScopedTimer) {
	const int COUNT = 10;
	auto sleep_time = std::chrono::milliseconds(1);

	for (int step = 0; step < COUNT; ++step) {
		HANDY_TIMER_SCOPE("sleep.time", step);
		std::this_thread::sleep_for(sleep_time);
	}

	handystats::message_queue::wait_until_empty();
	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	ASSERT_TRUE(
			boost::get<handystats::internal::internal_timer*>(handystats::internal::internal_metrics["sleep.time"])
			->base_timer
			->instances.empty()
			);

	auto agg_stats =
		boost::get<handystats::internal::internal_timer*>(handystats::internal::internal_metrics["sleep.time"])
		->base_timer
		->values;

	ASSERT_EQ(agg_stats.count(), COUNT);
	ASSERT_GE(agg_stats.min(), handystats::chrono::duration_cast<handystats::chrono::time_duration>(sleep_time).count());

	std::cout << *HANDY_JSON_DUMP() << std::endl;
}

TEST_F(HandyScopedTimerTest, TestSeveralScopedTimersInOneScope) {
	const int COUNT = 10;
	auto sleep_time = std::chrono::milliseconds(1);

	for (int step = 0; step < COUNT; ++step) {
		HANDY_TIMER_SCOPE("double.sleep.time", step);
		std::this_thread::sleep_for(sleep_time);
		HANDY_TIMER_SCOPE("sleep.time", step);
		std::this_thread::sleep_for(sleep_time);
	}

	handystats::message_queue::wait_until_empty();
	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	ASSERT_TRUE(
			boost::get<handystats::internal::internal_timer*>(handystats::internal::internal_metrics["sleep.time"])
			->base_timer
			->instances.empty()
			);

	ASSERT_TRUE(
			boost::get<handystats::internal::internal_timer*>(handystats::internal::internal_metrics["double.sleep.time"])
			->base_timer
			->instances.empty()
			);

	auto agg_stats =
		boost::get<handystats::internal::internal_timer*>(handystats::internal::internal_metrics["sleep.time"])
		->base_timer
		->values;

	ASSERT_EQ(agg_stats.count(), COUNT);
	ASSERT_GE(agg_stats.min(), handystats::chrono::duration_cast<handystats::chrono::time_duration>(sleep_time).count());

	auto double_agg_stats =
		boost::get<handystats::internal::internal_timer*>(handystats::internal::internal_metrics["double.sleep.time"])
		->base_timer
		->values;

	ASSERT_EQ(double_agg_stats.count(), COUNT);
	ASSERT_GE(double_agg_stats.min(), handystats::chrono::duration_cast<handystats::chrono::time_duration>(sleep_time).count() * 2);

	std::cout << *HANDY_JSON_DUMP() << std::endl;
}

