#pragma once

#include <utility>
#include <optional>
#include <queue>

class Mouse
{
	friend class Window;
public:
	class Event
	{
	public:
		enum class Type
		{
			LPress,
			LRelease,
			RPress,
			RRelease,
			WheelUp,
			WheelDown,
			Move,
			Enter,
			Leave
		};
	public:
		Event( Type type,const Mouse& parent ) noexcept;
		bool LeftIsPressed() const noexcept;
		bool RightIsPressed() const noexcept;
		std::pair<int,int> GetPos() const noexcept;
		int GetX() const noexcept;
		int GetY() const noexcept;
		Type GetType() const noexcept;
	private:
		Type type;
		int x;
		int y;
		bool leftIsPressed;
		bool rightIsPressed;
	};
public:
	Mouse() = default;
	Mouse( const Mouse& ) = delete;
	Mouse& operator=( const Mouse& ) = delete;
	// button states
	bool LeftIsPressed() const noexcept;
	bool RightIsPressed() const noexcept;
	// position getters
	std::pair<int,int> GetPos() const noexcept;
	int GetX() const noexcept;
	int GetY() const noexcept;
	bool IsInWindow() const noexcept;
	 // event stuff
	std::optional<Event> Read() noexcept;
	void Flush() noexcept;
	bool IsEmpty() const noexcept;
private:
	void OnLeftPress( int x_in,int y_in ) noexcept;
	void OnLeftRelease( int x_in,int y_in ) noexcept;
	void OnRightPress( int x_in,int y_in ) noexcept;
	void OnRightRelease( int x_in,int y_in ) noexcept;
	void OnWheelUp() noexcept;
	void OnWheelDown() noexcept;
	void OnWheelDelta( int delta ) noexcept;
	void OnMouseMove( int x_in,int y_in ) noexcept;
	void OnMouseEnter() noexcept;
	void OnMouseLeave() noexcept;
	void TrimBuffer() noexcept;
private:
	bool leftIsPressed = false;
	bool rightIsPressed = false;
	bool isInWindow = false;
	int x = 0;
	int y = 0;
	int wheelDelta = 0;
	static constexpr unsigned int bufferSize = 16u;
	std::queue<Event> buffer;
};

