#pragma once

#include "HAL.h"
#include "common/node/bus/ISPI.h"

namespace silk
{
namespace node
{
namespace bus
{

class SPI_Linux : public ISPI
{
public:
    SPI_Linux(HAL& hal);
    ~SPI_Linux();

    struct Init_Params
    {
        std::string name;
        std::string dev;
        uint32_t mode = 0;
    };

    auto init(rapidjson::Value const& json) -> bool;
    auto init(Init_Params const& params) -> bool;
    void close();

    auto get_name() const -> std::string const&;

    void lock();
    auto try_lock() -> bool;
    void unlock();

    auto read(uint8_t* data, size_t size) -> bool;
    auto write(uint8_t const* data, size_t size) -> bool;

    auto read_register(uint8_t reg, uint8_t* data, size_t size) -> bool;
    auto write_register(uint8_t reg, uint8_t const* data, size_t size) -> bool;

private:
    HAL& m_hal;
    Init_Params m_params;
    int m_fd = -1;
    std::recursive_mutex m_mutex;
    std::vector<uint8_t> m_buffer;
};

}
}
}