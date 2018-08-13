// Copyright 2018 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "gtest/gtest.h"
#include "jwt_verify_lib/verify.h"
#include "src/test_common.h"

namespace google {
namespace jwt_verify {
namespace {

// Please see jwt_generator.py and jwk_generator.py under /tools/.
// for ES256-signed jwt token and public jwk generation, respectively.
// jwt_generator.py uses ES256 private key file to generate JWT token.
// ES256 private key file can be generated by:
// $ openssl ecparam -genkey -name prime256v1 -noout -out private_key.pem
// jwk_generator.py uses ES256 public key file to generate JWK. ES256
// public key file can be generated by:
// $ openssl ec -in private_key.pem -pubout -out public_key.pem.

// ES256 private key:
// "-----BEGIN EC PRIVATE KEY-----"
// "MHcCAQEEIOyf96eKdFeSFYeHiM09vGAylz+/auaXKEr+fBZssFsJoAoGCCqGSM49"
// "AwEHoUQDQgAEEB54wykhS7YJFD6RYJNnwbWEz3cI7CF5bCDTXlrwI5n3ZsIFO8wV"
// "DyUptLYxuCNPdh+Zijoec8QTa2wCpZQnDw=="
// "-----END EC PRIVATE KEY-----"

// ES256 public key:
// "-----BEGIN PUBLIC KEY-----"
// "MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEEB54wykhS7YJFD6RYJNnwbWEz3cI"
// "7CF5bCDTXlrwI5n3ZsIFO8wVDyUptLYxuCNPdh+Zijoec8QTa2wCpZQnDw=="
// "-----END PUBLIC KEY-----"

const std::string PublicKeyJwkEC = R"(
{
  "keys": [
    {
      "kty": "EC",
      "crv": "P-256",
      "alg": "ES256",
      "kid": "abc",
      "x": "EB54wykhS7YJFD6RYJNnwbWEz3cI7CF5bCDTXlrwI5k",
      "y": "92bCBTvMFQ8lKbS2MbgjT3YfmYo6HnPEE2tsAqWUJw8"
    },
    {
      "kty": "EC",
      "crv": "P-256",
      "alg": "ES256",
      "kid": "xyz",
      "x": "EB54wykhS7YJFD6RYJNnwbWEz3cI7CF5bCDTXlrwI5k",
      "y": "92bCBTvMFQ8lKbS2MbgjT3YfmYo6HnPEE2tsAqWUJw8"
    }
  ]
}
)";

// "{"kid":"abc"}"
const std::string JwtTextEC =
    "eyJhbGciOiJFUzI1NiIsInR5cCI6IkpXVCIsImtpZCI6ImFiYyJ9.eyJpc3MiOiI2Mj"
    "g2NDU3NDE4ODEtbm9hYml1MjNmNWE4bThvdmQ4dWN2Njk4bGo3OHZ2MGxAZGV2ZWxvc"
    "GVyLmdzZXJ2aWNlYWNjb3VudC5jb20iLCJzdWIiOiI2Mjg2NDU3NDE4ODEtbm9hYml1"
    "MjNmNWE4bThvdmQ4dWN2Njk4bGo3OHZ2MGxAZGV2ZWxvcGVyLmdzZXJ2aWNlYWNjb3V"
    "udC5jb20iLCJhdWQiOiJodHRwOi8vbXlzZXJ2aWNlLmNvbS9teWFwaSJ9.T2KAwChqg"
    "o2ZSXyLh3IcMBQNSeRZRe5Z-MUDl-s-F99XGoyutqA6lq8bKZ6vmjZAlpVG8AGRZW9J"
    "Gp9lq3cbEw";

// "{"kid":"abcdef"}"
const std::string JwtTextWithNonExistKidEC =
    "eyJhbGciOiJFUzI1NiIsInR5cCI6IkpXVCIsImtpZCI6ImFiY2RlZiJ9.eyJpc3MiOi"
    "I2Mjg2NDU3NDE4ODEtbm9hYml1MjNmNWE4bThvdmQ4dWN2Njk4bGo3OHZ2MGxAZ"
    "GV2ZWxvcGVyLmdzZXJ2aWNlYWNjb3VudC5jb20iLCJzdWIiOiI2Mjg2NDU3NDE4"
    "ODEtbm9hYml1MjNmNWE4bThvdmQ4dWN2Njk4bGo3OHZ2MGxAZGV2ZWxvcGVyLmd"
    "zZXJ2aWNlYWNjb3VudC5jb20iLCJhdWQiOiJodHRwOi8vbXlzZXJ2aWNlLmNvbS"
    "9teWFwaSJ9.rWSoOV5j7HxHc4yVgZEZYUSgY7AUarG3HxdfPON1mw6II_pNUsc8"
    "_sVf7Yv2-jeVhmf8BtR99wnOwEDhVYrVpQ";

const std::string JwtTextECNoKid =
    "eyJhbGciOiJFUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiI2Mjg2NDU3NDE4ODEtbm"
    "9hYml1MjNmNWE4bThvdmQ4dWN2Njk4bGo3OHZ2MGxAZGV2ZWxvcGVyLmdzZXJ2a"
    "WNlYWNjb3VudC5jb20iLCJzdWIiOiI2Mjg2NDU3NDE4ODEtbm9hYml1MjNmNWE4"
    "bThvdmQ4dWN2Njk4bGo3OHZ2MGxAZGV2ZWxvcGVyLmdzZXJ2aWNlYWNjb3VudC5"
    "jb20iLCJhdWQiOiJodHRwOi8vbXlzZXJ2aWNlLmNvbS9teWFwaSJ9.zlFcET8Fi"
    "OYcKe30A7qOD4TIBvtb9zIVhDcM8pievKs1Te-UOBcklQxhwXMnRSSEBY4P0pfZ"
    "qWJT_V5IVrKrdQ";

class VerifyJwkECTest : public testing::Test {
 protected:
  void SetUp() {
    jwks_ = Jwks::createFrom(PublicKeyJwkEC, Jwks::Type::JWKS);
    EXPECT_EQ(jwks_->getStatus(), Status::Ok);
  }

