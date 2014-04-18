// Copyright (C) 2013  Internet Systems Consortium, Inc. ("ISC")
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
// REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
// INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
// LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
// OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.

#ifndef CALLOUT_HANDLE_STORE_H
#define CALLOUT_HANDLE_STORE_H

#include <hooks/hooks_manager.h>
#include <hooks/callout_handle.h>

namespace bundy {
namespace dhcp {

/// @brief CalloutHandle Store
///
/// When using the Hooks Framework, there is a need to associate an
/// bundy::hooks::CalloutHandle object with each request passing through the
/// server.  For the DHCP servers, the association is provided by this function.
///
/// The DHCP servers process a single request at a time. At points where the
/// CalloutHandle is required, the pointer to the current request (packet) is
/// passed to this function.  If the request is a new one, a pointer to
/// the request is stored, a new CalloutHandle is allocated (and stored) and
/// a pointer to the latter object returned to the caller.  If the request
/// matches the one stored, the pointer to the stored CalloutHandle is
/// returned.
///
/// A special case is a null pointer being passed.  This has the effect of
/// clearing the stored pointers to the packet being processed and
/// CalloutHandle.  As the stored pointers are shared pointers, clearing them
/// removes one reference that keeps the pointed-to objects in existence.
///
/// @note If the behaviour of the server changes so that multiple packets can
///       be active at the same time, this simplistic approach will no longer
///       be adequate and a more complicated structure (such as a map) will
///       be needed.
///
/// @param pktptr Pointer to the packet being processed.  This is typically a
///        Pkt4Ptr or Pkt6Ptr object.  An empty pointer is passed to clear
///        the stored pointers.
///
/// @return Shared pointer to a CalloutHandle.  This is the previously-stored
///         CalloutHandle if pktptr points to a packet that has been seen
///         before or a new CalloutHandle if it points to a new one.  An empty
///         pointer is returned if pktptr is itself an empty pointer.

template <typename T>
bundy::hooks::CalloutHandlePtr getCalloutHandle(const T& pktptr) {

    // Stored data is declared static, so is initialized when first accessed
    static T stored_pointer;                // Pointer to last packet seen
    static bundy::hooks::CalloutHandlePtr stored_handle;
                                            // Pointer to stored handle

    if (pktptr) {

        // Pointer given, have we seen it before? (If we have, we don't need to
        // do anything as we will automatically return the stored handle.)
        if (pktptr != stored_pointer) {

            // Not seen before, so store the pointer passed to us and get a new
            // CalloutHandle.  (The latter operation frees and probably deletes
            // (depending on other pointers) the stored one.)
            stored_pointer = pktptr;
            stored_handle = bundy::hooks::HooksManager::createCalloutHandle();
        }
        
    } else {

        // Empty pointer passed, clear stored data
        stored_pointer.reset();
        stored_handle.reset();
    }

    return (stored_handle);
}

} // namespace shcp
} // namespace bundy

#endif // CALLOUT_HANDLE_STORE_H
