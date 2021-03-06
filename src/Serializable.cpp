/// @file
/// @version 3.1
/// 
/// @section LICENSE
/// 
/// This program is free software; you can redistribute it and/or modify it under
/// the terms of the BSD license: http://opensource.org/licenses/BSD-3-Clause

#include "Serializable.h"

namespace liteser
{
	Serializable::Serializable()
	{
	}

	Serializable::~Serializable()
	{
	}
	
	Serializable* Serializable::clone()
	{
		Serializable* object = NULL;
		liteser::clone(this, &object);
		return object;
	}

}
