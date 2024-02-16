// Copyright (c) Meta Platforms, Inc. and affiliates.
#pragma once

#include "Python.h"
#include "cinderx/Common/ref.h"

#include "cinderx/Jit/deopt_patcher.h"
#include "cinderx/Jit/runtime.h"

#include <variant>

namespace jit {

// Patch a DeoptPatchpoint when the given PyTypeObject changes at all. This
// should only be used (instead of a more specific subclass) in cases where it
// is impossible to check the property we care about in maybePatch() (e.g., if
// the change to the type happens after PyType_Modified() is called).
class TypeDeoptPatcher : public DeoptPatcher {
 public:
  TypeDeoptPatcher(BorrowedRef<PyTypeObject> type);

  virtual bool maybePatch(BorrowedRef<PyTypeObject> new_ty);

 protected:
  void init() override;

  BorrowedRef<PyTypeObject> type_;
};

// Patch a DeoptPatchpoint when the given PyTypeObject no longer has the given
// PyObject* at the specified name.
class TypeAttrDeoptPatcher : public TypeDeoptPatcher {
 public:
  TypeAttrDeoptPatcher(
      BorrowedRef<PyTypeObject> type,
      BorrowedRef<PyUnicodeObject> attr_name,
      BorrowedRef<> target_object);

  bool maybePatch(BorrowedRef<PyTypeObject> new_ty) override;

 private:
  Ref<PyUnicodeObject> attr_name_;
  Ref<> target_object_;
};

class SplitDictDeoptPatcher : public TypeDeoptPatcher {
 public:
  SplitDictDeoptPatcher(
      BorrowedRef<PyTypeObject> type,
      BorrowedRef<PyUnicodeObject> attr_name,
      PyDictKeysObject* keys);

  bool maybePatch(BorrowedRef<PyTypeObject> new_ty) override;

 private:
  Ref<PyUnicodeObject> attr_name_;

  // We don't need to hold a strong reference to keys_ like we do for
  // attr_name_ because calls to PyTypeModified() happen before the old keys
  // object is decrefed.
  PyDictKeysObject* keys_;
};

} // namespace jit
