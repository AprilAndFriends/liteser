/// @file
/// @author  Boris Mikic
/// @version 2.1
/// 
/// @section LICENSE
/// 
/// This program is free software; you can redistribute it and/or modify it under
/// the terms of the BSD license: http://www.opensource.org/licenses/bsd-license.php

#include <stdint.h>

#include <hltypes/harray.h>
#include <hltypes/hmap.h>
#include <hltypes/hstring.h>

#include "liteser.h"
#include "Type.h"

namespace liteser
{
	Type::Type()
	{
		this->value = NONE;
	}

	Type::~Type()
	{
		foreach (Type*, it, this->subTypes)
		{
			delete (*it);
		}
	}

	void Type::assign(VPtr<char>* arg)
	{
		this->value = INT8;
	}

	void Type::assign(VPtr<unsigned char>* arg)
	{
		this->value = UINT8;
	}

	void Type::assign(VPtr<int16_t>* arg)
	{
		this->value = INT16;
	}

	void Type::assign(VPtr<uint16_t>* arg)
	{
		this->value = UINT16;
	}

	void Type::assign(VPtr<int32_t>* arg)
	{
		this->value = INT32;
	}

	void Type::assign(VPtr<uint32_t>* arg)
	{
		this->value = UINT32;
	}

	void Type::assign(VPtr<float>* arg)
	{
		this->value = FLOAT;
	}

	void Type::assign(VPtr<double>* arg)
	{
		this->value = DOUBLE;
	}

	void Type::assign(VPtr<bool>* arg)
	{
		this->value = BOOL;
	}

	void Type::assign(VPtr<hstr>* arg)
	{
		this->value = HSTR;
	}

	void Type::assign(VPtr<grect>* arg)
	{
		this->value = GRECT;
	}

	void Type::assign(VPtr<gvec2>* arg)
	{
		this->value = GVEC2;
	}

	void Type::assign(VPtr<gvec3>* arg)
	{
		this->value = GVEC3;
	}

	void Type::assign(VPtr<Serializable>* arg)
	{
		this->value = OBJECT;
	}

	void Type::assign(VPtr<Serializable*>* arg)
	{
		this->value = OBJPTR;
	}

}