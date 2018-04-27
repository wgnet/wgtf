#ifndef PYOBJECT_POINTER_HPP
#define PYOBJECT_POINTER_HPP

namespace wgt
{
namespace PyScript
{
inline void incrementReferenceCount(const PyObject& Q)
{
	Py_INCREF(const_cast<PyObject*>(&Q)); // Q guaranteed non-null
}

inline void decrementReferenceCount(const PyObject& Q)
{
	Py_DECREF(const_cast<PyObject*>(&Q)); // Q guaranteed non-null
}

inline bool hasZeroReferenceCount(const PyObject& Q)
{
	return (const_cast<PyObject*>(&Q)->ob_refcnt == 0); // Q guaranteed non-null
}

class PyObjectPtr
{
public:
	/**
	 *	This constructor initialises this pointer to refer to the input object.
	 */
	PyObjectPtr(PyObject* P = 0, bool alreadyIncremented = false)
	{
		object_ = P;
		if (object_)
		{
			if (!alreadyIncremented)
			{
				incrementReferenceCount(*object_);
			}
			else
			{
				if (hasZeroReferenceCount(*object_))
				{
					assert(0 && "ConstSmartPointer::ConstSmartPointer - The reference "
					            "count should not be zero\nfor objects that already "
					            "have their reference incremented");
				}
			}
		}
	}

	/**
	 *	The copy constructor.
	 */
	PyObjectPtr(const PyObjectPtr& P)
	{
		object_ = P.get();
		if (object_)
		{
			incrementReferenceCount(*object_);
		}
	}

	/**
	 *	The assignment operator.
	 */
	PyObjectPtr& operator=(const PyObjectPtr& X)
	{
		if (object_ != X.get())
		{
			const PyObject* pOldObj = object_;
			object_ = X.get();

			if (object_)
				incrementReferenceCount(*object_);
			if (pOldObj)
				decrementReferenceCount(*pOldObj);
		}
		return *this;
	}

	/**
	 *	Destructor.
	 */
	~PyObjectPtr()
	{
		if (object_)
		{
			decrementReferenceCount(*object_);
		}
		object_ = 0;
	}

	/**
	 *	This method returns the object that this pointer points to.
	 */
	PyObject* get() const
	{
		return object_;
	}

	/**
	 *	This method returns whether or not this pointer points to anything.
	 */
	bool exists() const
	{
		return object_ != 0;
	}

	/**
	 *	This method implements the dereference operator. It helps allow this
	 *	object to be used as you would a normal pointer.
	 */
	const PyObject& operator*() const
	{
		assert(object_ != nullptr);
		return *object_;
	}

	PyObject& operator*()
	{
		assert(object_ != nullptr);
		return *object_;
	}

	/**
	 *	This method implements the dereference operator. It helps allow this
	 *	object to be used as you would a normal pointer.
	 */
	const PyObject* operator->() const
	{
		assert(object_ != nullptr);
		return object_;
	}

	PyObject* operator->()
	{
		assert(object_ != nullptr);
		return object_;
	}

	/**
	 *	These functions return whether or not the input objects refer to the same
	 *	object.
	 */
	friend bool operator==(const PyObjectPtr& A, const PyObjectPtr& B)
	{
		return A.object_ == B.object_;
	}

	friend bool operator==(const PyObjectPtr& A, const PyObject* B)
	{
		return A.object_ == B;
	}

	friend bool operator==(const PyObject* A, const PyObjectPtr& B)
	{
		return A == B.object_;
	}

	/**
	 *	These functions return not or whether the input objects refer to the same
	 *	object.
	 */
	friend bool operator!=(const PyObjectPtr& A, const PyObjectPtr& B)
	{
		return A.object_ != B.object_;
	}

	friend bool operator!=(const PyObjectPtr& A, const PyObject* B)
	{
		return A.object_ != B;
	}

	friend bool operator!=(const PyObject* A, const PyObjectPtr& B)
	{
		return A != B.object_;
	}

	/**
	 *	These functions give an ordering on smart pointers so that they can be
	 *	placed in sorted containers.
	 */
	friend bool operator<(const PyObjectPtr& A, const PyObjectPtr& B)
	{
		return A.object_ < B.object_;
	}

	friend bool operator<(const PyObjectPtr& A, const PyObject* B)
	{
		return A.object_ < B;
	}

	friend bool operator<(const PyObject* A, const PyObjectPtr& B)
	{
		return A < B.object_;
	}

	/**
	 *	These functions give an ordering on smart pointers so that they can be
	 *	compared.
	 */
	friend bool operator>(const PyObjectPtr& A, const PyObjectPtr& B)
	{
		return A.object_ > B.object_;
	}

	friend bool operator>(const PyObjectPtr& A, const PyObject* B)
	{
		return A.object_ > B;
	}

	friend bool operator>(const PyObject* A, const PyObjectPtr& B)
	{
		return A > B.object_;
	}

	/**
	 *	This method returns whether or not this pointers points to anything.
	 */

	typedef PyObject* PyObjectPtr::*unspecified_bool_type;
	operator unspecified_bool_type() const
	{
		return object_ == 0 ? 0 : &PyObjectPtr::object_;
	}

protected:
	PyObject* object_;
};

} // namespace PyScript
} // end namespace wgt
#endif // PYOBJECT_POINTER_HPP
