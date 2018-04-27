#ifndef REFLECTION_AUTO_REGISTER_HPP
#define REFLECTION_AUTO_REGISTER_HPP

namespace wgt
{

namespace ReflectionAutoRegistration
{
	template<size_t N>
	struct AutoReflectionRegistrar
	{
		static void autoRegister(
			IDefinitionManager& definitionManager,
			std::vector<IClassDefinition*>& o_Definitions)
		{
			AutoReflectionRegistrar<N - 1>::autoRegister(definitionManager, o_Definitions);
			o_Definitions.emplace_back(
				ReflectionCreatorHelper<N - 1>::autoRegister(definitionManager));
		}
	};

	template<>
	struct AutoReflectionRegistrar<0>
	{
		static void autoRegister(
			IDefinitionManager& definitionManager,
			std::vector<IClassDefinition*>& o_Definitions)
		{
		}
	};

	template<size_t Id = value()>
	static std::vector<IClassDefinition*>
		initAutoRegistration(IDefinitionManager& definitionManager)
	{
		std::vector<IClassDefinition*> definitions;
		AutoReflectionRegistrar<Id>::autoRegister(definitionManager, definitions);
		return definitions;
	}
}// end namespace ReflectionAutoRegistration

}//end namespace wgt

#endif //REFLECTION_AUTO_REGISTER_HPP