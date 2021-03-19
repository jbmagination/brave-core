/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/components/permissions/permission_expiration_key.h"

#include <algorithm>

#include "base/strings/string_number_conversions.h"
#include "base/strings/string_util.h"

namespace permissions {

namespace {

base::Time ParseExpirationTime(const std::string& key_str) {
  int64_t expiration_time = 0;
  if (!base::StringToInt64(key_str, &expiration_time)) {
    return base::Time();
  }
  return base::Time::FromDeltaSinceWindowsEpoch(
      base::TimeDelta::FromMicroseconds(expiration_time));
}

std::string ExpirationTimeToStr(base::Time expiration_time) {
  return std::to_string(
      expiration_time.ToDeltaSinceWindowsEpoch().InMicroseconds());
}

bool IsDot(char c) {
  return c == '.';
}

}  // namespace

PermissionExpirationKey::PermissionExpirationKey(base::Time time)
    : time_(time) {}
PermissionExpirationKey::PermissionExpirationKey(std::string domain)
    : time_(base::Time::Max()), domain_(std::move(domain)) {
  DCHECK(!domain_.empty());
  // Replace dots so the key can be a part of a path to store in prefs properly.
  std::replace(domain_.begin(), domain_.end(), '.', '_');
}
PermissionExpirationKey::PermissionExpirationKey(
    const PermissionExpirationKey&) = default;
PermissionExpirationKey& PermissionExpirationKey::operator=(
    const PermissionExpirationKey&) = default;
PermissionExpirationKey::PermissionExpirationKey(
    PermissionExpirationKey&&) noexcept = default;
PermissionExpirationKey& PermissionExpirationKey::operator=(
    PermissionExpirationKey&&) noexcept = default;
PermissionExpirationKey::~PermissionExpirationKey() = default;

// static
PermissionExpirationKey PermissionExpirationKey::FromPrefKey(
    const std::string& pref_key) {
  DCHECK(!std::any_of(pref_key.begin(), pref_key.end(), IsDot));
  auto expiration_time = ParseExpirationTime(pref_key);
  return !expiration_time.is_null() ? PermissionExpirationKey(expiration_time)
                                    : PermissionExpirationKey(pref_key);
}

std::string PermissionExpirationKey::ToPrefKey() const {
  std::string pref_key = IsTimeKey() ? ExpirationTimeToStr(time_) : domain_;
  DCHECK(!std::any_of(pref_key.begin(), pref_key.end(), IsDot));
  return pref_key;
}

bool PermissionExpirationKey::operator<(
    const PermissionExpirationKey& rhs) const {
  auto tie = [](const PermissionExpirationKey& obj) {
    return std::tie(obj.time_, obj.domain_);
  };
  return tie(*this) < tie(rhs);
}

bool PermissionExpirationKey::operator==(
    const PermissionExpirationKey& rhs) const {
  auto tie = [](const PermissionExpirationKey& obj) {
    return std::tie(obj.time_, obj.domain_);
  };
  return tie(*this) == tie(rhs);
}

bool PermissionExpirationKey::IsTimeKey() const {
  return domain_.empty();
}

}  // namespace permissions
