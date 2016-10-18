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

	void initialise(IComponentContext& context);
	const DialogReflectedData& operator=(const DialogReflectedData& data);

private:
	void setCheckBoxState(const bool& checked);
	void getCheckBoxState(bool* checked) const;

	void setColor(const Vector3& color);
	void getColor(Vector3* color) const;

	void setTextField(const std::wstring& text);
	void getTextField(std::wstring* text) const;

	void setVector(const GenericObjectPtr& vec);
	const GenericObjectPtr& getVector() const;

	void setSlideData(const double& length);
	void getSlideData(double* length) const;
	static int getSlideMaxData();
	static int getSlideMinData();

	struct Data;
	std::unique_ptr<Data> data_;
};

} // end namespace wgt
#endif // DIALOG_REFLECTED_DATA_HPP
