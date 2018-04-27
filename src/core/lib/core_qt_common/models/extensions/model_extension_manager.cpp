#include "model_extension_manager.hpp"

#include "core_common/assert.hpp"
#include "core_variant/variant.hpp"
#include "core_reflection/object_handle.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_qt_common/private/component_version.hpp"
#include "core_qt_common/qt_framework_common.hpp"
#include "core_qt_common/qt_scripting_engine_base.hpp"
#include "core_data_model/i_model_extension.hpp"
#include "core_data_model/i_model_extension.mpp"

#include "qt_model_extension.hpp"
#include "custom_model_extension.hpp"
#include "column_extension.hpp"
#include "component_extension.hpp"
#include "grid_extension.hpp"
#include "image_extension.hpp"
#include "list_extension.hpp"
#include "table_extension.hpp"
#include "tree_extension.hpp"

#include <map>
#include <string>
#include <cassert>

#include <QtQuick>

#pragma warning(push)
#pragma warning(disable : 4503)
namespace wgt
{
namespace ModelExtensionManagerDetails
{
struct ExtensionCreator
{
	ExtensionCreator(QtFrameworkCommon& frameworkCommon) : frameworkCommon_(frameworkCommon)
	{
	}

	virtual QtModelExtension* create(QObject* view) = 0;

	QtFrameworkCommon& frameworkCommon_;
};

template <class T>
struct DefaultExtensionCreator : public ExtensionCreator
{
	DefaultExtensionCreator(QtFrameworkCommon& frameworkCommon) : ExtensionCreator(frameworkCommon)
	{
	}

	QtModelExtension* create(QObject* view) override
	{
		auto extension = new T();
		extension->setParent(view);
		extension->setFramework(frameworkCommon_);
		return extension;
	}
};

template <>
struct DefaultExtensionCreator<CustomModelExtension> : public ExtensionCreator
{
	DefaultExtensionCreator(QtFrameworkCommon& frameworkCommon, IDefinitionManager& definitionManager,
		std::function<ManagedObjectPtr()> creator)
	    : ExtensionCreator(frameworkCommon), definitionManager_(definitionManager), creator_(creator)
	{
	}

	QtModelExtension* create(QObject* view) override
	{
		auto extension = new CustomModelExtension();
		extension->setParent(view);
		extension->initialise(frameworkCommon_, view, creator_());
		return extension;
	}

	IDefinitionManager& definitionManager_;
	IClassDefinition* baseDefinition_;
	std::function<ManagedObjectPtr()> creator_;
};

typedef DefaultExtensionCreator<CustomModelExtension> CustomModelExtensionCreator;
}

struct ModelExtensionManager::Implementation
{
	using ExtensionCreator = ModelExtensionManagerDetails::ExtensionCreator;
	using ExtensionCreatorPointer = std::unique_ptr<ExtensionCreator>;

	// This struct is to avoid a 4503 type name length warning
	struct ExtensionCreators : public std::map<std::vector<int>, ExtensionCreatorPointer, ComponentVersion>
	{
	};

	Implementation(ModelExtensionManager& self, QtFrameworkCommon& qtFrameworkCommon);
	virtual ~Implementation();

	ExtensionCreator* findExtensionCreator(const char* id, const char* version) const;
	QtModelExtension* createExtension(const char* id, const char* version, QObject* view) const;

	bool registerExtension(const char* id, const char* version, ExtensionCreator* creator);

	template <class T>
	bool registerExtension(const char* id, const char* version)
	{
		auto versionList = ComponentVersion::tokenise(version);
		TF_ASSERT(versionList.size() == 2);
		qmlRegisterType<T>("WGControls", versionList[0], versionList[1], id);

		auto creator = new ModelExtensionManagerDetails::DefaultExtensionCreator<T>(qtFrameworkCommon_);
		return registerExtension(id, version, creator);
	}

	void registerDefaultExtensions();

