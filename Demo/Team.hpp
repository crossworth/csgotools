#ifndef CSGOTOOLS_TEAM_HPP_
#define CSGOTOOLS_TEAM_HPP_

#include <Steam.hpp>

namespace csgotools {

	class Team {
	public:

		enum class Type {
			SPECTATOR = 1,
			TERRORIST,
			COUNTER_TERRORIST,
			UNKNOWN
		};

		Team() = default;
		
		uint32 GetNumber() const { return number_; }
		uint32 GetTotalScore() const { return total_score_; }
		uint32 GetScoreFirstHalf() const { return score_first_half_; }
		uint32 GetScoreSecondHalf() const { return score_second_half_; }
		uint32 GetScoreOvertime() const { return score_overtime_; }
		uint32 GetClanID() const { return clan_id_; }
		bool Surrendered() const { return surrendered_; }
		std::string GetClanName() const { return clan_name_; }
		std::string GetName() const { return name_; }
		std::string GetFlagImage() const { return flag_image_; }
		std::string GetLogoImage() const { return logo_image_; }
		Type GetType() const { return type_; }

		void SetNumber(uint32 number) { number_ = number; }
		void SetTotalScore(uint32 total_score) { total_score_ = total_score; }
		void SetScoreFirstHalf(uint32 score_first_half) { score_first_half_ = score_first_half; }
		void SetScoreSecondHalf(uint32 score_second_half) { score_second_half_ = score_overtime_; }
		void SetScoreOvertime(uint32 score_overtime) { score_overtime_ = score_overtime; }
		void SetClanID(uint32 clan_id) { clan_id_ = clan_id; }
		void SetSurrendered(bool surrendered) { surrendered_ = surrendered; }
		void SetClanName(const std::string& clan_name) { clan_name_ = clan_name; }
		void SetName(const std::string& name) { name_ = name; }
		void SetFlagImage(const std::string& flag_image) { flag_image_ = flag_image; }
		void SetLogoImage(const std::string& logo_image) { logo_image_ = logo_image; }
		void SetType(const Type& type) { type_ = type; }

	private:
		uint32 number_{};
		uint32 total_score_{};
		uint32 score_first_half_{};
		uint32 score_second_half_{};
		uint32 score_overtime_{};
		uint32 clan_id_{};
		bool surrendered_{false};
		std::string clan_name_;
		std::string name_;
		std::string flag_image_;
		std::string logo_image_;
		Type type_{Type::UNKNOWN};
	};

	std::ostream& operator<<(std::ostream& out, const Team& team);
}

#endif