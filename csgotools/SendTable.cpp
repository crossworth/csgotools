#include "SendTable.hpp"
#include "netmessages_public.pb.h"

using namespace csgotools;

void SendTable::GatherExcluded(SendTable& table,
                               std::set<std::string>& excluded,
                               std::unordered_map<std::string, SendTable>& tables) {

    for (auto& prop : table.Props()) {
        if (prop.Flags() & SendProp::kSPropExclude) {
            excluded.insert(prop.DTName() + prop.VarName());
        } else if (static_cast<SendPropType>(prop.Type()) == SendPropType::DATA_TABLE) {
            if (tables.find(prop.DTName()) != tables.end()) {
                SendTable& sub_table = tables[prop.DTName()];
                GatherExcluded(sub_table, excluded, tables);
            }
        }
    }
}

void SendTable::GatherProps(SendTable& table, std::set<std::string>& excluded, 
                            std::unordered_map<std::string, SendTable>& tables,
                            ServerClass& server_class) {
    GatherPropsIterator(table, server_class.FlattenedProps(), excluded, tables, server_class);
}

void SendTable::GatherPropsIterator(SendTable& table,
                                    std::vector<FlatSendProp>& flattened_props,
                                    std::set<std::string>& excluded,
                                    std::unordered_map<std::string, SendTable>& tables,
                                    ServerClass& server_class) {

    for (uint32 i = 0; i < table.Props().size(); ++i) {
        const auto& prop = table.Props()[i];

        std::string excluded_name{ table.NetTableName() + prop.VarName() };
        if (prop.Flags() & SendProp::kSPropInsideArray || prop.Flags() & SendProp::kSPropExclude) {
            continue;
        } else if (excluded.count(excluded_name)) {
            continue;
        } else if (static_cast<SendPropType>(prop.Type()) == SendPropType::DATA_TABLE) {
            if (tables.find(prop.DTName()) != tables.end()) {
                auto& sub_table = tables[prop.DTName()];

                if (prop.Flags() & SendProp::kSPropInsideArray) {
                    GatherPropsIterator(sub_table, flattened_props, excluded, tables, server_class);
                } else {
                    GatherProps(sub_table, excluded, tables, server_class);
                }
            }
        } else {
            if (static_cast<SendPropType>(prop.Type()) == SendPropType::ARRAY) {
                flattened_props.emplace_back(&prop, &(table.Props()[i -1]));
            } else {
                flattened_props.emplace_back(&prop, nullptr);
            }
        }
    }
}

void SendTable::MakeFlat(ServerClass& server_class, std::unordered_map<std::string, SendTable>& tables) {
    std::set<std::string> excluded;
    GatherExcluded(*this, excluded, tables);
    GatherProps(*this, excluded, tables, server_class);

    auto& flattened_props = server_class.FlattenedProps();
    std::vector<uint32> priorities;
    priorities.push_back(64);

    GetPriorities(priorities, flattened_props);
    SortPropsByPriority(priorities, flattened_props);
}

void SendTable::GetPriorities(std::vector<uint32>& priorities, std::vector<FlatSendProp>& flattened_props) {
    for (auto& prop : flattened_props) {
        uint32 priority = prop.prop->Priority();

        bool found = false;

        for (uint32 i = 0; i < priorities.size(); ++i) {
            if (priorities[i] == priority) {
                found = true;
                break;
            }
        }

        if (!found) {
            priorities.push_back(priority);
        }
    }
}

void SendTable::SortPropsByPriority(std::vector<uint32>& priorities, std::vector<FlatSendProp>& flattened_props) {
    std::sort(priorities.begin(), priorities.end());

    uint32 start = 0;
    for (auto& priority : priorities) {
        while (true) {
            uint32 current_prop = start;
            while (current_prop < flattened_props.size()) {
                const auto& flat_prop = flattened_props[current_prop];

                if (flat_prop.prop->Priority() == priority || priority == 64 &&
                    flat_prop.prop->Flags() & SendProp::kSPropChangesOften) {
                    if (start != current_prop) {
                        std::swap(flattened_props[start], flattened_props[current_prop]);
                    }
                    ++start;
                    break;
                }
                ++current_prop;
            }

            if (current_prop == flattened_props.size()) {
                break;
            }
        }
    }
}

SendTable SendTable::CreateFromMemory(DemoMemoryBitStream& memory) {
    SendTable result;
    CSVCMsg_SendTable table;

    int32 type = memory.ReadVarInt32();

    if (type != SVC_Messages::svc_SendTable) {
        CSGOTOOLS_ERROR("SendTable::CreateFromMemory: SendTable wrong type: " + std::to_string(static_cast<int>(type)));
        return result;
    }
    
    int32 size = memory.ReadVarInt32();

    if (size < 0 || size > SendTable::kNetMaxPayload) {
        CSGOTOOLS_ERROR("SendTable::CreateFromMemory: Invalid Net Max Payload size");
        return result;
    }

    if (size > SendTable::kDemoRecordBufferSize) {
        CSGOTOOLS_ERROR("SendTable::CreateFromMemory: Invalid Record Buffer size");
        return result;
    }

    char* buffer = new char[size];
    memory.ReadBytes(buffer, size);
    table.ParseFromArray(buffer, size);
    delete[] buffer;

    result.IsEnd(table.is_end());
    result.NetTableName(table.net_table_name());
    result.NeedsDecoder(table.needs_decoder());

    for (int32 i = 0; i < table.props_size(); ++i) {
        auto& prop = table.props(i);

        result.props_.emplace_back(static_cast<SendPropType>(prop.type()),
                                   prop.var_name(),
                                   prop.flags(),
                                   prop.priority(),
                                   prop.dt_name(),
                                   prop.num_elements(),
                                   prop.low_value(),
                                   prop.high_value(),
                                   prop.num_bits());
    }

    return result;
}
