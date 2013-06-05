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

#include <d2/d2_controller.h>
#include <d2/d2_process.h>
#include <d2/spec_config.h>

namespace isc {
namespace d2 {

DControllerBasePtr&
D2Controller::instance() {
    // If the instance hasn't been created yet, create it.  Note this method
    // must use the base class singleton instance methods.  The base class
    // must have access to the singleton in order to use it within BIND10 
    // static function callbacks.
    if (!getController()) {
        DControllerBasePtr controller_ptr(new D2Controller());
        setController(controller_ptr);
    }

    return (getController());
}

DProcessBase* D2Controller::createProcess() {
    // Instantiate and return an instance of the D2 application process. Note
    // that the process is passed the controller's io_service.
    return (new D2Process(getName().c_str(), getIOService()));
}

D2Controller::D2Controller()
    : DControllerBase(D2_MODULE_NAME) {
    // set the BIND10 spec file either from the environment or
    // use the production value.
    if (getenv("B10_FROM_BUILD")) {
        setSpecFileName(std::string(getenv("B10_FROM_BUILD")) +
            "/src/bin/d2/d2.spec");
    } else {
        setSpecFileName(D2_SPECFILE_LOCATION);
    }
}

D2Controller::~D2Controller() {
}

}; // end namespace isc::d2
}; // end namespace isc
