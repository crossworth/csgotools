#include "Team.hpp"

using namespace csgotools;

std::ostream& csgotools::operator<<(std::ostream& out, const Team& team) {
	out << "------------Team------------" << std::endl;
	out << "Name: " << team.GetName() << std::endl;
	out << "Type: ";
	switch (team.GetType()) {
		case Team::Type::SPECTATOR :{
			out << "Team::Type::SPECTATOR";
		} break;
		case Team::Type::TERRORIST: {
			out << "Team::Type::TERRORIST";
		} break;
		case Team::Type::COUNTER_TERRORIST: {
			out << "Team::Type::COUNTER_TERRORIST";
		} break;
		default: {
			out << "Team::Type::UNKNOWN";
		} break;
	}
	out << std::endl;
	out << "Number: " << team.GetNumber() << std::endl;
	out << "Clan Name: " << team.GetClanName() << std::endl;
	out << "ClanID: " << team.GetClanID() << std::endl;
	out << "Logo Image: " << team.GetLogoImage() << std::endl;
	out << "Flag Image: " << team.GetFlagImage() << std::endl;
	out << "Surrendered: " << team.Surrendered() << std::endl;
	out << "Total score: " << team.GetTotalScore() << std::endl;
	out << "Score First half: " << team.GetScoreFirstHalf() << std::endl;
	out << "Score Second half: " << team.GetScoreSecondHalf() << std::endl;
	out << "Score Overtime: " << team.GetScoreOvertime() << std::endl;
	out << "------------Team------------" << std::endl;
	return out;
}
