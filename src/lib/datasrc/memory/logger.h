// Copyright (C) 2012  Internet Systems Consortium, Inc. ("ISC")
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

#ifndef DATASRC_MEMORY_LOGGER_H
#define DATASRC_MEMORY_LOGGER_H

#include <log/macros.h>
#include <datasrc/memory/memory_messages.h>

/// \file datasrc/memory/logger.h
/// \brief Data Source memory library global logger
///
/// This holds the logger for the data source memory library. It is a
/// private header and should not be included in any publicly used
/// header, only in local cc files.

namespace bundy {
namespace datasrc {
namespace memory {

/// \brief The logger for this library
extern bundy::log::Logger logger;

/// \brief Trace basic operations
const int DBG_TRACE_BASIC = DBGLVL_TRACE_BASIC;

/// \brief Trace data changes and lookups as well
const int DBG_TRACE_DATA = DBGLVL_TRACE_BASIC_DATA;

/// \brief Detailed even about how the lookups happen
const int DBG_TRACE_DETAILED = DBGLVL_TRACE_DETAIL;

} // namespace memory
} // namespace datasrc
} // namespace bundy

#endif // DATASRC_MEMORY_LOGGER_H

// Local Variables:
// mode: c++
// End:
