#ifndef PROJECT_HPP
#define PROJECT_HPP

#include "core_reflection/reflected_object.hpp"
#include "core_reflection/object_handle.hpp"
#include "wg_types/vector3.hpp"
#include "wg_types/vector4.hpp"
#include "core_common/wg_future.hpp"
#include "core_object/managed_object.hpp"
#include "core_ui_framework/i_view.hpp"
#include "core_dependency_system/depends.hpp"
#include "core_environment_system/i_env_system.hpp"
#include <string>
#include <unordered_map>

namespace wgt
{
class AbstractTreeModel;
class IEnvManager;
class IViewCreator;
class IDefinitionManager;
class IFileSystem;
class IUIFramework;
class IUIApplication;
class ICommandManager;
class IPreferences;
class IViewport;
class ProjectData;

class Project
: Depends<IEnvManager, IViewCreator, IDefinitionManager, IFileSystem, IUIFramework, IUIApplication, ICommandManager>
{
public:
	Project();
	~Project();
	bool init(const char* projectName, const char* dataFile = nullptr);
	void fini();
	void saveData(const char* dataFile) const;
	const char* getProjectName() const;
	const ObjectHandle& getProjectData() const;

private:
    ManagedObject<ProjectData> projectData_;
    ObjectHandle projectHandle_;
	std::string projectName_;
	wg_future<std::unique_ptr<IView>> view_;
	std::unique_ptr<AbstractTreeModel> model_;
	std::unique_ptr<IViewport> viewport_;
};

class ProjectManager : Depends<IDefinitionManager, IFileSystem, IUIFramework, IPreferences>
{
public:
	ProjectManager();
	void init();
	void fini();

	void createProject();
	void openProject();
	void saveProject() const;
	void closeProject();
	bool canOpen() const;
	bool canSave() const;
	bool canClose() const;

	// exposed method
	bool isProjectNameOk(const Variant& strProjectName);
	void setNewProjectName(const Variant& strProjectFile);
	void setOpenProjectFile(const Variant& strProjectFile);

private:
	std::unordered_map<std::string, Project*> projects_;
	std::unique_ptr<Project> curProject_;
	std::string newProjectName_;
	std::string openProjectFile_;
};

class ProjectData
{
	DECLARE_REFLECTED
public:
	ProjectData();
	~ProjectData();

private:
	void setCheckBoxState(const bool& bChecked);
	void getCheckBoxState(bool* bChecked) const;

	void setTextField(const std::wstring& text);
	void getTextField(std::wstring* text) const;

	void setSlideData(const double& length);
	void getSlideData(double* length) const;

	static int getSlideMaxData();
	static int getSlideMinData();

	void setNumber(const int& num);
	void getNumber(int* num) const;

	void setSelected(const int& select);
	void getSelected(int* select) const;

	void setVector3(const Vector3& vec3);
	void getVector3(Vector3* vec3) const;

	void setVector4(const Vector4& vec4);
	void getVector4(Vector4* vec4) const;

	void setColor3(const Vector3& color);
	void getColor3(Vector3* color) const;

	void setColor4(const Vector4& color);
	void getColor4(Vector4* color) const;

	bool bChecked_;
	std::wstring text_;
	double curSlideData_;
	int curNum_;
	int curSelected_;
	int enumValue_;
	Vector3 vec3_;
	Vector4 vec4_;
	Vector3 color3_;
	Vector4 color4_;
};
} // end namespace wgt
#endif // PROJECT_HPP
