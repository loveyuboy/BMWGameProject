#pragma once
#ifndef	_COMMONDEF_H_
#define	_COMMONDEF_H_

#define DECLARE_SINGLETON( ClassName ) protected: static ClassName* spSingleton; public: static void Initialize( ); static void Finalize( ); static bool IsInitialized( ); static ClassName& GetSingleton( );

#define IMPLEMENT_SINGLETON( ClassName ) ClassName* ClassName::spSingleton = NULL; void ClassName::Initialize( ) { if ( spSingleton == NULL ) spSingleton = new ClassName; } void ClassName::Finalize( ) { delete spSingleton; spSingleton = NULL; } bool ClassName::IsInitialized( ) { return spSingleton != NULL; } ClassName& ClassName::GetSingleton( )	{ Initialize(); return *spSingleton; }


#endif