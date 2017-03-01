/// @file
/// @version 3.0
/// 
/// @section LICENSE
/// 
/// This program is free software; you can redistribute it and/or modify it under
/// the terms of the BSD license: http://opensource.org/licenses/BSD-3-Clause

#include <stdint.h>

#include <gtypes/Rectangle.h>
#include <gtypes/Vector2.h>
#include <hltypes/harray.h>
#include <hltypes/hmap.h>
#include <hltypes/hsbase.h>
#include <hltypes/hstring.h>
#include <hltypes/hversion.h>

#include "liteser.h"
#include "Serializable.h"
#include "Serialize.h"
#include "Type.h"
#include "Utility.h"
#include "Variable.h"

namespace liteser
{
	void _dumpType(Type::Identifier identifier)
	{
		stream->dump((unsigned char)identifier.value);
	}

	inline void __dumpVariable(Variable* variable)
	{
		if (variable->type->identifier == Type::Identifier::Int8)				stream->dump(*variable->value<char>());
		else if (variable->type->identifier == Type::Identifier::UInt8)			stream->dump(*variable->value<unsigned char>());
		else if (variable->type->identifier == Type::Identifier::Int16)			stream->dump(*variable->value<short>());
		else if (variable->type->identifier == Type::Identifier::UInt16)		stream->dump(*variable->value<unsigned short>());
		else if (variable->type->identifier == Type::Identifier::Int32)			stream->dump(*variable->value<int>());
		else if (variable->type->identifier == Type::Identifier::UInt32)		stream->dump(*variable->value<unsigned int>());
		else if (variable->type->identifier == Type::Identifier::Int64)			stream->dump(*variable->value<int64_t>());
		else if (variable->type->identifier == Type::Identifier::UInt64)		stream->dump(*variable->value<uint64_t>());
		else if (variable->type->identifier == Type::Identifier::Float)			stream->dump(*variable->value<float>());
		else if (variable->type->identifier == Type::Identifier::Double)		stream->dump(*variable->value<double>());
		else if (variable->type->identifier == Type::Identifier::Bool)			stream->dump(*variable->value<bool>());
		else if (variable->type->identifier == Type::Identifier::Hstr)			_dump(variable->value<hstr>());
		else if (variable->type->identifier == Type::Identifier::Hversion)		_dump(variable->value<hversion>());
		else if (variable->type->identifier == Type::Identifier::Henum)			_dump(variable->value<henum>());
		else if (variable->type->identifier == Type::Identifier::Grect)			_dump(variable->value<grect>());
		else if (variable->type->identifier == Type::Identifier::Gvec2)			_dump(variable->value<gvec2>());
		else if (variable->type->identifier == Type::Identifier::Gvec3)			_dump(variable->value<gvec3>());
		else if (variable->type->identifier == Type::Identifier::ValueObject)	_dump(variable->value<Serializable>());
		else if (variable->type->identifier == Type::Identifier::Object)		_dump(variable->value<Serializable*>());
		else if (variable->type->identifier == Type::Identifier::Harray)		__dumpContainer(variable);
		else if (variable->type->identifier == Type::Identifier::Hmap)			__dumpContainer(variable);
	}

	void __dumpContainer(Variable* variable)
	{
		stream->dump(variable->containerSize);
		if (variable->containerSize > 0)
		{
			stream->dump((unsigned int)variable->type->subTypes.size());
			foreach (Type*, it, variable->type->subTypes)
			{
				_dumpType((*it)->identifier);
			}
			foreach (Variable*, it, variable->subVariables)
			{
				__dumpVariable(*it);
			}
		}
	}

	void _dump(hstr* value)
	{
		unsigned int id = 0;
		if (!_currentHeader.stringPooling)
		{
			stream->dump(*value);
		}
		else if (__tryMapString(&id, *value))
		{
			stream->dump(id);
			stream->dump(*value);
		}
		else
		{
			stream->dump(id);
		}
	}

