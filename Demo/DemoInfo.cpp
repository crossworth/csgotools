#include "DemoInfo.hpp"

using namespace csgotools;

bool DemoInfo::Open(const std::string& file_name) {
    std::string string_buffer;

    std::ifstream file;
    file.open(file_name.c_str(), std::ios::in | std::ios::binary);

    if (file.is_open()) {
        file.seekg(0, std::ios::end);
        file_size_ = static_cast<size_t>(file.tellg());
        file.seekg(0, std::ios::beg);

        if (file_size_ <= 0) {
            std::cerr << "DemoInfo::Open: File too small: " << file_size_ << " bytes" << std::endl;
            file.close();
            return false;
        }

        string_buffer.resize(file_size_);
        file.read(&string_buffer[0], file_size_);
        file.close();

        memory_ = MemoryBitStream(string_buffer.c_str(), file_size_);
        ParseDemoInfo();
    } else {
        std::cerr << "DemoInfo::Open: Could not open the file: " << file_name << std::endl;
        return false;
    }

    return true;
}

bool DemoInfo::ParseDemoInfo() {
    CDataGCCStrike15_v2_MatchInfo data;

    data.ParseFromArray(memory_.GetBufferPointer(), file_size_);
    if (data.has_matchid() && data.has_matchtime()) {
        match_date_ = data.matchtime();
        match_id_ = data.matchid();
        is_valid_ = true;
        return true;
    } else {
        std::cerr << "DemoInfo::ParseDemoInfo: Could not read the match id or the match time" << std::endl;
        is_valid_ = false;
    }

    return false;
}

std::string DemoInfo::GetDateGMT() const {
    if (!is_valid_) {
        return "";
    }

    std::stringstream ss;
    ss << std::put_time(std::gmtime(&match_date_), "%c %Z");
    return ss.str();
}

std::string DemoInfo::GetDateUTC() const {
    if (!is_valid_) {
        return "";
    }

    std::stringstream ss;
    ss << std::put_time(std::localtime(&match_date_), "%c %Z");
    return ss.str();
}