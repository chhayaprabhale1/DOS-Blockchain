#pragma once

namespace dp {

  /* provides access to the OpenSSL random number generator */
  void rand_bytes(char* buf, int count);
  void rand_pseudo_bytes(char* buf, int count);
} // namespace dp
