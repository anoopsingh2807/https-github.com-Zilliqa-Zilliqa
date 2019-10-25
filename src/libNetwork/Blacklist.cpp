/*
 * Copyright (C) 2019 Zilliqa
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "Blacklist.h"
#include "libUtils/IPConverter.h"
#include "libUtils/Logger.h"

using namespace std;

Blacklist::Blacklist() : m_enabled(true) {}

Blacklist::~Blacklist() {}

Blacklist& Blacklist::GetInstance() {
  static Blacklist blacklist;
  return blacklist;
}

/// P2PComm may use this function
bool Blacklist::Exist(const uint128_t& ip, [[gnu::unused]] const bool strict) {
  if (!m_enabled) {
    return false;
  }
  lock_guard<mutex> g(m_mutexBlacklistIP);
  return (m_blacklistIP.end() != m_blacklistIP.find(ip) &&
          (m_whitelistedIP.end() == m_whitelistedIP.find(ip)));
  /*
    lock_guard<mutex> g(m_mutexBlacklistIP);
    const auto& bl = m_blacklistIP.find(ip);
    if (bl != m_blacklistIP.end()) {
      if (strict) {
        // always return exist when strict, must be checked while sending
    message return true;
      }

      return bl->second;
    }
    return false;
  */
}

/// Reputation Manager may use this function
void Blacklist::Add(const uint128_t& ip, [[gnu::unused]] const bool strict) {
  if (!m_enabled) {
    return;
  }

  lock_guard<mutex> g(m_mutexBlacklistIP);
  if (m_whitelistedIP.end() == m_whitelistedIP.find(ip)) {
    /*
    const auto& res = m_blacklistIP.emplace(ip, strict);
    // already existed, then over-ride strictness i.e. false by true
    if (!res.second && strict) {
      res.first->second = strict;
    }
    */
    m_blacklistIP.emplace(ip, true);
  } else {
    LOG_GENERAL(INFO,
                "Whitelisted IP: " << IPConverter::ToStrFromNumericalIP(ip));
  }
}

/// Reputation Manager may use this function
void Blacklist::Remove(const uint128_t& ip) {
  if (!m_enabled) {
    return;
  }

  lock_guard<mutex> g(m_mutexBlacklistIP);
  m_blacklistIP.erase(ip);
}

/// Reputation Manager may use this function
void Blacklist::Clear() {
  lock_guard<mutex> g(m_mutexBlacklistIP);
  m_blacklistIP.clear();
  LOG_GENERAL(INFO, "Blacklist cleared");
}

void Blacklist::Pop(unsigned int num_to_pop) {
  if (!m_enabled) {
    return;
  }

  lock_guard<mutex> g(m_mutexBlacklistIP);
  LOG_GENERAL(INFO, "Num of nodes in blacklist: " << m_blacklistIP.size());

  unsigned int counter = 0;
  for (auto it = m_blacklistIP.begin(); it != m_blacklistIP.end();) {
    if (counter < num_to_pop) {
      it = m_blacklistIP.erase(it);
      counter++;
    } else {
      break;
    }
  }

  LOG_GENERAL(INFO, "Removed " << counter << " nodes from blacklist");
}

unsigned int Blacklist::SizeOfBlacklist() {
  lock_guard<mutex> g(m_mutexBlacklistIP);
  return m_blacklistIP.size();
}

void Blacklist::Enable(const bool enable) {
  if (!enable) {
    Clear();
  }

  m_enabled = enable;
}

bool Blacklist::Whitelist(const uint128_t& ip) {
  if (!m_enabled) {
    return false;
  }
  lock_guard<mutex> g(m_mutexBlacklistIP);
  return m_whitelistedIP.emplace(ip).second;
}

bool Blacklist::RemoveFromWhitelist(const uint128_t& ip) {
  if (!m_enabled) {
    return false;
  }
  lock_guard<mutex> g(m_mutexBlacklistIP);
  return (m_whitelistedIP.erase(ip) > 0);
}

bool Blacklist::IsWhitelistedIP(const uint128_t& ip) {
  lock_guard<mutex> g(m_mutexBlacklistIP);
  return m_whitelistedIP.end() != m_whitelistedIP.find(ip);
}
