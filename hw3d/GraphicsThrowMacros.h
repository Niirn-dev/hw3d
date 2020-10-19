#pragma once

#define GFX_EXEPT_NOINFO(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall) if( HRESULT _hr; FAILED( _hr = (hrcall) ) ) throw Graphics::HrException( __LINE__,__FILE__,_hr )

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if ( HRESULT _hr; FAILED( _hr = (hrcall) ) ) throw GFX_EXCEPT( _hr )
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_ONLYINFO(call) infoManager.Set(); (call); if ( auto msgs = infoManager.GetMessages(); !msgs.empty() ) throw Graphics::InfoException( __LINE__,__FILE__,msgs )
#else
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO( (hrcall) )
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_ONLYINFO(call) (call)
#endif // !NDEBUG