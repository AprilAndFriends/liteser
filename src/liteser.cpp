/// @file
/// @version 2.5
/// 
/// @section LICENSE
/// 
/// This program is free software; you can redistribute it and/or modify it under
/// the terms of the BSD license: http://opensource.org/licenses/BSD-3-Clause

#include <hltypes/hexception.h>
#include <hltypes/hmap.h>
#include <hltypes/hlog.h>
#include <hltypes/hsbase.h>
#include <hltypes/hstream.h>
#include <hltypes/hstring.h>
#include <hltypes/hversion.h>
#include <hlxml/Document.h>
#include <hlxml/Node.h>
#include <hlxml/Property.h>

#include "Deserialize.h"
#include "DeserializeXml.h"
#include "liteser.h"
#include "Serializable.h"
#include "Serialize.h"
#include "SerializeXml.h"
#include "Utility.h"
#include "Variable.h"

#define _LS_HEADER_0 'L'
#define _LS_HEADER_1 'S'

#define HEADER_SIZE 4

#define XML_HEADER "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
#define LITESER_XML_ROOT_BEGIN hsprintf("<Liteser version=\"%d.%d\">\n\t", _LS_VERSION_MAJOR, _LS_VERSION_MINOR)
#define LITESER_XML_ROOT_END "\t\n</Liteser>"

#define DECLARE_HARRAY_SERIALIZER(type) \
	bool serialize(hsbase* stream, harray<type>& value) \
	{ \
		if (!stream->isOpen()) \
		{ \
			throw FileNotOpenException("Liteser Stream"); \
		} \
		_start(stream); \
		stream->writeRaw(header, HEADER_SIZE); \
		_dumpHarray(&value); \
		_finish(stream); \
		return true; \
	}
	