	void _dump(hversion* value)
	{
		stream->dump(value->major);
		stream->dump(value->minor);
		stream->dump(value->revision);
		stream->dump(value->build);
	}

	void _dump(henum* value)
	{
		stream->dump(value->value);
	}

	void _dump(grect* value)
	{
		stream->dump(value->x);
		stream->dump(value->y);
		stream->dump(value->w);
		stream->dump(value->h);
	}

	void _dump(gvec2* value)
	{
		stream->dump(value->x);
		stream->dump(value->y);
	}

	void _dump(gvec3* value)
	{
		stream->dump(value->x);
		stream->dump(value->y);
		stream->dump(value->z);
	}

	void _dump(Serializable* value)
	{
		unsigned int id = 0;
		if (!_currentHeader.allowMultiReferencing || __tryMapObject(&id, value))
		{
			if (_currentHeader.allowMultiReferencing)
			{
				stream->dump(id);
			}
			hstr name = value->_lsName();
			_dump(&name);
			harray<Variable*> variables = value->_lsVars();
			stream->dump((unsigned int)variables.size());
			foreach (Variable*, it, variables)
			{
				_dump(&(*it)->name);
				_dumpType((*it)->type->identifier);
				__dumpVariable(*it);
				delete (*it);
			}
		}
		else
		{
			stream->dump(id);
		}
	}

	void _dump(Serializable** value)
	{
		_dump(*value);
	}

	void _dumpHarray(harray<Serializable*>* value)
	{
		_dumpType(Type::Identifier::Harray);
		int size = value->size();
		stream->dump((unsigned int)size);
		if (size > 0)
		{
			stream->dump(1u);
			_dumpType(Type::Identifier::Object);
			foreach (Serializable*, it, *value)
			{
				_dump(*it);
			}
		}
	}

#define DEFINE_DUMP_HARRAY(type) \
	void _dumpHarray(harray<type>* value) \
	{ \
		_dumpType(Type::Identifier::Harray); \
		int size = value->size(); \
		stream->dump((unsigned int)size); \
		if (size > 0) \
		{ \
			stream->dump(1u); \
			Type dumpType; \
			dumpType.assign((VPtr<type>*)NULL); \
			_dumpType(dumpType.identifier); \
			foreach (type, it, *value) \
			{ \
				stream->dump(*it); \
			} \
		} \
	}

#define DEFINE_DUMP_HARRAY_C(type) \
	void _dumpHarray(harray<type>* value) \
	{ \
		_dumpType(Type::Identifier::Harray); \
		int size = value->size(); \
		stream->dump((unsigned int)size); \
		if (size > 0) \
		{ \
			stream->dump(1u); \
			Type dumpType; \
			dumpType.assign((VPtr<type>*)NULL); \
			_dumpType(dumpType.identifier); \
			stream->dump((unsigned int)value->size()); \
			foreach (type, it, *value) \
			{ \
				_dump(&(*it)); \
			} \
		} \
	}

	DEFINE_DUMP_HARRAY(char);
	DEFINE_DUMP_HARRAY(unsigned char);
	DEFINE_DUMP_HARRAY(short);
	DEFINE_DUMP_HARRAY(unsigned short);
	DEFINE_DUMP_HARRAY(int);
	DEFINE_DUMP_HARRAY(unsigned int);
	DEFINE_DUMP_HARRAY(int64_t);
	DEFINE_DUMP_HARRAY(uint64_t);
	DEFINE_DUMP_HARRAY(float);
	DEFINE_DUMP_HARRAY(double);
	DEFINE_DUMP_HARRAY_C(hstr);
	DEFINE_DUMP_HARRAY_C(hversion);
	DEFINE_DUMP_HARRAY_C(henum);
	DEFINE_DUMP_HARRAY_C(grect);
	DEFINE_DUMP_HARRAY_C(gvec2);
	DEFINE_DUMP_HARRAY_C(gvec3);

}
