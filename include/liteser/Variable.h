/// @file
/// @version 2.2
/// 
/// @section LICENSE
/// 
/// This program is free software; you can redistribute it and/or modify it under
/// the terms of the BSD license: http://opensource.org/licenses/BSD-3-Clause
/// 
/// @section DESCRIPTION
/// 
/// Represents data about a variable definition and value.

#ifndef LITESER_VARIABLE_H
#define LITESER_VARIABLE_H

#include <stdint.h>

#include <gtypes/Rectangle.h>
#include <gtypes/Vector2.h>
#include <hltypes/exception.h>
#include <hltypes/harray.h>
#include <hltypes/hmap.h>
#include <hltypes/hstring.h>

#include "liteser.h"
#include "liteserExport.h"
#include "Ptr.h"
#include "Type.h"

namespace liteser
{
	class Serializable;

	class liteserExport Variable
	{
	public:
		hstr name;
		Type* type;
		Ptr* ptr;
		harray<Variable*> subVariables;
		unsigned int containerSize;

		Variable(chstr name = "");
		~Variable();

		Variable* assign(VPtr<char>* ptr); // char is always 8 bits
		Variable* assign(VPtr<unsigned char>* ptr); // unsigned char is always 8 bits
		Variable* assign(VPtr<int16_t>* ptr);
		Variable* assign(VPtr<uint16_t>* ptr);
		Variable* assign(VPtr<int32_t>* ptr);
		Variable* assign(VPtr<uint32_t>* ptr);
		Variable* assign(VPtr<float>* ptr);
		Variable* assign(VPtr<double>* ptr);
		Variable* assign(VPtr<bool>* ptr);
		Variable* assign(VPtr<hstr>* ptr);
		Variable* assign(VPtr<grect>* ptr);
		Variable* assign(VPtr<gvec2>* ptr);
		Variable* assign(VPtr<gvec3>* ptr);
		Variable* assign(VPtr<harray<char> >* ptr); // char is always 8 bits
		Variable* assign(VPtr<harray<unsigned char> >* ptr); // unsigned char is always 8 bits
		Variable* assign(VPtr<harray<int16_t> >* ptr);
		Variable* assign(VPtr<harray<uint16_t> >* ptr);
		Variable* assign(VPtr<harray<int32_t> >* ptr);
		Variable* assign(VPtr<harray<uint32_t> >* ptr);
		Variable* assign(VPtr<harray<float> >* ptr);
		Variable* assign(VPtr<harray<double> >* ptr);
		Variable* assign(VPtr<harray<hstr> >* ptr);
		Variable* assign(VPtr<harray<grect> >* ptr);
		Variable* assign(VPtr<harray<gvec2> >* ptr);
		Variable* assign(VPtr<harray<gvec3> >* ptr);
		template <class T>
		inline Variable* assign(VPtr<T*>* ptr)
		{
			this->type->assign((VPtr<Serializable*>*)NULL);
			this->ptr = ptr;
			// IMPORTANT NOTE: If you get C2440 on the line below, it means that the class does not inherit liteser::Serializable
			// or there is a forward declaration of the class instead of including the actual header.
			VPtr<Serializable>(*ptr->value);
			return this;
		}
		template <class T>
		inline Variable* assign(VPtr<T>* ptr)
		{
			this->type->assign((VPtr<Serializable>*)NULL);
			this->ptr = ptr;
			// IMPORTANT NOTE: If you get C2440 on the line below, it means that the class does not inherit liteser::Serializable
			// or there is a forward declaration of the class instead of including the actual header.
			VPtr<Serializable>(ptr->value);
			return this;
		}
		template <class T>
		inline Variable* assign(VPtr<harray<T*> >* ptr)
		{
			this->type->assign((VPtr<harray<Serializable*> >*)NULL);
			this->ptr = ptr;
			this->containerSize = ptr->value->size();
			harray<Serializable*>* obj = (harray<Serializable*>*)ptr->value;
			foreach (Serializable*, it, *obj)
			{
				this->subVariables += (new Variable())->assign(new VPtr<Serializable*>(&(*it)));
			}
			return this;
		}
		template <class K, class V>
		inline Variable* assign(VPtr<hmap<K*, V> >* ptr)
		{
			this->type->assign((VPtr<harray<Serializable*> >*)NULL);
			this->ptr = ptr;
			this->containerSize = ptr->value->size();
			harray<K*> originalKeys = ptr->value->keys();
			harray<Serializable*>* keys = new harray<Serializable*>(originalKeys.template cast<Serializable*>());
			harray<V>* values = new harray<V>();
			for_iter (i, 0, originalKeys.size()) // cannot use foreach here because GCC can't compile it properly
			{
				values->add(ptr->value->operator[](originalKeys[i]));
			}
			this->ptrKeys = new DPtr<Serializable*>(keys);
			this->ptrValues = new DPtr<V>(values);
			this->subVariables += (new Variable())->assign(new VPtr<harray<Serializable*> >(keys));
			this->subVariables += (new Variable())->assign(new VPtr<harray<V> >(values));
			return this;
		}
		template <class K, class V>
		inline Variable* assign(VPtr<hmap<K, V> >* ptr)
		{
			this->type->assign(ptr);
			this->ptr = ptr;
			this->containerSize = ptr->value->size();
			harray<K>* keys = new harray<K>(ptr->value->keys());
			harray<V>* values = new harray<V>();
			for_iter (i, 0, keys->size()) // cannot use foreach here because GCC can't compile it properly
			{
				values->add(ptr->value->operator[](keys->operator[](i)));
			}
			this->ptrKeys = new DPtr<K>(keys);
			this->ptrValues = new DPtr<V>(values);
			this->subVariables += (new Variable())->assign(new VPtr<harray<K> >(keys));
			this->subVariables += (new Variable())->assign(new VPtr<harray<V> >(values));
			return this;
		}

