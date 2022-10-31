#include "analyzer.h"

bool DefaultAnalyzer::ExtractStreams()
{
	// Is this a zip archive? The first 4 bytes should be 50 4B 03 04,
	// that is 67324752 for uint32_t.
	const uint32_t* zipsig = reinterpret_cast<const uint32_t*>(input_cursor_);
	if (67324752 == *zipsig)
	{

	}

	// 再看有没有有效的header长度信息
	// 如果没有，那就尝试搜索

	// 如果有长度信息或者搜索到了，就解压header

	// 再把body拼在后面

	// 如果成功获取数据，就返回true
}