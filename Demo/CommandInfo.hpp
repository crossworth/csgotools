#ifndef CSGOTOOLS_COMMAND_INFO_HPP_
#define CSGOTOOLS_COMMAND_INFO_HPP_

#include <Steam.hpp>
#include <Vector.hpp>
#include <QAngle.hpp>
#include <MemoryBitStream.hpp>


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
				view_origin_ = Vector();
				view_angles_ = QAngle();
				local_view_angles_ = QAngle();

				view_origin_2_ = Vector();
				view_angles_2_ = QAngle();
				local_view_angles_2_ = QAngle();
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

			const Vector& GetViewOrigin() {
				if (flags & CommandInfo::kFDemoUseOrigin2) {
					return view_origin_2_;
				}
				
				return view_origin_;
			}

			const QAngle& GetViewAngles() {
				if (flags & CommandInfo::kFDemoUseAngles2) {
					return view_angles_2_;;
				}
				return view_angles_;
			}

			const QAngle& GetLocalViewAngles() {
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

			Vector view_origin_;
			QAngle view_angles_;
			QAngle local_view_angles_;

			Vector view_origin_2_;
			QAngle view_angles_2_;
			QAngle local_view_angles_2_;
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

#endif