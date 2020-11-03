#pragma once
#include <memory>
#include <vector>
#include <DirectXMath.h>
#include "Graphics.h"

class Bindable;

class Drawable
{
	template<class D>
	friend class DrawableBase;
public:
	Drawable() = default;
	Drawable( const Drawable& ) = delete;
	Drawable& operator=( const Drawable& ) = delete;
	virtual ~Drawable() = default;
	void Draw( Graphics& gfx ) noexcept( !IS_DEBUG );
	virtual void Update( float dt ) noexcept = 0;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
protected:
	void AddBind( std::unique_ptr<Bindable> pBind ) noexcept( !IS_DEBUG );
	void AddIndexBuffer( std::unique_ptr<class IndexBuffer> pBind ) noexcept( !IS_DEBUG );
private:
	virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept = 0;
private:
	const class IndexBuffer* pIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bindable>> binds;
};
