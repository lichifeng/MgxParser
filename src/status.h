#include <string>

class Status
{
public:
    int debug_flag_ = 0;
	bool input_loaded_ = false;
	bool stream_extracted_ = false;
    bool version_detected_ = false;

	[[nodiscard]] inline bool invalid() const { return !input_loaded_; }
};