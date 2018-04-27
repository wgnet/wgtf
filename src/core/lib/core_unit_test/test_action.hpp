#pragma once

#include "core_ui_framework/i_action.hpp"
#include "core_variant/variant.hpp"

namespace wgt
{
class TestAction : public IAction
{
public:
	virtual void text(const char*) override {}
	virtual const char* id() const override { return ""; }
	virtual const char* text() const override { return ""; }
	virtual const char* icon() const override { return ""; }
	virtual const char* windowId() const override { return ""; }
	virtual const std::vector<std::string>& paths() const override { return paths_; }
	virtual const char* shortcut() const override { return ""; }
	virtual const char* group() const override { return ""; }
	virtual void setShortcut(const char* shortcut) override {}
	virtual int order() const override { return 0;  }
	virtual bool isSeparator() const override { return false; }
	virtual bool enabled() const override { return true; }
	virtual bool checked() const override { return true; }
	virtual bool isCheckable() const override { return true; }
	virtual void visible(bool) {}
	virtual bool visible() const { return true; }
	virtual void execute() override { executed_ = true; }
	virtual void setData(const Variant& data) override { data_ = data; }
	virtual Variant& getData() override { return data_; }
	virtual const Variant& getData() const override { return data_; }
	bool hasExecuted() const { return executed_; }

private:
	Variant data_;
	std::vector<std::string> paths_;
	bool executed_ = false;
};
}