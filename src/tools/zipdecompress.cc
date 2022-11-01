#include "zipdecompress.h"
#include "zlib.h"

int ZipDecompress(uint8_t* stream, size_t stream_size, std::vector<uint8_t>& outbuffer)
{
	// Some settings
	constexpr uint32_t reserved = 5 * 1024 * 1024;
	constexpr uint32_t chunk = 10 * 1024 * 1024;
	uint8_t* cursor = stream;

	z_stream strm;
	int ret;
	uint32_t have;
	uint32_t remain;

	/* allocate inflate state */
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	ret = inflateInit2(&strm, -MAX_WBITS);
	if (ret != Z_OK)
		return ret;

	outbuffer.resize(reserved);
	/* decompress until deflate stream ends or end of file */
	do
	{
		remain = stream_size - (cursor - stream);
		strm.avail_in = remain >= chunk ? chunk : remain;

		if (strm.avail_in == 0)
			break;
		strm.next_in = cursor;
		cursor += strm.avail_in;

		/* run inflate() on input until output buffer not full */
		do
		{
			strm.avail_out = chunk;
			if (outbuffer.size() < (strm.total_out + strm.avail_out))
				outbuffer.resize(strm.total_out + strm.avail_out);
			strm.next_out = outbuffer.data() + strm.total_out;

			ret = inflate(&strm, Z_NO_FLUSH);
			// assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
			switch (ret)
			{
			case Z_NEED_DICT:
				ret = Z_DATA_ERROR; /* and fall through */
			case Z_DATA_ERROR:
			case Z_MEM_ERROR:
				(void)inflateEnd(&strm);
				return ret;
			}
		} while (strm.avail_out == 0);

		/* done when inflate() says it's done */
	} while (ret != Z_STREAM_END);

    outbuffer.resize(strm.total_out);
	/* clean up and return */
	(void)inflateEnd(&strm);
	return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}