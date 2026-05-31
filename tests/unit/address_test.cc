#include <gtest/gtest.h>
#include "core/address.h"

namespace minibus {
namespace {

TEST(AddressTest, PackThenGetRoleRoundtrip) {
  uint32_t addr = Address::Pack(Role::CAMERA, Name::MEDIAD);
  EXPECT_EQ(Address::GetRole(addr), Role::CAMERA);
}

TEST(AddressTest, PackThenGetNameRoundtrip) {
  uint32_t addr = Address::Pack(Role::SCREEN, Name::INS_GUI);
  EXPECT_EQ(Address::GetName(addr), Name::INS_GUI);
}

TEST(AddressTest, PackNoneNoneReturnsZero) {
  uint32_t addr = Address::Pack(Role::NONE, Name::NONE);
  EXPECT_EQ(addr, 0u);
}

TEST(AddressTest, RoleAndNameOccupyCorrectBits) {
  uint32_t addr = Address::Pack(Role::BOX, Name::HALD);
  // Role occupies high 16 bits, Name occupies low 16 bits
  EXPECT_EQ(addr >> 16, static_cast<uint32_t>(Role::BOX));
  EXPECT_EQ(addr & 0xFFFF, static_cast<uint32_t>(Name::HALD));
}

TEST(AddressTest, ToStringFormat) {
  uint32_t addr = Address::Pack(Role::CAMERA, Name::MEDIAD);
  EXPECT_EQ(Address::ToString(addr), "CAMERA:MEDIAD");
}

TEST(AddressTest, ToStringWithNone) {
  uint32_t addr = Address::Pack(Role::NONE, Name::NONE);
  EXPECT_EQ(Address::ToString(addr), "NONE:NONE");
}

TEST(AddressTest, AllRolesHaveString) {
  EXPECT_FALSE(Address::RoleToString(Role::CAMERA).empty());
  EXPECT_FALSE(Address::RoleToString(Role::SCREEN).empty());
  EXPECT_FALSE(Address::RoleToString(Role::BOX).empty());
  EXPECT_FALSE(Address::RoleToString(Role::GUI).empty());
  EXPECT_FALSE(Address::RoleToString(Role::NONE).empty());
}

TEST(AddressTest, AllNamesHaveString) {
  EXPECT_FALSE(Address::NameToString(Name::MEDIAD).empty());
  EXPECT_FALSE(Address::NameToString(Name::HALD).empty());
  EXPECT_FALSE(Address::NameToString(Name::INS_GUI).empty());
  EXPECT_FALSE(Address::NameToString(Name::CAMERA_SVC).empty());
  EXPECT_FALSE(Address::NameToString(Name::IBUSD_SELF).empty());
  EXPECT_FALSE(Address::NameToString(Name::NONE).empty());
}

}  // namespace
}  // namespace minibus
