// Copyright (C) 2014  Internet Systems Consortium, Inc. ("ISC")
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

// BEGIN_HEADER_GUARD

#include <stdint.h>

#include <dns/name.h>
#include <dns/rdata.h>

#include <string>
#include <vector>

// BEGIN_BUNDY_NAMESPACE

// BEGIN_COMMON_DECLARATIONS
// END_COMMON_DECLARATIONS

// BEGIN_RDATA_NAMESPACE

struct TLSAImpl;

class TLSA : public Rdata {
public:
    // BEGIN_COMMON_MEMBERS
    // END_COMMON_MEMBERS

    TLSA(uint8_t certificate_usage, uint8_t selector,
         uint8_t matching_type, const std::string& certificate_assoc_data);
    TLSA& operator=(const TLSA& source);
    ~TLSA();

    ///
    /// Specialized methods
    ///
    uint8_t getCertificateUsage() const;
    uint8_t getSelector() const;
    uint8_t getMatchingType() const;
    const std::vector<uint8_t>& getData() const;
    size_t getDataLength() const;

private:
    TLSAImpl* constructFromLexer(MasterLexer& lexer);

    TLSAImpl* impl_;
};

// END_RDATA_NAMESPACE
// END_BUNDY_NAMESPACE
// END_HEADER_GUARD

// Local Variables:
// mode: c++
// End:
