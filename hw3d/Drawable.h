#pragma once
#include <memory>
#include <vector>
#include <DirectXMath.h>
#include "Graphics.h"

class Bindable;

class Drawable
{
public:
	Drawable() = default;
	Drawable( const Drawable& ) = delete;
	Drawable& operator=( const Drawable& ) = delete;
	virtual ~Drawable() = default;
	void Draw( Graphics& gfx ) noexcept( !IS_DEBUG );
	virtual void Update( float dt ) noexcept = 0;
	void AddBind( std::unique_ptr<Bindable> pBind ) noexcept;
	void AddIndexBuffer( std::unique_ptr<class IndexBuffer> pBind ) noexcept;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
private:
	const IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bindable>> binds;
};
