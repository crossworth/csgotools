#pragma once

#include <vector>
#include <unordered_map>
#include <set>

#include "ErrorHandling.hpp"
#include "DemoMemoryBitStream.hpp"
#include "ServerClass.hpp"
#include "SendProp.hpp"
#include "FlatSendProp.hpp"


namespace csgotools {

    class SendTable {
        static constexpr uint32 kNetMaxPayload = 262140; 
        static constexpr uint32 kDemoRecordBufferSize = 2097152;

        bool is_end_{false};
        std::string net_table_name_;
        bool needs_decoder_{ false };
        std::vector<SendProp> props_;


        static void GatherExcluded(SendTable& table, 
                                   std::set<std::string>& excluded, 
                                   std::unordered_map<std::string, SendTable>& tables);

        static void GatherProps(SendTable& table,
                                std::set<std::string>& excluded,
                                std::unordered_map<std::string, SendTable>& tables,
                                ServerClass& server_class);

        static void GatherPropsIterator(SendTable& table,
                                        std::vector<FlatSendProp>& flattened_props,
                                        std::set<std::string>& excluded,
                                        std::unordered_map<std::string, SendTable>& tables,
                                        ServerClass& server_class);

        static void GetPriorities(std::vector<uint32>& priorities, std::vector<FlatSendProp>& flattened_props);
        static void SortPropsByPriority(std::vector<uint32>& priorities, std::vector<FlatSendProp>& flattened_props);

    public:
        bool IsEnd() const { return is_end_; }
        void IsEnd(bool is_end) { is_end_ = is_end; }
        std::string NetTableName() const { return net_table_name_; }
        void NetTableName(const std::string& net_table_name) { net_table_name_ = net_table_name; }
        bool NeedsDecoder() const { return needs_decoder_; }
        void NeedsDecoder(bool needs_decoder) { needs_decoder_ = needs_decoder; }
        std::vector<SendProp>& Props() { return props_; }
        void Props(const std::vector<SendProp>& send_props) { props_ = send_props; }
        void MakeFlat(ServerClass& server_class, std::unordered_map<std::string, SendTable>& tables);

        static SendTable CreateFromMemory(DemoMemoryBitStream& memory);
    };
}
