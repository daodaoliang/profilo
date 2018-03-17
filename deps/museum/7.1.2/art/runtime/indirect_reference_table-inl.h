/*
 * Copyright (C) 2014 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ART_RUNTIME_INDIRECT_REFERENCE_TABLE_INL_H_
#define ART_RUNTIME_INDIRECT_REFERENCE_TABLE_INL_H_

#include <museum/7.1.2/art/runtime/indirect_reference_table.h>

#include <museum/7.1.2/art/runtime/gc_root-inl.h>
#include <museum/7.1.2/art/runtime/runtime-inl.h>
#include <museum/7.1.2/art/runtime/verify_object-inl.h>

namespace facebook { namespace museum { namespace MUSEUM_VERSION { namespace art {
namespace mirror {
class Object;
}  // namespace mirror

// Verifies that the indirect table lookup is valid.
// Returns "false" if something looks bad.
inline bool IndirectReferenceTable::GetChecked(IndirectRef iref) const {
  if (UNLIKELY(iref == nullptr)) {
    LOG(WARNING) << "Attempt to look up nullptr " << kind_;
    return false;
  }
  if (UNLIKELY(GetIndirectRefKind(iref) == kHandleScopeOrInvalid)) {
    AbortIfNoCheckJNI(StringPrintf("JNI ERROR (app bug): invalid %s %p",
                                   GetIndirectRefKindString(kind_),
                                   iref));
    return false;
  }
  const int topIndex = segment_state_.parts.topIndex;
  int idx = ExtractIndex(iref);
  if (UNLIKELY(idx >= topIndex)) {
    std::string msg = StringPrintf(
        "JNI ERROR (app bug): accessed stale %s %p  (index %d in a table of size %d)",
        GetIndirectRefKindString(kind_),
        iref,
        idx,
        topIndex);
    AbortIfNoCheckJNI(msg);
    return false;
  }
  if (UNLIKELY(table_[idx].GetReference()->IsNull())) {
    AbortIfNoCheckJNI(StringPrintf("JNI ERROR (app bug): accessed deleted %s %p",
                                   GetIndirectRefKindString(kind_),
                                   iref));
    return false;
  }
  if (UNLIKELY(!CheckEntry("use", iref, idx))) {
    return false;
  }
  return true;
}

// Make sure that the entry at "idx" is correctly paired with "iref".
inline bool IndirectReferenceTable::CheckEntry(const char* what, IndirectRef iref, int idx) const {
  IndirectRef checkRef = ToIndirectRef(idx);
  if (UNLIKELY(checkRef != iref)) {
    std::string msg = StringPrintf(
        "JNI ERROR (app bug): attempt to %s stale %s %p (should be %p)",
        what,
        GetIndirectRefKindString(kind_),
        iref,
        checkRef);
    AbortIfNoCheckJNI(msg);
    return false;
  }
  return true;
}

template<ReadBarrierOption kReadBarrierOption>
inline mirror::Object* IndirectReferenceTable::Get(IndirectRef iref) const {
  if (!GetChecked(iref)) {
    return nullptr;
  }
  uint32_t idx = ExtractIndex(iref);
  mirror::Object* obj = table_[idx].GetReference()->Read<kReadBarrierOption>();
  VerifyObject(obj);
  return obj;
}

inline void IndirectReferenceTable::Update(IndirectRef iref, mirror::Object* obj) {
  if (!GetChecked(iref)) {
    LOG(WARNING) << "IndirectReferenceTable Update failed to find reference " << iref;
    return;
  }
  uint32_t idx = ExtractIndex(iref);
  table_[idx].SetReference(obj);
}

} } } } // namespace facebook::museum::MUSEUM_VERSION::art

#endif  // ART_RUNTIME_INDIRECT_REFERENCE_TABLE_INL_H_
