#ifndef SERVICE_HPP
#define SERVICE_HPP

#include <memory>
#include <unordered_map>

#include "Arc_Assert.hpp"

namespace Arcusical { namespace ServiceModel {
	
	template<typename T>
	class ServiceResolver final
	{
	public:

		typedef std::weak_ptr<T> ServiceRef;

		static ServiceRef ResolveService();
		static ServiceRef ResolveUrn(const std::string&);

		static void RegisterSingleton(std::shared_ptr<T> singleton);
		static void DeregisterSingleton();
		static void RegisterUrn(const std::string& urn, std::shared_ptr<T> specificInstance);
		static void DeregisterUrn(const std::string& urn);

		ServiceResolver() = delete;

	private:

		static std::shared_ptr<T> s_singleton;
		static std::unordered_map<std::string, std::shared_ptr<T>> s_serviceMap;
	};

	/************************************************************************/
	/* Implementation                                                       */
	/************************************************************************/

	template<typename T>
	std::shared_ptr<T> ServiceResolver<T>::s_singleton(nullptr);
	template<typename T>
	std::unordered_map<std::string, std::shared_ptr<T>> ServiceResolver<T>::s_serviceMap;

	template<typename T>
	void ServiceResolver<T>::RegisterSingleton(std::shared_ptr<T> singleton)
	{
		ARC_ASSERT_MSG(s_singleton == nullptr,	"A service of name " << T::ServiceName 
												<< " has already been registered! New registration will stomp old registration!");
		s_singleton = singleton;
	}

	template<typename T>
	void ServiceResolver<T>::RegisterUrn(const std::string& urn, std::shared_ptr<T> specificInstance)
	{
		ARC_ASSERT_MSG(s_serviceMap.find(urn) == s_serviceMap.end(),	"A urn of name : " << urn 
																		<< " for the service " << T:ServiceName 
																		<< " has already been registered! New Registration will stomp old registration!");
		s_serviceMap[urn] = specificInstance;
	}

	template<typename T>
	typename ServiceResolver<T>::ServiceRef ServiceResolver<T>::ResolveService()
	{
		ARC_ASSERT_MSG(s_singleton != nullptr, "Attempting to access " << T::ServiceName << " when one hasn't been registered!");
		return s_singleton;
	}

	template<typename T>
	typename ServiceResolver<T>::ServiceRef ServiceResolver<T>::ResolveUrn(const std::string& urn)
	{
		ARC_ASSERT_MSG(s_serviceMap.find(urn) != s_serviceMap.end(), "Attempted to access urn : " << urn << " for service " << T::ServiceName << " which is not registered!");
		return s_serviceMap.at(urn);
	}

	template<typename T>
	void ServiceResolver<T>::DeregisterSingleton()
	{
		ARC_ASSERT_MSG(s_singleton != nullptr, "Attempted to deregister service of name " << T::ServiceName << " when there was such service registered!");
		s_singleton = nullptr;
	}

	template<typename T>
	void ServiceResolver<T>::DeregisterUrn(const std::string& urn)
	{
		ARC_ASSERT_MSG(s_serviceMap.find(urn) != s_serviceMap.end(),	"Attempted to deregister urn: " << urn << " for service "
																		<< T::ServiceName << " when no such urn is registered!");
		s_serviceMap.erase(urn);
	}

}/* Service Model */ } /* Arcusical */

#endif