#include "analyzer.h"
#include <filesystem>
#include <iterator>

bool DefaultAnalyzer::LoadFile()
{
	auto inputpath = std::filesystem::path(inputpath_);

	if (!std::filesystem::exists(inputpath))
		return false;

	input_filename_ = inputpath.filename().generic_string();
	input_size_ = filesystem::file_size(inputpath);
	if (input_size_ < MIN_SIZE)
		return false;

	input_file_.open(inputpath_, std::ios::in | std::ios::binary);

	if (!input_file_.is_open())
		return false;

	input_stream_.reserve(input_size_);
	input_stream_.insert(
		input_stream_.begin(),
		std::istream_iterator<RECBYTE>(input_file_),
		std::istream_iterator<RECBYTE>()
	);
	input_cursor_ = input_stream_.data();

	input_file_.close();

	return true;
}