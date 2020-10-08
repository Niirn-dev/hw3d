#include "Keyboard.h"

bool Keyboard::KeyIsPressed( unsigned char keycode ) const noexcept
{
    return keystates[keycode];
}

std::optional<Keyboard::Event> Keyboard::ReadKey() noexcept
{
    if ( !KeyIsEmpty() )
    {
        const auto e = keyBuffer.front();
        keyBuffer.pop();
        return e;
    }
    return std::nullopt;
}

bool Keyboard::KeyIsEmpty() const noexcept
{
    return keyBuffer.empty();
}

void Keyboard::FlushKey() noexcept
{
    keyBuffer = std::queue<Event>{};
}

char Keyboard::ReadChar() noexcept
{
    if ( !CharIsEmpty() )
    {
        const auto c = charBuffer.front();
        charBuffer.pop();
        return c;
    }
    return 0;
}

bool Keyboard::CharIsEmpty() const noexcept
{
    return charBuffer.empty();
}

void Keyboard::FlushChar() noexcept
{
    charBuffer = std::queue<char>{};
}

void Keyboard::Flush() noexcept
{
    FlushKey();
    FlushChar();
}

void Keyboard::EnableAutorepeat() noexcept
{
    autorepeattIsEnabled = true;
}

void Keyboard::DisableAutorepeat() noexcept
{
    autorepeattIsEnabled = false;
}

bool Keyboard::AutorepeatIsEnabled() const noexcept
{
    return autorepeattIsEnabled;
}

void Keyboard::OnKeyPressed( unsigned char keycode ) noexcept
{
    keystates[keycode] = true;
    keyBuffer.push( Keyboard::Event{ Keyboard::Event::Type::Press,keycode } );
    TrimBuffer( keyBuffer );
}

void Keyboard::OnKeyReleased( unsigned char keycode ) noexcept
{
    keystates[keycode] = false;
    keyBuffer.push( Keyboard::Event{ Keyboard::Event::Type::Release,keycode } );
    TrimBuffer( keyBuffer );
}

void Keyboard::OnChar( char c ) noexcept
{
    charBuffer.push( c );
    TrimBuffer( charBuffer );
}

void Keyboard::ClearState() noexcept
{
    keystates.reset();
}

template<typename T>
void Keyboard::TrimBuffer( std::queue<T>& buffer ) noexcept
{
    while ( buffer.size() > bufferSize )
    {
        buffer.pop();
    }
}
