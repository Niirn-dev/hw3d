#include "Mouse.h"

Mouse::Event::Event( Type type,int x,int y ) noexcept
    :
    type( type ),
    x( x ),
    y( y )
{}

bool Mouse::Event::LeftIsPressed() const noexcept
{
    return type == Event::Type::LPress;
}

bool Mouse::Event::RightIsPressed() const noexcept
{
    return type == Event::Type::RPress;
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
    buffer.push( Event{ Event::Type::LPress,x_in,y_in } );
    TrimBuffer();
}

void Mouse::OnLeftRelease( int x_in,int y_in ) noexcept
{
    leftIsPressed = false;
    x = x_in;
    y = y_in;
    buffer.push( Event{ Event::Type::LRelease,x_in,y_in } );
    TrimBuffer();
}

void Mouse::OnRightPress( int x_in,int y_in ) noexcept
{
    rightIsPressed = true;
    x = x_in;
    y = y_in;
    buffer.push( Event{ Event::Type::RPress,x_in,y_in } );
    TrimBuffer();
}

void Mouse::OnRightRelease( int x_in,int y_in ) noexcept
{
    rightIsPressed = false;
    x = x_in;
    y = y_in;
    buffer.push( Event{ Event::Type::RRelease,x_in,y_in } );
    TrimBuffer();
}

void Mouse::OnWheelUp( int x_in,int y_in ) noexcept
{
    x = x_in;
    y = y_in;
    buffer.push( Event{ Event::Type::WheelUp,x_in,y_in } );
    TrimBuffer();
}

void Mouse::OnWheelDown( int x_in,int y_in ) noexcept
{
    x = x_in;
    y = y_in;
    buffer.push( Event{ Event::Type::WheelDown,x_in,y_in } );
    TrimBuffer();
}

void Mouse::OnMove( int x_in,int y_in ) noexcept
{
    x = x_in;
    y = y_in;
    buffer.push( Event{ Event::Type::Move,x_in,y_in } );
    TrimBuffer();
}

void Mouse::TrimBuffer() noexcept
{
    while ( buffer.size() > bufferSize )
    {
        buffer.pop();
    }
}
