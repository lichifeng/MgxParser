/***************************************************************
 * \file       mainproc_extractstreams.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/7
 * \copyright  Copyright (c) 2020-2022
 ***************************************************************/

#include <array>
#include <sstream>

#include "analyzer.h"
#include "searcher.h"
#include "zipdecompress.h"

bool DefaultAnalyzer::ExtractStreams() {
    // Check status
    if (StreamReady())
        return true;
    if (!status_.input_loaded_)
        throw std::string(message_);

    // Is this a zip archive? The first 4 bytes should be 50 4B 03 04,
    // that is 67324752 for uint32_t.
    // Header of a zip file. https://docs.fileformat.com/compression/zip/
    if (input_stream_.empty() && !input_cursor_)
        throw std::string("No available input.");

    auto input_start = input_stream_.empty() ? input_cursor_ : input_stream_.data();
    auto input_end = input_stream_.empty() ? (input_cursor_ + input_size_) : &*input_stream_.cend();
    bool is_zip = 67324752 == *(uint32_t *)input_start;
    uint32_t *compressed_size_p;
    uint16_t *namelen_p;
    uint16_t *exlen_p;
    uint16_t *raw_msdos_time_p;
    uint16_t *raw_msdos_date_p;

    if (is_zip) {
        compressed_size_p = (uint32_t *)(input_start + 18);
        namelen_p = (uint16_t *)(input_start + 26);
        exlen_p = (uint16_t *)(input_start + 28);

        // get modified time of file, https://groups.google.com/g/comp.os.msdos.programmer/c/ffAVUFN2NbA
        raw_msdos_time_p = (uint16_t *)(input_start + 10);
        raw_msdos_date_p = (uint16_t *)(input_start + 12);
        int year, mon, day, hour, minute, sec;
        year = (int)(((*raw_msdos_date_p) >> 9) & 0x7f) + 1980;
        mon = ((*raw_msdos_date_p) >> 5) & 0x0f;
        day = (*raw_msdos_date_p) & 0x1f;
        hour = (*raw_msdos_time_p) >> 11;
        minute = ((*raw_msdos_time_p) >> 5) & 0x3f;
        sec = (*raw_msdos_time_p) & 0x1f * 2;
        std::stringstream iso_time_str;
        iso_time_str << std::setfill ('0') << std::setw(4) << year << '-' 
                     << std::setw(2) << mon << '-' 
                     << std::setw(2) << day << 'T' 
                     << std::setw(2) << hour << ':' 
                     << std::setw(2) << minute << ':' 
                     << std::setw(2) << sec << ".000Z";
        modified_date_ = iso_time_str.str();

        std::vector<RECBYTE> outbuffer;
        if (0 != ZipDecompress(const_cast<uint8_t *>(input_start + 30 + *namelen_p + *exlen_p),
                               input_size_,  // zip64 have compressed_size == 0, so use input_size instead.
                                             // https://pkware.cachefly.net/webdocs/casestudies/APPNOTE.TXT
                               outbuffer))
            throw std::string("Failed to unzip input file.");

        extracted_file_ = std::string((char *)(input_start + 30), *namelen_p);
        input_stream_ = std::move(outbuffer);
        input_size_ = input_stream_.size();
        input_start = input_stream_.data();
        input_end = input_stream_.data() + input_size_;
    }

    // 再看有没有有效的header长度信息
    auto datalen = *(uint32_t *)input_start;
    auto nextpos = *(uint32_t *)(input_start + 4);
    uint32_t headerlen, headerpos;

    // 如果没有，那就尝试搜索
    if (datalen < 25000 || datalen > input_size_) {
        // not a valid header length info
        std::array<uint8_t, 4> body_sync_interval = {0xf4, 0x01, 0x00, 0x00};
        auto possible_bodystart =
            SearchPattern(input_start, input_end, body_sync_interval.cbegin(), body_sync_interval.cend());
        if (possible_bodystart == input_end)
            throw std::string("Invalid header length and cannot find body start.");
        if (possible_bodystart - input_start < 25000)
            throw std::string("Invalid header length and found an invalid body start.");

        int32_t is_multiplayer = *(int32_t *)(&possible_bodystart[0] + 4);
        int32_t reveal_map = *(int32_t *)(&possible_bodystart[0] + 12);
        bool bodystart_valid =
            (0 == is_multiplayer || 1 == is_multiplayer) && (reveal_map >= 0 || reveal_map < 10);  // reveal_map: [0, 3]
        if (!bodystart_valid)
            throw std::string("Invalid header length and found an invalid body start.");

        int32_t log_version = *(int32_t *)(&possible_bodystart[0] - 4);
        if (log_version > 0 && log_version < 10) {
            // not aok
            possible_bodystart -= 4;
            headerpos = 8;
        } else {
            headerpos = 4;
        }
        headerlen = possible_bodystart - input_start - headerpos;
    } else {
        input_size_ = input_stream_.empty() ? input_size_ : input_stream_.size();
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
        throw std::string("Error when extracting header stream.");

    // Mark start point of body stream
    body_start_ = combined_stream_.size();

    // combine header_ and body_
    combined_stream_.insert(combined_stream_.cend(), input_start + datalen, input_end);

    // Prepare cursor_
    cursor_(0);

    // Calc md5 of real record file
    if (calc_md5_)
        file_md5_ = CalcFileMd5(input_start, input_size_);

    // Unzip the record if requested
    // 这儿要注意的问题是不能放在上面解压的代码那里，因为刚解压完并不知道压缩包里这个文件是不是有效录像，所以那时候解压出来没意义
    if (is_zip) {
        if (unzip_buffer_) {
            // remember to free memory if using this!!!
            *unzip_buffer_ = (char *)malloc(*unzip_size_ptr_ = input_size_);
            memcpy(*unzip_buffer_, input_stream_.data(), input_size_);
        } else if (!unzip_.empty() && unzip_.size() > 0 && unzip_.size() < 255) {
            std::string unzip_filename =
                (unzip_ == "original" && extracted_file_.size() > 0) ? extracted_file_ : unzip_;
            std::ofstream raw(unzip_filename, std::ofstream::binary);
            raw.write((char *)input_stream_.data(), input_size_);
            raw.close();
        }
    }

    // input stream is not intended to be use after here, release memory
    std::vector<uint8_t>().swap(input_stream_);

    return status_.stream_extracted_ = true;
}