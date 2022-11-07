#include <array>
#include "analyzer.h"
#include "zipdecompress.h"
#include "searcher.h"

bool DefaultAnalyzer::ExtractStreams() {
    // Is this a zip archive? The first 4 bytes should be 50 4B 03 04,
    // that is 67324752 for uint32_t.
    // Header of a zip file. https://docs.fileformat.com/compression/zip/
    if (input_stream_.empty() && !input_cursor_)
        throw std::string("No available input.");
    auto input_start = input_stream_.empty() ? input_cursor_ : input_stream_.data();
    auto input_end = input_stream_.empty() ? (input_cursor_ + input_size_) : &*input_stream_.cend();
    auto zipsig_p = (uint32_t *) input_start;
    uint32_t *compressed_size_p;
    uint16_t *namelen_p;
    uint16_t *exlen_p;

    if (67324752 == *zipsig_p) {
        compressed_size_p = (uint32_t *) (input_start + 18);
        namelen_p = (uint16_t *) (input_start + 26);
        exlen_p = (uint16_t *) (input_start + 28);

        vector<RECBYTE> outbuffer;
        if (0 != ZipDecompress(
                const_cast<uint8_t *>(input_start + 30 + *namelen_p + *exlen_p),
                *compressed_size_p,
                outbuffer))
            throw "Failed to unzip input file.";

        extracted_file_ = std::string((char *) (input_start + 30), *namelen_p);
        input_stream_ = std::move(outbuffer);
        input_size_ = input_stream_.size();
        input_start = input_stream_.data();
        input_end = input_stream_.data() + input_size_;
    }

    // 再看有没有有效的header长度信息
    auto datalen = *(uint32_t *) input_start;
    auto nextpos = *(uint32_t *) (input_start + 4);
    uint32_t headerlen, headerpos;

    // 如果没有，那就尝试搜索
    if (datalen < 25000 || datalen > input_size_) {
        // not a valid header length info
        std::array<uint8_t, 4> body_sync_interval = {0xf4, 0x01, 0x00, 0x00};
        auto possible_bodystart = SearchPattern(
                input_start, input_end,
                body_sync_interval.cbegin(), body_sync_interval.cend()
        );
        if (possible_bodystart == input_end)
            throw "Invalid header length and cannot find body start.";
        if (possible_bodystart - input_start < 25000)
            throw "Invalid header length and found an invalid body start.";

        int32_t is_multiplayer = *(int32_t *) (&possible_bodystart[0] + 4);
        int32_t reveal_map = *(int32_t *) (&possible_bodystart[0] + 12);
        bool bodystart_valid =
                (0 == is_multiplayer || 1 == is_multiplayer)
                && (0 == reveal_map || 1 == reveal_map);
        if (!bodystart_valid)
            throw "Invalid header length and found an invalid body start.";

        int32_t log_version = *(int32_t *) (&possible_bodystart[0] - 4);
        if (log_version > 0 && log_version < 10) {
            // not aok
            possible_bodystart -= 4;
            headerpos = 8;
        } else {
            headerpos = 4;
        }
        headerlen = possible_bodystart - input_start - headerpos;
    } else {
        input_size_ = input_stream_.size() ? input_stream_.size() : input_size_;
        if (nextpos < input_size_) {
            headerpos = 8;
        } else {
            headerpos = 4;
            version_code_ = AOK;
        }
        headerlen = datalen - headerpos;
    }

    // 如果有长度信息或者搜索到了，就解压header
    if (0 != ZipDecompress(const_cast<uint8_t *>(input_start + headerpos), headerlen, combined_stream_))
        throw "Error when extracting header stream.";

    // Mark start point of body stream
    body_start_ = combined_stream_.size();

    // combine header_ and body_
    combined_stream_.insert(combined_stream_.cend(), input_start + datalen, input_end);

    // Prepare cursor_
    cursor_(0);

    // input stream is not intended to be use after here, release memory
    vector<uint8_t>().swap(input_stream_);

    return status_.stream_extracted_ = true;
}