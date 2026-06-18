#include <string>
#include <vector>

#include <absl/container/flat_hash_map.h>
#include <absl/status/status.h>
#include <absl/strings/str_cat.h>
#include <absl/strings/str_split.h>
#include <absl/time/clock.h>
#include <absl/time/time.h>

#undef NDEBUG
#include <cassert>

int main ()
{
  // strings: StrCat is a non-inline function defined in str_cat.cc.
  //
  std::string s (absl::StrCat ("abseil-", 20250127, "-", "ok"));
  assert (s == "abseil-20250127-ok");

  // strings: StrSplit (str_split.cc).
  //
  std::vector<std::string> v (absl::StrSplit ("a,b,c", ','));
  assert (v.size () == 3 && v[1] == "b");

  // time: Now()/UnixEpoch() (clock.cc, time.cc).
  //
  absl::Time t (absl::Now ());
  assert (t > absl::UnixEpoch ());

  // status: status.cc.
  //
  absl::Status ok (absl::OkStatus ());
  assert (ok.ok ());

  absl::Status err (absl::InvalidArgumentError ("boom"));
  assert (!err.ok () && err.message () == "boom");

  // container: flat_hash_map exercises raw_hash_set.cc.
  //
  absl::flat_hash_map<std::string, int> m;
  m["x"] = 7;
  assert (m.at ("x") == 7);
}
