//-----------------------------------------------------------------------------
//
//  test_tool.hpp
//
//-----------------------------------------------------------------------------
//  Copyright (c) 2017-2017 Wargaming World, Ltd. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <core_dependency_system/i_interface.hpp>
#include <toolbox/i_tool.hpp>

#include "core_reflection/reflection_macros.hpp"
#include "core_reflection/utilities/reflection_function_utilities.hpp"
#include "core_reflection/metadata/meta_types.hpp"
#include "core_reflection/i_definition_manager.hpp"

namespace wgt
{
class BaseMode
{
	DECLARE_REFLECTED
public:
	~BaseMode()
	{
	}

private:
	bool visible_;
};

BEGIN_EXPOSE(BaseMode, MetaDisplayName(L"Base Mode"))
EXPOSE("Visible", visible_, MetaNone())
END_EXPOSE()

class TextMode : public BaseMode
{
	DECLARE_REFLECTED
private:
	std::string text_;
};

BEGIN_EXPOSE(TextMode, BaseMode, MetaDisplayName(L"Text Mode"))
EXPOSE("Text", text_, MetaNone())
END_EXPOSE()

enum class ComboModeEnum
{
	Value1,
	Value2,
	Value3
};

class ComboBoxMode : public BaseMode
{
	DECLARE_REFLECTED
private:
	int curSelected_;
	ComboModeEnum curEnum_;
};

BEGIN_EXPOSE_ENUM(ComboModeEnum)
{ComboModeEnum::Value1, "Value 1"},
{ComboModeEnum::Value2, "Value 2"},
{ComboModeEnum::Value3, "Value 3"}
END_EXPOSE()

BEGIN_EXPOSE(ComboBoxMode, BaseMode, MetaDisplayName(L"ComboBox Mode"))
EXPOSE("Selected Index", curSelected_, MetaEnum(L"Select0=0|Select1|Select2"))
EXPOSE("Enum", curEnum_, MetaEnum<ComboModeEnum>())
END_EXPOSE()

class ColorMode : public BaseMode
{
	DECLARE_REFLECTED
private:
	Vector4 color_;
};

BEGIN_EXPOSE(ColorMode, BaseMode,
             MetaDisplayName(L"Color Mode") + MetaIcon("qrc:/plg_toolbox_test/icons/color_mode.png"))
EXPOSE("Color", color_, MetaColor())
END_EXPOSE()

class DummyTool : public Implements<ITool>
{
	DECLARE_REFLECTED
public:
	bool canActivate() const override
	{
		return true;
	}
	void deactivate() override
	{
	}
	void activate() override
	{
	}
	void update() override
	{
	}
	void draw() override
	{
	}

	uint64_t id() const override
	{ 
		static TypeId s_typeId = TypeId::getType<DummyTool>();
		return s_typeId.getHashcode();
	}

private:
	int dummyVar = 2;
	ObjectHandleT<BaseMode> polyStruct_;
};

BEGIN_EXPOSE(DummyTool, ITool, MetaDisplayName(L"Dummy tool"))
EXPOSE("DummyVar", dummyVar, MetaNone())
EXPOSE("Sub Modes", polyStruct_, MetaNoNull())
END_EXPOSE()

class NovaTool : public Implements<ITool>
{
	DECLARE_REFLECTED
public:
	bool canActivate() const override
	{
		return true;
	}
	void deactivate() override
	{
	}
	void activate() override
	{
	}
	void update() override
	{
	}
	void draw() override
	{
	}

	uint64_t id() const override
	{
		static TypeId s_typeId = TypeId::getType<NovaTool>();
		return s_typeId.getHashcode();
	}

private:
	int novaVar = 11;
	ObjectHandleT<BaseMode> polyStruct_;
};

BEGIN_EXPOSE(NovaTool, ITool, MetaIcon("qrc:/plg_toolbox_test/icons/tool.png"))
EXPOSE("NovaVar", novaVar, MetaNone())
EXPOSE("Sub Modes", polyStruct_, MetaNoNull())
END_EXPOSE()

} // end namespace wgt
