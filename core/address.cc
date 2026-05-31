#include "address.h"

namespace minibus {

uint32_t Address::Pack(Role role, Name name) {
  return (static_cast<uint32_t>(role) << 16) | static_cast<uint32_t>(name);
}

Role Address::GetRole(uint32_t addr) {
  return static_cast<Role>(addr >> 16);
}

Name Address::GetName(uint32_t addr) {
  return static_cast<Name>(addr & 0xFFFF);
}

std::string Address::RoleToString(Role role) {
  switch (role) {
    case Role::CAMERA:
      return "CAMERA";
    case Role::SCREEN:
      return "SCREEN";
    case Role::BOX:
      return "BOX";
    case Role::GUI:
      return "GUI";
    case Role::NONE:
    default:
      return "NONE";
  }
}

std::string Address::NameToString(Name name) {
  switch (name) {
    case Name::MEDIAD:
      return "MEDIAD";
    case Name::HALD:
      return "HALD";
    case Name::INS_GUI:
      return "INS_GUI";
    case Name::CAMERA_SVC:
      return "CAMERA_SVC";
    case Name::IBUSD_SELF:
      return "IBUSD_SELF";
    case Name::NONE:
    default:
      return "NONE";
  }
}

std::string Address::ToString(uint32_t addr) {
  return RoleToString(GetRole(addr)) + ":" + NameToString(GetName(addr));
}

}  // namespace minibus
