#include "plugin_static_initializer.hpp"

namespace wgt
{

//==========================================================================
struct PluginStaticInitializer::InitNode
{
	InitNode(
		const std::shared_ptr< ExecFunc > & execFunc,
		const std::shared_ptr< FiniFunc > & finiFunc)
		: executed_(false)
		, execFunc_(execFunc)
		, finiFunc_(finiFunc)
	{
	}

	~InitNode()
	{
		auto finiFunc = finiFunc_.lock();
		if (finiFunc &&
			*finiFunc)
		{
			(*finiFunc)();
		}
	}

	std::unique_ptr<InitNode> next_;
	bool executed_;
	std::weak_ptr< ExecFunc > execFunc_;
	std::weak_ptr< FiniFunc > finiFunc_;
};


//------------------------------------------------------------------------------
PluginStaticInitializer::PluginStaticInitializer() : tailInitNode_(nullptr)
{
}


//------------------------------------------------------------------------------
PluginStaticInitializer::~PluginStaticInitializer()
{
}


//------------------------------------------------------------------------------
void PluginStaticInitializer::registerStaticInitializer(
	const std::shared_ptr< ExecFunc > & execFunc,
	const std::shared_ptr< FiniFunc > & finiFunc)
{
	std::unique_ptr<InitNode> initNode(new InitNode(execFunc, finiFunc));
	if (headInitNode_ == nullptr)
	{
		headInitNode_ = std::move(initNode);
		tailInitNode_ = headInitNode_.get();
		return;
	}
	tailInitNode_->next_ = std::move(initNode);
	tailInitNode_ = tailInitNode_->next_.get();
}


//------------------------------------------------------------------------------
void PluginStaticInitializer::initStatics(IComponentContext& context)
{
	auto node = headInitNode_.get();
	while (node)
	{
		if (node->executed_ == false)
		{
			node->executed_ = true;
			auto execFunc = node->execFunc_.lock();
			if (execFunc &&
				*execFunc)
			{
				(*execFunc)(context);
			}
		}
		node = node->next_.get();
	}
}


//------------------------------------------------------------------------------
void PluginStaticInitializer::destroy()
{
	headInitNode_.reset();
}

}