#define DECLARE_HARRAY_DESERIALIZER(type) \
	bool deserialize(hsbase* stream, harray<type>* value) \
	{ \
		if (!stream->isOpen()) \
		{ \
			throw FileNotOpenException("Liteser Stream"); \
		} \
		if (value->size() > 0) \
		{ \
			throw Exception("Output harray is not empty!"); \
		} \
		_start(stream); \
		unsigned char readHeader[HEADER_SIZE]; \
		stream->readRaw(readHeader, HEADER_SIZE); \
		if (readHeader[0] != _LS_HEADER_0 || readHeader[1] != _LS_HEADER_1) \
		{ \
			throw Exception("Invalid header!"); \
		} \
		unsigned char major = readHeader[2]; \
		unsigned char minor = readHeader[3]; \
		_checkVersion(major, minor); \
		unsigned int size = 0; \
		if (major > 2 || major == 2 && minor >= 7) \
		{ \
			if (_loadType() != Type::HARRAY) \
			{ \
				_finish(stream); \
				throw Exception("Cannot load object from file that does not contain a harray<" #type ">!"); \
			} \
			size = stream->loadUint32(); \
			if (size > 0) \
			{ \
				if (stream->loadUint32() != 1) \
				{ \
					_finish(stream); \
					throw Exception("Cannot load object from file that does not contain a harray<" #type ">!"); \
				} \
				Type subType; \
				subType.assign((VPtr<type>*)NULL); \
				if (_loadType() != subType.value) \
				{ \
					_finish(stream); \
					throw Exception("Cannot load object from file that does not contain a harray<" #type ">!"); \
				} \
			} \
		} \
		else \
		{ \
			size = stream->loadUint32(); \
		} \
		_loadHarray(value, size); \
		_finish(stream); \
		return true; \
	}

#define DECLARE_HARRAY_SERIALIZER_XML(type) \
	bool serialize(hsbase* stream, harray<type>& value) \
	{ \
		if (!stream->isOpen()) \
		{ \
			throw FileNotOpenException("Liteser XML Stream"); \
		} \
		_start(stream); \
		stream->writeLine(XML_HEADER); \
		stream->writeLine(LITESER_XML_ROOT_BEGIN); \
		Type subType; \
		subType.assign((VPtr<type>*)NULL); \
		stream->writeLine("\t<Container type=\"" + hsprintf("%02X", Type::HARRAY) + "\" sub_types=\"" + hsprintf("%02X", subType.value) + "\">"); \
		_indent += "\t"; \
		xml::_dumpHarray(&value); \
		_indent = _indent(0, _indent.size() - 1); \
		stream->writeLine("\t</Container>"); \
		stream->writeLine(LITESER_XML_ROOT_END); \
		_finish(stream); \
		return true; \
	}
	
#define DECLARE_HARRAY_DESERIALIZER_XML(type) \
	bool deserialize(hsbase* stream, harray<type>* value) \
	{ \
		if (!stream->isOpen()) \
		{ \
			throw FileNotOpenException("Liteser Stream"); \
		} \
		if (value->size() > 0) \
		{ \
			throw Exception("Output harray is not empty!"); \
		} \
		_start(stream); \
		hlxml::Document doc(*stream); \
		hlxml::Node* root = doc.root(); \
		if (*root != "Liteser") \
		{ \
			throw Exception("Invalid header!"); \
		} \
		hstr versionString = root->pstr("version", ""); \
		if (versionString.count(".") != 1) \
		{ \
			throw Exception("Invalid header!"); \
		} \
		hstr majorString; \
		hstr minorString; \
		if (!versionString.split('.', majorString, minorString)) \
		{ \
			throw Exception("Invalid header!"); \
		} \
		unsigned char major = (unsigned char)(int)majorString; \
		unsigned char minor = (unsigned char)(int)minorString; \
		_checkVersion(major, minor); \
		hlxml::Node* node = root; \
		if (major > 2 || major == 2 && minor >= 7) \
		{ \
			node = root->iterChildren(); \
			if (node->next() != NULL || *node != "Container" || node->pstr("type", "00").unhex() != Type::HARRAY) \
			{ \
				_finish(stream); \
				throw Exception("Cannot load object from file that does not contain a harray<\"" #type "\">!"); \
			} \
			Type subType; \
			subType.assign((VPtr<type>*)NULL); \
			harray<hstr> subTypes = node->pstr("sub_types", "00").split(',', -1, true); \
			if (subTypes.size() != 1 || subTypes.first().unhex() != subType.value) \
			{ \
				_finish(stream); \
				throw Exception("Cannot load object from file that does not contain a harray<" #type ">!"); \
			} \
		} \
		xml::_loadHarray(node, value); \
		_finish(stream); \
		return true; \
	}

namespace liteser
{
	char header[4] = {_LS_HEADER_0, _LS_HEADER_1, (char)_LS_VERSION_MAJOR, (char)_LS_VERSION_MINOR};

	hstr logTag = "liteser";

	bool serialize(hsbase* stream, Serializable* object)
	{
		if (!stream->isOpen())
		{
			throw FileNotOpenException("Liteser Stream");
		}
		// TODO - add exception handling
		_start(stream);
		stream->writeRaw(header, HEADER_SIZE);
		_dumpType(Type::OBJPTR);
		_dump(&object);
		_finish(stream);
		return true;
	}

	DECLARE_HARRAY_SERIALIZER(Serializable*);
	DECLARE_HARRAY_SERIALIZER(char);
	DECLARE_HARRAY_SERIALIZER(unsigned char);
	DECLARE_HARRAY_SERIALIZER(short);
	DECLARE_HARRAY_SERIALIZER(unsigned short);
	DECLARE_HARRAY_SERIALIZER(int);
	DECLARE_HARRAY_SERIALIZER(unsigned int);
	DECLARE_HARRAY_SERIALIZER(int64_t);
	DECLARE_HARRAY_SERIALIZER(uint64_t);
	DECLARE_HARRAY_SERIALIZER(float);
	DECLARE_HARRAY_SERIALIZER(double);
	DECLARE_HARRAY_SERIALIZER(hstr);
	DECLARE_HARRAY_SERIALIZER(hversion);
	DECLARE_HARRAY_SERIALIZER(henum);
	DECLARE_HARRAY_SERIALIZER(grect);
	DECLARE_HARRAY_SERIALIZER(gvec2);
	DECLARE_HARRAY_SERIALIZER(gvec3);
	
	bool deserialize(hsbase* stream, Serializable** object)
	{
		if (!stream->isOpen())
		{
			throw FileNotOpenException("Liteser Stream");
		}
		if (*object != NULL)
		{
			throw Exception("Given pointer to object for deserialization is not NULL.");
		}
		// TODO - add exception handling
		_start(stream);
		unsigned char readHeader[HEADER_SIZE];
		stream->readRaw(readHeader, HEADER_SIZE);
		if (readHeader[0] != _LS_HEADER_0 || readHeader[1] != _LS_HEADER_1)
		{
			throw Exception("Invalid header!");
		}
		unsigned char major = readHeader[2];
		unsigned char minor = readHeader[3];
		_checkVersion(major, minor);
		if (major > 2 || (major == 2 && minor >= 7))
		{
			Type::Value type = _loadType();
			if (type != Type::OBJPTR)
			{
				_finish(stream);
				throw Exception("Cannot load object from file that does not contain an object!");
			}
		}
		_load(object);
		_finish(stream);
		return true;
	}

	DECLARE_HARRAY_DESERIALIZER(Serializable*);
	DECLARE_HARRAY_DESERIALIZER(char);
	DECLARE_HARRAY_DESERIALIZER(unsigned char);
	DECLARE_HARRAY_DESERIALIZER(short);
	DECLARE_HARRAY_DESERIALIZER(unsigned short);
	DECLARE_HARRAY_DESERIALIZER(int);
	DECLARE_HARRAY_DESERIALIZER(unsigned int);
	DECLARE_HARRAY_DESERIALIZER(int64_t);
	DECLARE_HARRAY_DESERIALIZER(uint64_t);
	DECLARE_HARRAY_DESERIALIZER(float);
	DECLARE_HARRAY_DESERIALIZER(double);
	DECLARE_HARRAY_DESERIALIZER(hstr);
	DECLARE_HARRAY_DESERIALIZER(hversion);
	DECLARE_HARRAY_DESERIALIZER(henum);
	DECLARE_HARRAY_DESERIALIZER(grect);
	DECLARE_HARRAY_DESERIALIZER(gvec2);
	DECLARE_HARRAY_DESERIALIZER(gvec3);

	namespace xml
	{
		bool serialize(hsbase* stream, Serializable* object)
		{
			if (!stream->isOpen())
			{
				throw FileNotOpenException("Liteser Stream");
			}
			// TODO - add exception handling
			_start(stream);
			stream->writeLine(XML_HEADER);
			stream->writeLine(LITESER_XML_ROOT_BEGIN);
			xml::_dump(&object);
			stream->writeLine(LITESER_XML_ROOT_END);
			_finish(stream);
			return true;
		}

		DECLARE_HARRAY_SERIALIZER_XML(Serializable*);
		DECLARE_HARRAY_SERIALIZER_XML(char);
		DECLARE_HARRAY_SERIALIZER_XML(unsigned char);
		DECLARE_HARRAY_SERIALIZER_XML(short);
		DECLARE_HARRAY_SERIALIZER_XML(unsigned short);
		DECLARE_HARRAY_SERIALIZER_XML(int);
		DECLARE_HARRAY_SERIALIZER_XML(unsigned int);
		DECLARE_HARRAY_SERIALIZER_XML(int64_t);
		DECLARE_HARRAY_SERIALIZER_XML(uint64_t);
		DECLARE_HARRAY_SERIALIZER_XML(float);
		DECLARE_HARRAY_SERIALIZER_XML(double);
		DECLARE_HARRAY_SERIALIZER_XML(hstr);
		DECLARE_HARRAY_SERIALIZER_XML(hversion);
		DECLARE_HARRAY_SERIALIZER_XML(henum);
		DECLARE_HARRAY_SERIALIZER_XML(grect);
		DECLARE_HARRAY_SERIALIZER_XML(gvec2);
		DECLARE_HARRAY_SERIALIZER_XML(gvec3);

		bool deserialize(hsbase* stream, Serializable** object)
		{
			if (!stream->isOpen())
			{
				throw FileNotOpenException("Liteser Stream");
			}
			if (*object != NULL)
			{
				throw Exception("Given pointer to object for deserialization is not NULL.");
			}
			// TODO - add exception handling
			_start(stream);
			hlxml::Document doc(*stream);
			hlxml::Node* root = doc.root();
			if (*root != "Liteser")
			{
				throw Exception("Invalid header!");
			}
			hstr versionString = root->pstr("version", "");
			if (versionString.count(".") != 1)
			{
				throw Exception("Invalid header!");
			}
			hstr majorString;
			hstr minorString;
			if (!versionString.split('.', majorString, minorString))
			{
				throw Exception("Invalid header!");
			}
			unsigned char major = (unsigned char)(int)majorString;
			unsigned char minor = (unsigned char)(int)minorString;
			_checkVersion(major, minor);
			hlxml::Node* node = root->iterChildren();
			if (major > 2 || (major == 2 && minor >= 7))
			{
				if (node->next() != NULL || *node != "Object")
				{
					_finish(stream);
					throw Exception("Cannot load object from file that does not contain an object!");
				}
			}
			xml::_load(node, object);
			_finish(stream);
			return true;
		}

		DECLARE_HARRAY_DESERIALIZER_XML(Serializable*);
		DECLARE_HARRAY_DESERIALIZER_XML(char);
		DECLARE_HARRAY_DESERIALIZER_XML(unsigned char);
		DECLARE_HARRAY_DESERIALIZER_XML(short);
		DECLARE_HARRAY_DESERIALIZER_XML(unsigned short);
		DECLARE_HARRAY_DESERIALIZER_XML(int);
		DECLARE_HARRAY_DESERIALIZER_XML(unsigned int);
		DECLARE_HARRAY_DESERIALIZER_XML(int64_t);
		DECLARE_HARRAY_DESERIALIZER_XML(uint64_t);
		DECLARE_HARRAY_DESERIALIZER_XML(float);
		DECLARE_HARRAY_DESERIALIZER_XML(double);
		DECLARE_HARRAY_DESERIALIZER_XML(hstr);
		DECLARE_HARRAY_DESERIALIZER_XML(hversion);
		DECLARE_HARRAY_DESERIALIZER_XML(henum);
		DECLARE_HARRAY_DESERIALIZER_XML(grect);
		DECLARE_HARRAY_DESERIALIZER_XML(gvec2);
		DECLARE_HARRAY_DESERIALIZER_XML(gvec3);

	}

	bool clone(Serializable* input, Serializable** output)
	{
		if (*output != NULL)
		{
			throw Exception("Output does not point to NULL!");
		}
		hstream stream;
		_start(&stream);
		_dump(&input);
		_finish(&stream);
		stream.rewind();
		_start(&stream);
		_load(output);
		_finish(&stream);
		return true;
	}

}
