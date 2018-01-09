#pragma once


#include <string>


class malformed_tar_error : public std::exception {

	private:

		std::string _msg;

	public:

		malformed_tar_error (const std::string str) : _msg() {
			_msg = "malformed_tar_error: ";
			_msg += str;
		}

		const char* what () const noexcept {
			return _msg.c_str();
		}
};


class passthrough_error : public std::exception {

	private:

		std::string _msg;

	public:

		passthrough_error (const std::string str) : _msg() {
			_msg = "passthrough_error: ";
			_msg += str;
		}

		const char* what () const noexcept {
			return _msg.c_str();
		}
};


class system_error : public std::exception {

	private:

		std::string _msg;

	public:

		system_error (const std::string str) : _msg(str) {
			_msg = "system_error: " + _msg;
		}

		const char* what () const noexcept {
			return _msg.c_str();
		}
};
