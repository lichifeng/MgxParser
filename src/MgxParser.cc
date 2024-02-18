/***************************************************************
 * \file       MgxParser.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/7
 * \copyright  Copyright (c) 2020-2024
 ***************************************************************/

#include <string>
#include <utility>
#include "include/MgxParser.h"
#include "analyzers/default/analyzer.h"

static std::string _parse(DefaultAnalyzer &&a, MgxParser::Settings &s)
{
    if (s.full_parse)
    {
        try
        {
            a.calc_md5_ = s.calc_md5;
            if (!s.unzip.empty())
                a.unzip_ = s.unzip;
            a.unzip_buffer_ = s.unzip_buffer;
            a.unzip_size_ptr_ = s.unzip_size_ptr;
            a.Run();
        }
        catch (const std::string &msg)
        {
            a.logger_->Fatal("Df#{}@{}/{}: {}", a.status_.debug_flag_, a.Position(), a.TotalSize(), msg);
        }
        catch (const std::exception &e)
        {
            a.logger_->Fatal("Df#{}@{}/{}: {}", a.status_.debug_flag_, a.Position(), a.TotalSize(), e.what());
        }
        catch (...)
        {
            a.logger_->Fatal("Df#{}@{}/{}: Unknown Exception!", a.status_.debug_flag_, a.Position(), a.TotalSize());
        }
        a.parse_time_ = a.logger_->Elapsed();
    }

    if (s.map_type != MgxParser::NO_MAP)
    {
        a.map_type_ = s.map_type;
        try
        {
            if (s.map_dest)
            {
                a.DrawMap(s.map_dest, s.map_width, s.map_height);
            }
            else if (s.map_type != MgxParser::BASE64_NORMAL && s.map_type != MgxParser::BASE64_HD)
            {
                a.DrawMap(s.map_path.empty() ? "minimap.png" : s.map_path, s.map_width, s.map_height);
            }
            // if base64 is required, json output will handle it.
        }
        catch (...)
        {
            a.logger_->Warn("Failed to generate a map file.");
        }
    }

    if (!s.header_path.empty() || !s.body_path.empty())
    {
        a.Extract2Files(s.header_path, s.body_path);
    }

    a.message_ = std::move(a.logger_->DumpStr());

    return a.JsonOutput(s.json_indent);
}

std::string MgxParser::parse(MgxParser::Settings &s)
{
    if (s.input_stream && s.input_size > 0)
    {
        return _parse(DefaultAnalyzer(s.input_stream, s.input_size, s.input_path), s);
    }
    else
    {
        return _parse(DefaultAnalyzer(s.input_path), s);
    }
}