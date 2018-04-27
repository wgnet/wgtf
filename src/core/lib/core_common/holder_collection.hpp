#ifndef HOLDER_COLLECTION_HPP
#define HOLDER_COLLECTION_HPP

namespace wgt
{
template <typename T>
class HolderCollection
{
public:
	HolderCollection()
	{
	}
	HolderCollection(const T& connection)
	{
		*this += connection;
	}

	~HolderCollection()
	{
		reset();
	}

	void reset()
	{
		for (auto& connection : connections_)
		{
			connection.disconnect();
		}
		connections_.resize(0);
	}

	HolderCollection<T>& operator+=(T&& connection)
	{
		connections_.emplace_back(std::move(connection));
		return *this;
	}

	std::vector<T> connections_;
};

} // end namespace wgt

#endif // HOLDER_COLLECTION_HPP