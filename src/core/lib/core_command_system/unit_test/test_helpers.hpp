#ifndef TEST_HELPER_HPP
#define TEST_HELPER_HPP

namespace wgt
{
template <class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
	std::copy(vec.begin(), vec.end(), std::ostream_iterator<T>(os, ", "));
	return os;
}

template <class K, class T>
std::ostream& operator<<(std::ostream& os, const std::map<K, T>& m)
{
	for (std::map<K, T>::const_iterator iter = m.begin(), end = m.end(); iter != end; ++iter)
	{
		os << iter->first << ": " << iter->second << std::endl;
	}
	return os;
}
} // end namespace wgt
#endif // TEST_HELPER_HPP
