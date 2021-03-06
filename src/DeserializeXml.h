/// @file
/// @version 3.1
/// 
/// @section LICENSE
/// 
/// This program is free software; you can redistribute it and/or modify it under
/// the terms of the BSD license: http://opensource.org/licenses/BSD-3-Clause
/// 
/// @section DESCRIPTION
/// 
/// Defines all deserialization functions.

#ifndef LITESER_DESERIALIZE_XML_H
#define LITESER_DESERIALIZE_XML_H

#include <stdint.h>

#include <gtypes/Rectangle.h>
#include <gtypes/Vector2.h>
#include <gtypes/Vector3.h>
#include <hltypes/harray.h>
#include <hltypes/henum.h>
#include <hltypes/hmap.h>
#include <hltypes/hsbase.h>
#include <hltypes/hstring.h>
#include <hltypes/hversion.h>

#include "Type.h"

namespace hlxml
{
	class Node;
}

namespace liteser
{
	class Serializable;
	class Variable;

	namespace xml
	{
		void __loadContainer(hlxml::Node* node, Variable* variable, Type::Identifier loadedIdentifier);
		bool __skipContainer(hlxml::Node* node, Type::Identifier loadedIdentifier);

		void _load(hlxml::Node* node, hstr* value);
		void _load(hlxml::Node* node, hversion* value);
		void _load(hlxml::Node* node, henum* value);
		void _load(hlxml::Node* node, grectf* value);
		void _load(hlxml::Node* node, gvec2f* value);
		void _load(hlxml::Node* node, gvec3f* value);
		void _load(hlxml::Node* node, grecti* value);
		void _load(hlxml::Node* node, gvec2i* value);
		void _load(hlxml::Node* node, gvec3i* value);
		void _load(hlxml::Node* node, grectd* value);
		void _load(hlxml::Node* node, gvec2d* value);
		void _load(hlxml::Node* node, gvec3d* value);
		void _load(hlxml::Node* node, Serializable* value);
		void _load(hlxml::Node* node, Serializable** value);
		void __loadObject(hlxml::Node* node, Serializable** value);
		bool __skipObject(hlxml::Node* node);

		void _loadHarray(hlxml::Node* node, harray<char>* value);
		void _loadHarray(hlxml::Node* node, harray<unsigned char>* value);
		void _loadHarray(hlxml::Node* node, harray<short>* value);
		void _loadHarray(hlxml::Node* node, harray<unsigned short>* value);
		void _loadHarray(hlxml::Node* node, harray<int>* value);
		void _loadHarray(hlxml::Node* node, harray<unsigned int>* value);
		void _loadHarray(hlxml::Node* node, harray<int64_t>* value);
		void _loadHarray(hlxml::Node* node, harray<uint64_t>* value);
		void _loadHarray(hlxml::Node* node, harray<float>* value);
		void _loadHarray(hlxml::Node* node, harray<double>* value);
		void _loadHarray(hlxml::Node* node, harray<hstr>* value);
		void _loadHarray(hlxml::Node* node, harray<hversion>* value);
		void _loadHarray(hlxml::Node* node, harray<henum>* value);
		void _loadHarray(hlxml::Node* node, harray<grectf>* value);
		void _loadHarray(hlxml::Node* node, harray<gvec2f>* value);
		void _loadHarray(hlxml::Node* node, harray<gvec3f>* value);
		void _loadHarray(hlxml::Node* node, harray<grecti>* value);
		void _loadHarray(hlxml::Node* node, harray<gvec2i>* value);
		void _loadHarray(hlxml::Node* node, harray<gvec3i>* value);
		void _loadHarray(hlxml::Node* node, harray<grectd>* value);
		void _loadHarray(hlxml::Node* node, harray<gvec2d>* value);
		void _loadHarray(hlxml::Node* node, harray<gvec3d>* value);
		void _loadHarray(hlxml::Node* node, harray<Serializable*>* value);

	}

}
#endif
