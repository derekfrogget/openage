// Copyright 2013-2023 the openage authors. See copying.md for legal info.

#include "error.h"

#include <utility>

#include "../util/compiler.h"

#include "stackanalyzer.h"

namespace openage::error {


constexpr const char *runtime_error_message = "polymorphic openage Error object; catch by reference!";

static bool enable_break_on_create = false;
void Error::debug_break_on_create(bool state) {
	enable_break_on_create = state;
}

Error::Error(const log::message &msg, bool generate_backtrace, bool store_cause)
	:
	std::runtime_error{runtime_error_message},
	msg(msg) {

	if (enable_break_on_create) [[unlikely]] {
		BREAKPOINT;
	}

	if (generate_backtrace) {
		auto backtrace = std::make_shared<StackAnalyzer>();
		backtrace->analyze();
		this->backtrace = backtrace;
	}

	if (store_cause) {
		this->store_cause();
	}
}


void Error::store_cause() {
	// we could simply do this->cause = std::current_exception(),
	// but we need to trim the cause Error's backtrace.

	if (!std::current_exception()) [[likely]] {
		return;
	}

	try {
		throw;
	} catch (Error &cause) {
		cause.trim_backtrace();
		this->cause = std::current_exception();
	} catch (...) {
		this->cause = std::current_exception();
	}
}


void Error::trim_backtrace() {
	if (this->backtrace) {
		this->backtrace->trim_to_current_stack_frame();
	}
}


Error::Error()
	:
	std::runtime_error{runtime_error_message} {}


const char *Error::what() const noexcept {
	return this->msg.text.c_str();
}


std::string Error::type_name() const {
	return util::typestring(*this);
}


void Error::rethrow_cause() const {
	if (this->cause) {
		std::rethrow_exception(this->cause);
	}
}


std::ostream &operator <<(std::ostream &os, const Error &e) {
	// output the exception cause
	bool had_a_cause = true;
	try {
		e.rethrow_cause();
		had_a_cause = false;
	} catch (Error &cause) {
		os << cause << std::endl;
	} catch (std::exception &cause) {
		os << util::typestring(cause) << ": " << cause.what() << std::endl;
	} catch (...) {
		os << "unknown non std::exception cause!" << std::endl;
	}

	if (had_a_cause) {
		os << std::endl << "The above exception was the direct cause of the following exception:" << std::endl << std::endl;
	}

	// output the exception backtrace
	if (e.backtrace) {
		os << *e.backtrace;
	} else {
		os << "origin:" << std::endl;
	}

	// the exception message metadata also holds some "backtrace-like" info
	os << backtrace_symbol{
		e.msg.filename,
		e.msg.lineno,
		e.msg.functionname,
		nullptr} << std::endl;

	os << e.type_name();

	if (not e.msg.text.empty()) {
		os << ": " << e.msg.text;
	}

	return os;
}


} // openage::error
