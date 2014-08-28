#include <thread>

#include <gtest/gtest.h>

#include <handystats/rapidjson/allocators.h>

#include <handystats/chrono.hpp>
#include <handystats/metrics/timer.hpp>

#include <handystats/json/timer_json_writer.hpp>

using namespace handystats::metrics;

TEST(TimerTest, CheckTimerCountsRunningTimeCorrectly) {
	timer inter;
	inter.start();

	std::this_thread::sleep_for(handystats::chrono::time_duration(100));

	inter.stop();

	ASSERT_GT(handystats::chrono::duration_cast<handystats::chrono::time_duration>(inter.value).count(), 100);
}

TEST(Timer, CheckNotStartedTimerHasNoDuration) {
	timer inter;

	std::this_thread::sleep_for(handystats::chrono::time_duration(100));

	ASSERT_EQ(handystats::chrono::duration_cast<handystats::chrono::time_duration>(inter.value).count(), 0);
}

TEST(TimerTest, CheckNotStoppedTimerHasNoDuration) {
	timer inter;
	inter.start();

	std::this_thread::sleep_for(handystats::chrono::time_duration(100));

	ASSERT_EQ(handystats::chrono::duration_cast<handystats::chrono::time_duration>(inter.value).count(), 0);
}