		template <class T>
		inline T* value()
		{
			return ((VPtr<T>*)this->ptr)->value;
		}

		void createSubVariables(Type::Value type);
		void applyHmapSubVariables(Type::Value type);

	protected:
		Ptr* ptrKeys;
		Ptr* ptrValues;

		template <class T>
		inline void _addSubVariablesHarray()
		{
			harray<T>* container = ((VPtr<harray<T> >*)this->ptr)->value;
			if (container->size() > 0)
			{
				throw hl_exception("Harray in default constructor not empty initially: " + this->name);
			}
			container->add(T(), this->containerSize); // requires adding first because of possible reallocation of memory to another block
			for_itert (unsigned int, i, 0, this->containerSize)
			{
				this->subVariables += (new Variable())->assign(new VPtr<T>(&container->operator[](i)));
			}
		}
		template <class key>
		inline void _addSubVariablesHmapKey(Type::Value value)
		{
			switch (value)
			{
			case Type::INT8:	this->_addSubVariablesHmap<key, char>();			break;
			case Type::UINT8:	this->_addSubVariablesHmap<key, unsigned char>();	break;
			case Type::INT16:	this->_addSubVariablesHmap<key, int16_t>();			break;
			case Type::UINT16:	this->_addSubVariablesHmap<key, uint16_t>();		break;
			case Type::INT32:	this->_addSubVariablesHmap<key, int32_t>();			break;
			case Type::UINT32:	this->_addSubVariablesHmap<key, uint32_t>();		break;
			case Type::FLOAT:	this->_addSubVariablesHmap<key, float>();			break;
			case Type::DOUBLE:	this->_addSubVariablesHmap<key, double>();			break;
			case Type::HSTR:	this->_addSubVariablesHmap<key, hstr>();			break;
			case Type::GRECT:	this->_addSubVariablesHmap<key, grect>();			break;
			case Type::GVEC2:	this->_addSubVariablesHmap<key, gvec2>();			break;
			case Type::GVEC3:	this->_addSubVariablesHmap<key, gvec3>();			break;
			case Type::OBJPTR:	this->_addSubVariablesHmap<key, Serializable*>();	break;
			}
		}
		template <class K, class V>
		inline void _addSubVariablesHmap()
		{
			if (((VPtr<hmap<K, V> >*)this->ptr)->value->size() > 0)
			{
				throw hl_exception("Hmap in default constructor not empty initially: " + this->name);
			}
		}
		template <class keyType>
		inline void _applyHmapSubVariablesKey(Type::Value value)
		{
			switch (value)
			{
			case Type::INT8:	this->_applyHmapSubVariables<keyType, char>();			break;
			case Type::UINT8:	this->_applyHmapSubVariables<keyType, unsigned char>();	break;
			case Type::INT16:	this->_applyHmapSubVariables<keyType, int16_t>();		break;
			case Type::UINT16:	this->_applyHmapSubVariables<keyType, uint16_t>();		break;
			case Type::INT32:	this->_applyHmapSubVariables<keyType, int32_t>();		break;
			case Type::UINT32:	this->_applyHmapSubVariables<keyType, uint32_t>();		break;
			case Type::FLOAT:	this->_applyHmapSubVariables<keyType, float>();			break;
			case Type::DOUBLE:	this->_applyHmapSubVariables<keyType, double>();		break;
			case Type::HSTR:	this->_applyHmapSubVariables<keyType, hstr>();			break;
			case Type::GRECT:	this->_applyHmapSubVariables<keyType, grect>();			break;
			case Type::GVEC2:	this->_applyHmapSubVariables<keyType, gvec2>();			break;
			case Type::GVEC3:	this->_applyHmapSubVariables<keyType, gvec3>();			break;
			case Type::OBJPTR:	this->_applyHmapSubVariables<keyType, Serializable*>();	break;
			}
		}
		template <class K, class V>
		inline void _applyHmapSubVariables()
		{
			hmap<K, V>* container = ((VPtr<hmap<K, V> >*)this->ptr)->value;
			harray<K>* keys = ((DPtr<K>*)this->ptrKeys)->data;
			harray<V>* values = ((DPtr<V>*)this->ptrValues)->data;
			for_iter (i, 0, keys->size())
			{
				container->operator[](keys->operator[](i)) = values->operator[](i);
			}
		}

	};

}
#endif
