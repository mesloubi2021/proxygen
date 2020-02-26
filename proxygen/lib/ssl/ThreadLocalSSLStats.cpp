/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "proxygen/lib/ssl/ThreadLocalSSLStats.h"

using facebook::fb303::AVG;
using facebook::fb303::PERCENT;
using facebook::fb303::RATE;
using facebook::fb303::SUM;

namespace proxygen {

TLSSLStats::TLSSLStats(const std::string& prefix)
    : sslAcceptLatency_(
          prefix + "_ssl_accept_lat", 100, 0, 10000, AVG, 50, 95, 99),
      sslAcceptLatencyTS_(prefix + "_ssl_accept_lat", AVG),
      tlsTicketNew_(prefix + "_tls_ticket_new", SUM),
      tlsTicketHit_(prefix + "_tls_ticket_hit", SUM),
      tlsTicketMiss_(prefix + "_tls_ticket_miss", SUM),
      sslSessionNew_(prefix + "_ssl_sess_new", SUM),
      sslSessionHit_(prefix + "_ssl_sess_hit", SUM),
      sslSessionForeignHit_(prefix + "_ssl_sess_foreign_hit", SUM),
      sslSessionTotalMiss_(prefix + "_ssl_sess_total_miss", SUM),
      sslSessionRemove_(prefix + "_ssl_sess_remove", SUM),
      sslSessionFree_(prefix + "_ssl_sess_free", SUM),
      sslSessionSetError_(prefix + "_ssl_sess_error_set", SUM),
      sslSessionGetError_(prefix + "_ssl_sess_error_get", SUM),
      sslClientRenegotiations_(prefix + "_ssl_client_renegotiations", SUM),
      clientCertMismatch_(prefix + "_client_cert_mismatch", SUM),
      tlsTicketInvalidRotation_(prefix + "_tls_ticket_invalid_rotation", SUM),
      sslUpstreamHandshakes_(prefix + "_ssl_upstream_handshakes", SUM),
      sslUpstreamResumes_(prefix + "_ssl_upstream_resumes", SUM),
      sslUpstreamErrors_(prefix + "_ssl_upstream_errors", SUM),
      sslUpstreamVerifyErrors_(prefix + "_ssl_upstream_verify_errors", SUM),
      replayCacheNumRequests_(
          prefix + "_replay_cache.proxygen.external.num_requests", SUM, RATE),
      replayCacheNumHits_(prefix + "_replay_cache.proxygen.external.num_hits",
                          SUM,
                          PERCENT,
                          RATE),
      replayCacheNumErrors_(
          prefix + "_replay_cache.proxygen.external.num_errors",
          SUM,
          PERCENT,
          RATE),
      replayCacheDuration_(prefix + "_replay_cache.proxygen.external.duration",
                           100,
                           0,
                           5000,
                           AVG,
                           50,
                           95,
                           99),
      newSSLHandshakeShed_(prefix + "_ssl_handshake_shed_new", SUM, RATE),
      sslHandshakeErrors_(prefix + "_ssl_handshake_errors", SUM, PERCENT),
      sslHandshakeSuccesses_(prefix + "_ssl_handshake_successes", SUM),
      fizzHandshakeErrors_(prefix + "_fizz_handshake_errors", SUM, PERCENT),
      fizzHandshakeProtocolErrors_(prefix + "_fizz_handshake_protocol_errors",
                                   SUM),
      fizzHandshakeSuccesses_(prefix + "_fizz_handshake_successes", SUM),
      tfoSuccess_(prefix + "_tfo_success", SUM),
      fizzPskTypeNotSupported_(prefix + "_fizz_psktype_not_supported", SUM),
      fizzPskTypeNotAttempted_(prefix + "_fizz_psktype_not_attempted", SUM),
      fizzPskTypeRejected_(prefix + "_fizz_psktype_rejected", SUM),
      fizzPskTypeExternal_(prefix + "_fizz_psktype_external", SUM),
      fizzPskTypeResumption_(prefix + "_fizz_psktype_resumption", SUM) {
}

void TLSSLStats::recordSSLAcceptLatency(int64_t latency) noexcept {
  if (latency >= 0) {
    sslAcceptLatency_.add(latency);
    sslAcceptLatencyTS_.add(latency);
  }
}

void TLSSLStats::recordTLSTicket(bool ticketNew, bool ticketHit) noexcept {
  if (ticketNew) {
    tlsTicketNew_.add(1);
  } else if (ticketHit) {
    tlsTicketHit_.add(1);
  } else {
    tlsTicketMiss_.add(1);
  }
}

void TLSSLStats::recordSSLSession(bool sessionNew,
                                  bool sessionHit,
                                  bool foreign) noexcept {
  if (sessionNew) {
    sslSessionNew_.add(1);
  } else if (sessionHit) {
    sslSessionHit_.add((foreign) ? 0 : 1);
    sslSessionForeignHit_.add((foreign) ? 1 : 0);
  } else {
    sslSessionTotalMiss_.add(1);
  }
}

void TLSSLStats::recordSSLSessionRemove() noexcept {
  sslSessionRemove_.add(1);
}

void TLSSLStats::recordSSLSessionFree(uint32_t freed) noexcept {
  sslSessionFree_.add(freed);
}

void TLSSLStats::recordSSLSessionSetError(uint32_t /*err*/) noexcept {
  sslSessionSetError_.add(1);
}

void TLSSLStats::recordSSLSessionGetError(uint32_t /*err*/) noexcept {
  sslSessionGetError_.add(1);
}

void TLSSLStats::recordClientRenegotiation() noexcept {
  sslClientRenegotiations_.add(1);
}

void TLSSLStats::recordSSLClientCertificateMismatch() noexcept {
  clientCertMismatch_.add(1);
}

void TLSSLStats::recordTLSTicketRotation(bool valid) noexcept {
  if (!valid) {
    tlsTicketInvalidRotation_.add(1);
  }
}

void TLSSLStats::recordSSLUpstreamConnection(bool handshake) noexcept {
  if (handshake) {
    sslUpstreamHandshakes_.add(1);
  } else {
    sslUpstreamResumes_.add(1);
  }
}

void TLSSLStats::recordSSLUpstreamConnectionError(bool verifyError) noexcept {
  if (verifyError) {
    sslUpstreamVerifyErrors_.add(1);
  } else {
    sslUpstreamErrors_.add(1);
  }
}

void TLSSLStats::recordReplayCacheRequestComplete(uint64_t duration,
                                                  bool cacheHit) noexcept {
  replayCacheNumRequests_.add(1);
  replayCacheDuration_.add(duration);
  replayCacheNumHits_.add(cacheHit ? 1 : 0);
  replayCacheNumErrors_.add(0);
}

void TLSSLStats::recordReplayCacheRequestError() noexcept {
  replayCacheNumRequests_.add(1);
  replayCacheNumErrors_.add(1);
}

void TLSSLStats::recordNewSSLHandshakeShed() {
  newSSLHandshakeShed_.add(1);
}

void TLSSLStats::recordSSLHandshake(bool success) {
  if (success) {
    sslHandshakeSuccesses_.add(1);
    sslHandshakeErrors_.add(0);
  } else {
    sslHandshakeErrors_.add(1);
  }
}

void TLSSLStats::recordFizzHandshake(bool success) {
  if (success) {
    fizzHandshakeSuccesses_.add(1);
    fizzHandshakeErrors_.add(0);
  } else {
    fizzHandshakeErrors_.add(1);
  }
}

void TLSSLStats::recordFizzHandshakeProtocolError() {
  fizzHandshakeProtocolErrors_.add(1);
}

void TLSSLStats::recordTFOSuccess() {
  tfoSuccess_.add(1);
}

void TLSSLStats::recordPskType(folly::Optional<fizz::PskType> pskType) {
  if (!pskType.hasValue()) {
    return;
  }
  auto pskTypeVal = pskType.value();
  switch (pskTypeVal) {
    case fizz::PskType::NotSupported:
      fizzPskTypeNotSupported_.add(1);
      break;
    case fizz::PskType::NotAttempted:
      fizzPskTypeNotAttempted_.add(1);
      break;
    case fizz::PskType::Rejected:
      fizzPskTypeRejected_.add(1);
      break;
    case fizz::PskType::External:
      fizzPskTypeExternal_.add(1);
      break;
    case fizz::PskType::Resumption:
      fizzPskTypeResumption_.add(1);
      break;
  }
}

} // namespace proxygen
