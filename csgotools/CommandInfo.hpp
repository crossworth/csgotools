#pragma once

#include "Steam.hpp"
#include "Vector.hpp"
#include "MemoryBitStream.hpp"

namespace csgotools {

    class CommandInfo {
        static constexpr int32 kFDemoNormal = 0;
        static constexpr int32 kFDemoUseOrigin2 = 1 << 0;
        static constexpr int32 kFDemoUseAngles2 = 1 << 1;
        static constexpr int32 kMaxSplitScreenClients = 2;
    public:

        struct Split {
            Split() {
                flags = CommandInfo::kFDemoNormal;
                view_origin_ = Vector<float>();
                view_angles_ = Vector<float>();
                local_view_angles_ = Vector<float>();

                view_origin_2_ = Vector<float>();
                view_angles_2_ = Vector<float>();
                local_view_angles_2_ = Vector<float>();
            }

            Split& operator=(const Split &source) {
                if (this == &source) {
                    return *this;
                }

                flags = source.flags;
                view_origin_ = source.view_origin_;
                view_angles_ = source.view_angles_;
                local_view_angles_ = source.local_view_angles_;
                view_origin_2_ = source.view_origin_2_;
                view_angles_2_ = source.view_angles_2_;
                local_view_angles_2_ = source.local_view_angles_2_;

                return *this;
            }

            const Vector<float>& GetViewOrigin() const {
                if (flags & CommandInfo::kFDemoUseOrigin2) {
                    return view_origin_2_;
                }

                return view_origin_;
            }

            const Vector<float>& GetViewAngles() const {
                if (flags & CommandInfo::kFDemoUseAngles2) {
                    return view_angles_2_;;
                }
                return view_angles_;
            }

            const Vector<float>& GetLocalViewAngles() const {
                if (flags & CommandInfo::kFDemoUseAngles2) {
                    return local_view_angles_;
                }
                return local_view_angles_2_;
            }

            void Reset() {
                flags = 0;
                view_origin_2_ = view_origin_;
                view_angles_2_ = view_angles_;
                local_view_angles_2_ = local_view_angles_;
            }

            int32 flags;

            Vector<float> view_origin_;
            Vector<float> view_angles_;
            Vector<float> local_view_angles_;

            Vector<float> view_origin_2_;
            Vector<float> view_angles_2_;
            Vector<float> local_view_angles_2_;
        };


        void Reset() {
            for (int32 i = 0; i < CommandInfo::kMaxSplitScreenClients; i++) {
                u[i].Reset();
            }
        }

        Split u[CommandInfo::kMaxSplitScreenClients];

        CommandInfo() = default;

        static CommandInfo CreateFromMemory(MemoryBitStream& memory);
    };

}
