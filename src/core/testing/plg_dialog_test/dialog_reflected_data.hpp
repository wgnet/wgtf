#ifndef DIALOG_REFLECTED_DATA_HPP
#define DIALOG_REFLECTED_DATA_HPP

#include "core_reflection/reflected_object.hpp"
#include "core_reflection/generic/generic_object.hpp"
#include "wg_types/vector3.hpp"
#include <vector>

namespace wgt
{
class IComponentContext;

class DialogReflectedData
{
	DECLARE_REFLECTED

public:
	DialogReflectedData();
	~DialogReflectedData();

private:
	static std::string getDisplayName(std::string path, const ObjectHandle&);

	void setColor(const Vector4& color);
	void getColor(Vector4* color) const;

	void setTextField(const std::wstring& text);
	void getTextField(std::wstring* text) const;

	void setVector(const GenericObjectPtr& vec);
	const GenericObjectPtr& getVector() const;

	void setSlideData(const double& length);
	void getSlideData(double* length) const;
	static int getSlideMaxData();
	static int getSlideMinData();

	std::wstring text_;
	double slider_;
	ManagedObject<GenericObject> vector_;
	Vector4 color_;
	std::map<std::string, Variant> checkBoxes_;
};

} // end namespace wgt
#endif // DIALOG_REFLECTED_DATA_HPP
