/*
 * Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the
 * License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

#include "auth_mysql41.h"
#include <stdint.h>
#include <stdexcept>

// Avoid warnings from protobuf
#if defined __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#elif defined _MSC_VER
#pragma warning (push)
#endif

#include <boost/uuid/sha1.hpp>

#ifdef __GNUC__
#pragma GCC diagnostic pop
#elif defined _MSC_VER
#pragma warning (pop)
#endif


#define PVERSION41_CHAR '*'
#define SCRAMBLE_LENGTH 20
#define SHA1_HASH_SIZE 20

static void my_crypt(uint8_t *to, const uint8_t *s1, const uint8_t *s2, size_t len)
{
  const uint8_t *s1_end = s1 + len;

  while (s1 < s1_end)
    *to++= *s1++ ^ *s2++;
}


static void get_sha1_hash(boost::uuids::detail::sha1 &sha1, uint8_t *hash)
{
  unsigned int digest[5];
  sha1.get_digest(digest);
  for (int i = 0; i < 5; i++)
  {
    hash[i*4 + 3] = digest[i] & 0xff;
    hash[i*4 + 2] = (digest[i] >> 8) & 0xff;
    hash[i*4 + 1] = (digest[i] >> 16) & 0xff;
    hash[i*4 + 0] = (uint8_t)((digest[i] >> 24) & 0xff);  // (uint8_t) is there to keep -Wconversion happy.  For some reason it only warns about >> 24 and leaves previous 3 alone (because result includes the sign bit?)
  }
}


static std::string scramble(const std::string &scramble_data, const std::string &password)
{
  boost::uuids::detail::sha1 sha1;

  if (scramble_data.length() != SCRAMBLE_LENGTH)
    throw std::invalid_argument("Password scramble data is invalid");

  uint8_t hash_stage1[SHA1_HASH_SIZE];
  uint8_t hash_stage2[SHA1_HASH_SIZE];
  std::string result(SCRAMBLE_LENGTH, '\0');

  result.at(SCRAMBLE_LENGTH - 1) = '\0';

  /* Two stage SHA1 hash of the pwd */
  /* Stage 1: hash pwd */
  sha1.process_bytes(password.data(), password.length());
  get_sha1_hash(sha1, hash_stage1);

  /* Stage 2 : hash first stage's output. */
  sha1.reset();
  sha1.process_bytes(hash_stage1, SHA1_HASH_SIZE);
  get_sha1_hash(sha1, hash_stage2);

  /* create crypt string as sha1(message, hash_stage2) */;
  sha1.reset();
  sha1.process_bytes(scramble_data.data(), scramble_data.length());
  sha1.process_bytes(hash_stage2, SHA1_HASH_SIZE);
  get_sha1_hash(sha1, (uint8_t*)&result[0]);

  my_crypt((uint8_t*)&result[0], (const uint8_t*)&result[0], hash_stage1, SCRAMBLE_LENGTH);

  return result;
}


static char *octet2hex(char *to, const char *str, size_t len)
{
  const char *_dig_vec_upper = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  const char *str_end= str + len;
  for (; str != str_end; ++str)
  {
    *to++= _dig_vec_upper[((uint8_t) *str) >> 4];
    *to++= _dig_vec_upper[((uint8_t) *str) & 0x0F];
  }
  *to= '\0';
  return to;
}


static std::string get_password_from_salt(const std::string &hash_stage2)
{
  std::string result(2*SHA1_HASH_SIZE + 1, '\0');

  if (hash_stage2.length() != SHA1_HASH_SIZE)
    throw std::invalid_argument("Wrong size of binary hash password");

  result[0] = PVERSION41_CHAR;
  octet2hex(&result[1], &hash_stage2[0], SHA1_HASH_SIZE);

  return result;
}


std::string mysqlx::build_mysql41_authentication_response(const std::string &salt_data,
                                                  const std::string &user,
                                                  const std::string &password,
                                                  const std::string &schema)
{
  std::string data;
  std::string password_hash;

  if (password.length())
  {
    password_hash = scramble(salt_data, password);
    password_hash = get_password_from_salt(password_hash);
  }

  data.append(schema).push_back('\0'); // authz
  data.append(user).push_back('\0'); // authc
  data.append(password_hash); // pass

  return data;
}
