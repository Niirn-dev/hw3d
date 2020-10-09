#include "ChiliTimer.h"

ChiliTimer::ChiliTimer() noexcept
    :
    last( std::chrono::steady_clock::now() )
{}

float ChiliTimer::Mark() noexcept
{
    const auto now = std::chrono::steady_clock::now();
    return std::chrono::duration<float>( now - std::exchange( last,now ) ).count();
}

float ChiliTimer::Peek() const noexcept
{
    const auto now = std::chrono::steady_clock::now();
    return std::chrono::duration<float>( now - last ).count();
}
