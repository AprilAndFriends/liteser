/// @file
/// @author  Boris Mikic
/// @version 2.0
/// 
/// @section LICENSE
/// 
/// This program is free software; you can redistribute it and/or modify it under
/// the terms of the BSD license: http://www.opensource.org/licenses/bsd-license.php
/// 
/// @section DESCRIPTION
/// 
/// Defines utility functions.

#ifndef LITESER_UTILITY_H
#define LITESER_UTILITY_H

#include <hltypes/hsbase.h>

#include "Variable.h"

namespace liteser
{
	class Serializable;

	extern hsbase* stream;

	bool __tryMapObject(unsigned int* id, Serializable* object);
	bool __tryGetObject(unsigned int id, Serializable** object);
	void _start(hsbase* stream);
	void _finish(hsbase* stream);
	bool _isActive();
	void _checkVersion(unsigned char major, unsigned char minor);

}
#endif
