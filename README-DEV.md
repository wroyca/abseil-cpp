# Developer/packaging notes for libabseil

This file documents how this build2 package is put together and, in
particular, the modifications made to the upstream Abseil source code. It is
intended for maintainers of the package, not its consumers.

## Upstream source and layout

The upstream Abseil source code is tracked as a git submodule in `upstream/`
(see `.gitmodules`). The build2 package files (buildfiles, manifests, etc.)
are kept separate and the upstream component directories under `absl/` are
overlaid into the `libabseil/absl/` package directory using directory
symlinks. For example:

    libabseil/absl/base   -> ../../upstream/absl/base
    libabseil/absl/strings -> ../../upstream/absl/strings
    ...

These directory symlinks are also declared in `.gitattributes` with
`symlink=dir` so the package can be checked out from git directly on Windows.

## Patches to upstream source

We try hard to avoid patching upstream and instead keep the build2-specific
configuration in the package's own buildfiles. Where a patch is unavoidable
(typically a compiler/platform issue that upstream has not yet fixed), it is
applied using the "manual" approach from the build2 packaging guide so that the
modification is explicit and easy to re-apply on upstream upgrades:

  https://build2.org/build2-toolchain/doc/build2-toolchain-packaging.xhtml#howto-patch-upstream-source

For each patched file `foo.cc` you will find three files next to each other:

  - `foo.cc`        — a real (non-symlink) copy of the upstream file with our
                      modifications. This is the file that is actually compiled.
  - `foo.cc.orig`   — a symlink to the unmodified upstream file. Used as the
                      baseline when (re)generating the patch.
  - `foo.cc.patch`  — the unified diff of `.orig` vs the patched copy, for
                      review and re-application.

Because the upstream `absl/` components are overlaid as *directory* symlinks,
an individual file cannot be patched in place. To patch one file we "break out"
the smallest necessary part of the tree: the affected directory and its parents
are turned into real directories. That is, every sibling entry is re-created as
a symlink into `upstream/`, and only the patched file becomes a real copy. Any
directory symlinks created by such a break-out are added to `.gitattributes`.

### `absl/time/internal/cctz/src/time_zone_lookup.cc`

**Why:** When cross-compiling to MinGW (`x86_64-w64-mingw32`), this file's
WinRT code path includes `<windows.globalization.h>`, which pulls in
`<windows.foundation.h>`. The mingw-w64 headers of the relevant generation
(observed with mingw-w64 v13) contain a bug: both `IReference<BYTE>` and
`IReference<boolean>` are emitted as explicit specializations, but `BYTE` and
`boolean` are both typedefs for `unsigned char`, so the two collide as
`IReference<unsigned char>` and the compiler reports a redefinition:

    error: redefinition of 'struct ABI::Windows::Foundation::IReference<unsigned char>'

This is a system-header bug, not an Abseil bug, and it breaks any C++
translation unit that includes `<windows.globalization.h>` under this toolchain.

**Fix:** Disable the WinRT `USE_WIN32_LOCAL_TIME_ZONE` code path for MinGW by
removing the MinGW-enabling branch from the include guard. MinGW then falls back
to the generic `${TZ}` / `/etc/localtime` resolution in `local_time_zone()`
which is the behaviour MinGW had before the WinRT path was added. See
`time_zone_lookup.cc.patch` for the exact diff.

## Upgrading to a new upstream version

1. Update the `upstream/` submodule to the new release tag and update the
   package version in `libabseil/manifest`.

2. Re-create any symlinks for files/directories that upstream added or removed
   (compare against the upstream component layout).

3. For each `*.patch` in the package, re-apply it to the new upstream source:

       $ cd libabseil/absl/time/internal/cctz/src/   # (for this patch)
       # .orig now points at the new upstream file:
       $ cp -H time_zone_lookup.cc.orig time_zone_lookup.cc
       $ patch <time_zone_lookup.cc.patch

   If a hunk no longer applies cleanly, resolve the conflict manually and then
   regenerate the patch:

       $ diff -u time_zone_lookup.cc.orig time_zone_lookup.cc >time_zone_lookup.cc.patch

4. Check whether the patched issue has been fixed upstream. If so, drop the
   patch entirely (remove the real copy, `.orig`, and `.patch`, restore the
   directory symlink if no other patched files remain in that subtree, and
   update `.gitattributes` accordingly) and update this document.

5. Build for the affected configuration(s) (in particular the MinGW
   cross-build) and run the tests before releasing.
