#ifndef CSGOTOOLS_SERVER_CLASS_HPP_
#define CSGOTOOLS_SERVER_CLASS_HPP_


#include <Steam.hpp>
#include "FlattenedPropEntry.hpp"

namespace csgotools {
	class ServerClass {
	public:
		ServerClass() = default;

		int32 GetClassID() const { return class_id_; }
		std::string GetName() const { return name_; }
		std::string GetDTName() const { return dt_name_; }
		void SetClassID(int32 class_id) { class_id_ = class_id; }
		void SetName(const std::string& name) { name_ = name; }
		void SetDTName(const std::string& dt_name) { dt_name_ = dt_name; }

		std::vector<FlattenedPropEntry>& GetFlattenedPropsEntry() { return flattened_props_; }
	private:
		int32 class_id_{};
		std::string name_;
		std::string dt_name_;
		std::vector<FlattenedPropEntry> flattened_props_;
	};
}

#endif

