#pragma once

#include <type_traits>



namespace silk
{


template<class Base>
class Basic_Output_Stream : public Base
{
public:
    typedef typename Base::Sample Sample;
    typedef typename Sample::Value Value;

    auto get_last_sample() const -> Sample const& { return m_last_sample; }
    auto get_samples() const -> std::vector<Sample> const& { return m_samples; }
    auto get_rate() const -> uint32_t { return m_rate; }

    void set_rate(uint32_t rate)
    {
        if (rate > 0)
        {
            m_dt = std::chrono::nanoseconds(1000000000 / rate);
        }
        else
        {
            m_dt = std::chrono::microseconds(0);
        }
        m_rate = rate;
    }
    void set_tp(q::Clock::time_point tp)
    {
        m_tp = tp;
    }

    auto get_dt() const -> q::Clock::duration
    {
        return m_dt;
    }
    auto get_tp() const -> q::Clock::time_point
    {
        return m_tp;
    }

//    void push_sample(Value const& value, q::Clock::duration dt, q::Clock::time_point tp, bool is_healthy)
//    {
//        m_last_sample.value = value;
//        m_last_sample.dt = dt;
//        m_last_sample.tp = tp;
//        m_last_sample.is_healthy = is_healthy;
//        m_samples.push_back(m_last_sample);
//    }

    void push_last_sample(bool is_healthy)
    {
        push_sample(m_last_sample.value, is_healthy);
    }

    void push_sample(Value const& value, bool is_healthy)
    {
        m_tp += m_dt;

        auto future = q::Clock::now() + m_dt * 5;
        if (m_tp > future && !m_future_warning)
        {
            m_future_warning = true;
            QLOGW("Samples from the future: {}", m_tp - q::Clock::now());
        }

        m_last_sample.value = value;
        m_last_sample.is_healthy = is_healthy;
        m_samples.push_back(m_last_sample);
    }

    void clear()
    {
        m_samples.clear();
        m_future_warning = false;
    }

    auto compute_samples_needed() -> size_t
    {
        auto dt = q::Clock::now() - m_tp;
        if (dt < q::Clock::duration(0))
        {
            return 0;
        }
        size_t samples_needed = dt / m_dt;
        m_samples.reserve(samples_needed);
        return samples_needed;
    }

private:
    q::Clock::duration m_dt;
    q::Clock::time_point m_tp = q::Clock::now();
    uint32_t m_rate = 0;
    std::vector<Sample> m_samples;
    Sample m_last_sample;
    bool m_future_warning = false;
};




}
