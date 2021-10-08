// Copyright (c) 2008, Google Inc.
// All rights reserved.

// ---
// This file is a compatibility layer that defines Google's version of
// command line flags that are used for configuration.
//
// We put flags into their own namespace.  It is purposefully
// named in an opaque way that people should have trouble typing
// directly.  The idea is that DEFINE puts the flag in the weird
// namespace, and DECLARE imports the flag from there into the
// current namespace.  The net result is to force people to use
// DECLARE to get access to a flag, rather than saying
//   extern bool FLAGS_logtostderr;
// or some such instead.  We want this so we can put extra
// functionality (like sanity-checking) in DECLARE if we want,
// and make sure it is picked up everywhere.
//
// We also put the type of the variable in the namespace, so that
// people can't DECLARE_int32 something that they DEFINE_bool'd
// elsewhere.

#pragma once

#include <cstdint>
#include <cstdlib>
#include <string>

#define DECLARE_VARIABLE(type, shorttype, name, tn)                 \
  namespace fL##shorttype {                                         \
  extern type FLAGS_##name;                                         \
    }                                                               \
  using fL##shorttype::FLAGS_##name

#define DEFINE_VARIABLE(type, shorttype, name, value, meaning, tn)  \
  namespace fL##shorttype {                                         \
    type FLAGS_##name(value);                                       \
    char FLAGS_no##name;                                            \
        }                                                           \
  using fL##shorttype::FLAGS_##name

// bool specialization
#define DECLARE_bool(name)          DECLARE_VARIABLE(bool, B, name, bool)
#define DEFINE_bool(name, val, txt) DEFINE_VARIABLE(bool, B, name, val, txt, bool)
  

#define DECLARE_int32(name)         DECLARE_VARIABLE(int32_t, I, name, int32_t)
#define DEFINE_int32(name,val,txt)  DEFINE_VARIABLE(int32_t, I, name, val, txt, int32_t)

#define DECLARE_int64(name)         DECLARE_VARIABLE(int64_t, I64, name, int64_t)
#define DEFINE_int64(name,val,txt)  DEFINE_VARIABLE(int64_t, I64, name, val, txt, int64_t)

#define DECLARE_double(name)        DECLARE_VARIABLE(double, D, name, double)
#define DEFINE_double(name,val,txt) DEFINE_VARIABLE(double, D, name, val, txt, double)

// Special case for string, because we have to specify the namespace
// std::string, which doesn't play nicely with our FLAG__namespace hackery.
#define DECLARE_string(name)                                        \
  namespace fLS {                                                   \
    extern std::string& FLAGS_##name;                               \
        }                                                           \
  using fLS::FLAGS_##name

#define DEFINE_string(name, value, meaning)                         \
  namespace fLS {                                                   \
    std::string FLAGS_##name##_buf(value);                          \
    std::string& FLAGS_##name = FLAGS_##name##_buf;                 \
    char FLAGS_no##name;                                            \
        }                                                           \
  using fLS::FLAGS_##name


// If both an environment variable and a flag are specified, the value
// specified by a flag wins. 

#define QSF_DEFINE_bool(name, value, meaning) \
  DEFINE_bool(name, EnvToBool("QSF_" #name, value), meaning)

#define QSF_DEFINE_int32(name, value, meaning) \
  DEFINE_int32(name, EnvToInt("QSF_" #name, value), meaning)

#define QSF_DEFINE_string(name, value, meaning) \
  DEFINE_string(name, EnvToString("QSF_" #name, value), meaning)

// These macros (could be functions, but I don't want to bother with a .cc
// file), make it easier to initialize flags from the environment.

#define EnvToString(envname, dflt)   \
  (!getenv(envname) ? (dflt) : getenv(envname))

#define EnvToBool(envname, dflt)   \
  (!getenv(envname) ? (dflt) : memchr("tTyY1\0", getenv(envname)[0], 6) != NULL)

#define EnvToInt(envname, dflt)  \
  (!getenv(envname) ? (dflt) : strtol(getenv(envname), NULL, 10))
