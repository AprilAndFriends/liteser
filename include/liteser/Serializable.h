/// @file
/// @author  Boris Mikic
/// @version 2.1
/// 
/// @section LICENSE
/// 
/// This program is free software; you can redistribute it and/or modify it under
/// the terms of the BSD license: http://www.opensource.org/licenses/bsd-license.php
/// 
/// @section DESCRIPTION
/// 
/// Represents a base for a serializable object.

#ifndef LITESER_SERIALIZABLE_H
#define LITESER_SERIALIZABLE_H

#include <stdint.h>

#include <hltypes/hsbase.h>

#include "Factory.h"
#include "liteserExport.h"
#include "macros.h"
#include "Variable.h"

#define LS_CLASS_DECLARE(classe) \
	LS_CLASS_DECLARE_ABSTRACT(classe); \
	static liteser::Factory::Register<classe> _lsRegister;
#define LS_CLASS_DEFINE(classe) \
	liteser::Factory::Register<classe> classe::_lsRegister(#classe);
/// @brief Use this instead of LS_CLASS_DECLARE if class contains pure virtual methods. In this case LS_CLASS_DEFINE is not needed.
#define LS_CLASS_DECLARE_ABSTRACT(classe) \
	hstr _lsName() { return #classe; }
#define LS_VARS(superclass, ...) \
	__LS_FOREACH(__LS_VAR, (__VA_ARGS__)) \
	harray<liteser::Variable*> _lsVars() \
	{ \
		harray<liteser::Variable*> variables = superclass::_lsVars(); \
		static liteser::Variable* variable; \
		__LS_FOREACH(__LS_REF, (__VA_ARGS__)) \
		return variables; \
	}
#define LS_VARS_DECLARE(...) \
	__LS_FOREACH(__LS_VAR, (__VA_ARGS__)) \
	harray<liteser::Variable*> _lsVars();
#define LS_VARS_DEFINE(classe, superclass, ...) \
	harray<liteser::Variable*> classe::_lsVars() \
	{ \
		harray<liteser::Variable*> variables = superclass::_lsVars(); \
		static liteser::Variable* variable; \
		__LS_FOREACH(__LS_REF, (__VA_ARGS__)) \
		return variables; \
	}

namespace liteser
{
	class liteserExport Serializable
	{
	public:
		Serializable();
		virtual ~Serializable();

		virtual hstr _lsName() { return "liteser::Serializer"; }
		virtual harray<Variable*> _lsVars() { return harray<Variable*>(); }

	};

}
#endif