	ModelExtensionManager& self_;
	QtFrameworkCommon& qtFrameworkCommon_;
	IDefinitionManager* definitionManager_;
	IClassDefinition* modelExtensionDefinition_;
	std::map<std::string, ExtensionCreators> extensions_;
};

ModelExtensionManager::Implementation::Implementation(ModelExtensionManager& self, QtFrameworkCommon& qtFrameworkCommon)
    : self_(self), qtFrameworkCommon_(qtFrameworkCommon), modelExtensionDefinition_(nullptr),
      definitionManager_(nullptr)
{
}

ModelExtensionManager::Implementation::~Implementation()
{
}

ModelExtensionManagerDetails::ExtensionCreator* ModelExtensionManager::Implementation::findExtensionCreator(
const char* id, const char* version) const
{
	auto extensionsItr = extensions_.find(id);

	if (extensionsItr == extensions_.end())
	{
		return nullptr;
	}

	auto& extensions = extensionsItr->second;

	if (extensions.empty())
	{
		return nullptr;
	}

	auto tokens = ComponentVersion::tokenise(version);
	auto extensionItr = extensions.upper_bound(tokens);
	--extensionItr;
	return extensionItr->second.get();
}

QtModelExtension* ModelExtensionManager::Implementation::createExtension(const char* id, const char* version,
                                                                         QObject* view) const
{
	auto creator = findExtensionCreator(id, version);
	if(creator == nullptr)
	{
		NGT_WARNING_MSG("Failed to create extension %s %s\n", id, version);
	}
	return creator ? creator->create(view) : nullptr;
}

bool ModelExtensionManager::Implementation::registerExtension(const char* id, const char* version,
                                                              ExtensionCreator* creator)
{
	auto& extensions = extensions_[id];
	auto tokens = ComponentVersion::tokenise(version);
	TF_ASSERT(extensions.find(tokens) == extensions.end());

	if (extensions.find(tokens) != extensions.end())
	{
		return false;
	}

	extensions[tokens].reset(creator);
	return true;
}

void ModelExtensionManager::Implementation::registerDefaultExtensions()
{
	registerExtension<ColumnExtension>("ColumnExtension", "2.0");
	registerExtension<ComponentExtension>("ComponentExtension", "2.0");
	registerExtension<GridExtension>("GridExtension", "2.0");
	registerExtension<ImageExtension>("ImageExtension", "2.0");
	registerExtension<ListExtension>("ListExtension", "2.0");
	registerExtension<TableExtension>("TableExtension", "2.0");
	registerExtension<TreeExtension>("TreeExtension", "2.0");
	qmlRegisterType<CustomModelExtension>("WGControls", 2, 0, "CustomModelExtension");
}

ModelExtensionManager::ModelExtensionManager(QtFrameworkCommon& qtFrameworkCommon)
    : impl_(new Implementation(*this, qtFrameworkCommon))
{
}

ModelExtensionManager::~ModelExtensionManager()
{
	impl_.reset();
}

void ModelExtensionManager::initialise()
{
	impl_->definitionManager_ = impl_->qtFrameworkCommon_.definitionManager();

	if (impl_->definitionManager_)
	{
		impl_->modelExtensionDefinition_ =
		impl_->definitionManager_->registerDefinition<TypeClassDefinition<IModelExtension>>();
	}

	impl_->registerDefaultExtensions();
}

QtModelExtension* ModelExtensionManager::createExtension(const QString& id, const QString& version, QObject* view)
{
	QtFrameworkCommon& qtFrameworkCommon = impl_->qtFrameworkCommon_;

	Variant variant = qtFrameworkCommon.toVariant(id);
	TF_ASSERT(variant.typeIs<const char*>() || variant.typeIs<std::string>());
	std::string stringId;
	variant.tryCast(stringId);

	variant = qtFrameworkCommon.toVariant(version);
	TF_ASSERT(variant.typeIs<const char*>() || variant.typeIs<std::string>());
	std::string stringVersion;
	variant.tryCast(stringVersion);

	return impl_->createExtension(stringId.c_str(), stringVersion.c_str(), view);
}

bool ModelExtensionManager::registerExtension(const char* id, const char* version, std::function<ManagedObjectPtr()> creator)
{
	TF_ASSERT(impl_->definitionManager_ != nullptr);
	using namespace ModelExtensionManagerDetails;
	auto extCreator = new CustomModelExtensionCreator(impl_->qtFrameworkCommon_, *impl_->definitionManager_, creator);
	return impl_->registerExtension(id, version, extCreator);
}
} // end namespace wgt
#pragma warning(pop)
