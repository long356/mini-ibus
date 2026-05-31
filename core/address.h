// address.h - NodeAddress = (Role, Name) packed into uint32
#pragma once

#include <cstdint>
#include <string>

namespace minibus {

enum class Role : uint16_t {
  NONE = 0,
  CAMERA = 1,
  SCREEN = 2,
  BOX = 3,
  GUI = 4,
};

enum class Name : uint16_t {
  NONE = 0,
  MEDIAD = 1,
  HALD = 2,
  INS_GUI = 3,
  CAMERA_SVC = 4,
  IBUSD_SELF = 5,
};

class Address {
 public:
  static uint32_t Pack(Role role, Name name);
  static Role GetRole(uint32_t addr);
  static Name GetName(uint32_t addr);
  static std::string ToString(uint32_t addr);
  static std::string RoleToString(Role role);
  static std::string NameToString(Name name);
};

}  // namespace minibus
