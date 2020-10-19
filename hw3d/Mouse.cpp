#include "Mouse.h"
#include "WinFlags.h"

Mouse::Event::Event( Type type,const Mouse& parent ) noexcept
    :
    type( type ),
    x( parent.x ),
    y( parent.y ),
    leftIsPressed( parent.leftIsPressed ),
    rightIsPressed( parent.rightIsPressed )
{}

bool Mouse::Event::LeftIsPressed() const noexcept
{
    return leftIsPressed;
}

bool Mouse::Event::RightIsPressed() const noexcept
{
    return rightIsPressed;
}

std::pair<int,int> Mouse::Event::GetPos() const noexcept
{
    return { x,y };
}

int Mouse::Event::GetX() const noexcept
{
    return x;
}

int Mouse::Event::GetY() const noexcept
{
    return x;
}

Mouse::Event::Type Mouse::Event::GetType() const noexcept
{
    return type;
}

bool Mouse::LeftIsPressed() const noexcept
{
    return leftIsPressed;
}

bool Mouse::RightIsPressed() const noexcept
{
    return rightIsPressed;
}

std::pair<int,int> Mouse::GetPos() const noexcept
{
    return { x,y };
}

int Mouse::GetX() const noexcept
{
    return x;
}

int Mouse::GetY() const noexcept
{
    return y;
}

bool Mouse::IsInWindow() const noexcept
{
    return isInWindow;
}

std::optional<Mouse::Event> Mouse::Read() noexcept
{
    if ( !IsEmpty() )
    {
        const auto e = buffer.front();
        buffer.pop();
        return e;
    }
    return std::nullopt;
}

void Mouse::Flush() noexcept
{
    buffer = std::queue<Event>{};
}

bool Mouse::IsEmpty() const noexcept
{
    return buffer.empty();
}

void Mouse::OnLeftPress( int x_in,int y_in ) noexcept
{
    leftIsPressed = true;
    x = x_in;
    y = y_in;
    buffer.push( Event{ Event::Type::LPress,*this } );
    TrimBuffer();
}

void Mouse::OnLeftRelease( int x_in,int y_in ) noexcept
{
    leftIsPressed = false;
    x = x_in;
    y = y_in;
    buffer.push( Event{ Event::Type::LRelease,*this } );
    TrimBuffer();
}

void Mouse::OnRightPress( int x_in,int y_in ) noexcept
{
    rightIsPressed = true;
    x = x_in;
    y = y_in;
    buffer.push( Event{ Event::Type::RPress,*this } );
    TrimBuffer();
}

void Mouse::OnRightRelease( int x_in,int y_in ) noexcept
{
    rightIsPressed = false;
    x = x_in;
    y = y_in;
    buffer.push( Event{ Event::Type::RRelease,*this } );
    TrimBuffer();
}

void Mouse::OnWheelUp() noexcept
{
    buffer.push( Event{ Event::Type::WheelUp,*this } );
    TrimBuffer();
}

void Mouse::OnWheelDown() noexcept
{
    buffer.push( Event{ Event::Type::WheelDown,*this } );
    TrimBuffer();
}

void Mouse::OnWheelDelta( int delta ) noexcept
{
    wheelDelta += delta;
    while ( wheelDelta >= WHEEL_DELTA )
    {
        wheelDelta -= WHEEL_DELTA;
        OnWheelUp();
    }
    while ( wheelDelta <= -WHEEL_DELTA )
    {
        wheelDelta += WHEEL_DELTA;
        OnWheelDown();
    }
}

void Mouse::OnMouseMove( int x_in,int y_in ) noexcept
{
    x = x_in;
    y = y_in;
    buffer.push( Event{ Event::Type::Move,*this } );
    TrimBuffer();
}

void Mouse::OnMouseEnter() noexcept
{
    isInWindow = true;
    buffer.push( Event{ Event::Type::Enter,*this } );
    TrimBuffer();
}

void Mouse::OnMouseLeave() noexcept
{
    isInWindow = false;
    buffer.push( Event{ Event::Type::Leave,*this } );
    TrimBuffer();
}

void Mouse::TrimBuffer() noexcept
{
    while ( buffer.size() > bufferSize )
    {
        buffer.pop();
    }
}
