#include <string>

class Status
{
public:
	bool input_loaded_ = false;
	bool stream_extracted_ = false;

	inline bool invalid() { return !input_loaded_; }
};