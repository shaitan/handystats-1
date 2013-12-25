#ifndef HANDY_MONOTONIC_COUNTER_H_
#define HANDY_MONOTONIC_COUNTER_H_

#include <string>
#include <chrono>
#include <type_traits>

#include <handystats/chrono.hpp>
#include <handystats/counter.hpp>

namespace handystats {

template<class Value = int, class Duration = std::chrono::microseconds>
class monotonic_counter : public counter<monotonic_counter<Value, Duration>, Value, Duration>
{
public:
	// XXX check Value type for some condition (integer, arithmetic, ...)
	// or leave the decision to the user
	//static_assert(std::numeric_limits<Value>::is_integer, "value type must be integer!");
	static_assert(std::is_arithmetic<Value>::value, "Value type must be arithmetic!");

	typedef counter<monotonic_counter<Value, Duration>, Value, Duration> base_counter;

	typedef typename base_counter::value_type value_type;
	typedef typename base_counter::duration_type duration_type;
	typedef typename base_counter::clock clock;
	typedef typename base_counter::time_point time_point;

	monotonic_counter()
		: base_counter()
	{}

	monotonic_counter(value_type value, time_point initial_timestamp)
		: base_counter(value, initial_timestamp)
	{}

	void reset(value_type value = 0, time_point timestamp = clock::now()) {
		base_counter::reset(value, timestamp);
	}

	void increment(value_type inc_value = 1, time_point timestamp = clock::now()) {
		this->value += inc_value;
		this->update_timestamp(timestamp);
	}

	void incident(time_point timestamp = clock::now()) {
		increment(1, timestamp);
	}
};

} // namespace handystats

#endif // HANDY_MONOTONIC_COUNTER_H_