  JwksPtr jwks_;
};

TEST_F(VerifyJwkECTest, KidOK) {
  Jwt jwt;
  EXPECT_EQ(jwt.parseFromString(JwtTextEC), Status::Ok);
  EXPECT_EQ(verifyJwt(jwt, *jwks_), Status::Ok);

  fuzzJwtSignature(jwt, [this](const Jwt& jwt) {
    EXPECT_EQ(verifyJwt(jwt, *jwks_), Status::JwtVerificationFail);
  });
}

TEST_F(VerifyJwkECTest, NoKidOK) {
  Jwt jwt;
  EXPECT_EQ(jwt.parseFromString(JwtTextECNoKid), Status::Ok);
  EXPECT_EQ(verifyJwt(jwt, *jwks_), Status::Ok);

  fuzzJwtSignature(jwt, [this](const Jwt& jwt) {
    EXPECT_EQ(verifyJwt(jwt, *jwks_), Status::JwtVerificationFail);
  });
}

TEST_F(VerifyJwkECTest, NonExistKidFail) {
  Jwt jwt;
  EXPECT_EQ(jwt.parseFromString(JwtTextWithNonExistKidEC), Status::Ok);
  EXPECT_EQ(verifyJwt(jwt, *jwks_), Status::JwksKidAlgMismatch);
}

TEST_F(VerifyJwkECTest, PubkeyNoAlgOK) {
  // Remove "alg" claim from public key.
  std::string alg_claim = R"("alg": "ES256",)";
  std::string pubkey_no_alg = PublicKeyJwkEC;
  std::size_t alg_pos = pubkey_no_alg.find(alg_claim);
  while (alg_pos != std::string::npos) {
    pubkey_no_alg.erase(alg_pos, alg_claim.length());
    alg_pos = pubkey_no_alg.find(alg_claim);
  }

  jwks_ = Jwks::createFrom(pubkey_no_alg, Jwks::Type::JWKS);
  EXPECT_EQ(jwks_->getStatus(), Status::Ok);

  Jwt jwt;
  EXPECT_EQ(jwt.parseFromString(JwtTextEC), Status::Ok);
  EXPECT_EQ(verifyJwt(jwt, *jwks_), Status::Ok);
}

TEST_F(VerifyJwkECTest, PubkeyNoKidOK) {
  // Remove "kid" claim from public key.
  std::string kid_claim1 = R"("kid": "abc",)";
  std::string kid_claim2 = R"("kid": "xyz",)";
  std::string pubkey_no_kid = PublicKeyJwkEC;
  std::size_t kid_pos = pubkey_no_kid.find(kid_claim1);
  pubkey_no_kid.erase(kid_pos, kid_claim1.length());
  kid_pos = pubkey_no_kid.find(kid_claim2);
  pubkey_no_kid.erase(kid_pos, kid_claim2.length());

  jwks_ = Jwks::createFrom(pubkey_no_kid, Jwks::Type::JWKS);
  EXPECT_EQ(jwks_->getStatus(), Status::Ok);

  Jwt jwt;
  EXPECT_EQ(jwt.parseFromString(JwtTextEC), Status::Ok);
  EXPECT_EQ(verifyJwt(jwt, *jwks_), Status::Ok);
}

}  // namespace
}  // namespace jwt_verify
}  // namespace google
