/**
 * \file       test.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-27
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include <gtest/gtest.h>
#include <string>
#include <filesystem>
#include <fstream>
#include <cstdio>
#include <utility>
#include "../src/include/MgxParser.h"
#include "nlohmann_json_3/json.hpp"

using namespace std;
using json = nlohmann::json;

string genPath(string f)
{
    return (filesystem::path(__FILE__).parent_path() / "testRecords" / f).generic_string();
}

bool isPng(string f)
{
    ifstream pngf(f, ifstream::in | ifstream::binary);

    if (!pngf.is_open())
        return false;

    uint8_t pngHeader[8] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};
    uint8_t headerBuf[8];
    pngf.read((char *)headerBuf, 8);
    return 0 == memcmp(headerBuf, pngHeader, 8);
}

class ParserTest : public testing::Test
{
protected:
    void load(json &j, string f, int mapType = NO_MAP, string mapName = "testmap.png")
    {
        auto path = genPath(f);
        string rawret = MgxParser::parse(path, mapType, mapName);
        j = json::parse(rawret);
    }

    void loadBytes(json &j, const uint8_t *b, uint32_t size, int mapType = NO_MAP, string mapName = "testmap.png")
    {
        string rawret = MgxParser::parse(b, size, mapType, mapName);
        j = json::parse(rawret);
    }

    json recA;
    json recB;
};

// Test: parsing aoc10 record & handling .zip archive
TEST_F(ParserTest, AOC10ZipwithMap)
{
    load(recA, "AOC10_4v4_5_5e3b2a7e.mgx");
    EXPECT_EQ(recA["version"]["code"], "AOC10");
    EXPECT_EQ(recA["guid"], "a7ed6be00825bc6fd2f41fc7f2169732");
    EXPECT_EQ(recA["status"], "good");
    EXPECT_EQ(recA["duration"], 8035485);
    EXPECT_EQ(recA["message"], "");

    string mapName = "testmap.png";
    load(recB, "AOC10_4v4_3_192a8268.zip", HD_MAP, mapName);
    // A map file is successfully generated,
    EXPECT_TRUE(filesystem::exists(filesystem::path(mapName)));
    // and is a valid png file.
    EXPECT_TRUE(isPng(mapName));
    // Now delete is after testing.
    remove(mapName.c_str());

    // Test for different views of a same game
    EXPECT_EQ(recB["guid"], recA["guid"]);
}

// Test: parsing aoc10c record & handling .zip archive
TEST_F(ParserTest, AOC10CZipwithMap)
{
    load(recA, "AOC10c_2v2_4_717cd3fc.zip");
    EXPECT_EQ(recA["version"]["code"], "AOC10C");
    EXPECT_EQ(recA["status"], "good");
    EXPECT_EQ(recA["message"], "");
    EXPECT_EQ(recA["duration"], 3223100);
    EXPECT_EQ(recA["guid"], "4cad9c9c93fce1ef0d6939fc7d5d1758");
}

// Test: parsing zipped aok record
TEST_F(ParserTest, AOKwithMap)
{
    string mapName = "中文名地图.png";
    load(recA, "AOK_1v1_2_64b2d6dd.zip", NORMAL_MAP, mapName);
    EXPECT_EQ(recA["version"]["code"], "AOK");
    EXPECT_EQ(recA["status"], "good");
    EXPECT_EQ(recA["message"], "");
    EXPECT_EQ(recA["duration"], 2364525);
    EXPECT_EQ(recA["guid"], "c4616f6dce68f7649ded5a2c3706d080");
    EXPECT_EQ(recA["extractedName"], "AOK_1v1_2_64b2d6dd.mgl");
    EXPECT_EQ(recA["fileType"], "zip");

    // A map file is successfully generated,
    EXPECT_TRUE(filesystem::exists(filesystem::path(mapName)));
    // and is a valid png file.
    EXPECT_TRUE(isPng(mapName));
    // Now delete is after testing.
    remove(mapName.c_str());
}

// Test: parsing mixed team mode
TEST_F(ParserTest, AOC10cMixTeamMode)
{
    load(recA, "AOC10c_MIX_1_7ce24dd2.mgx");
    EXPECT_EQ(recA["version"]["code"], "AOC10C");
    EXPECT_EQ(recA["status"], "good");
    EXPECT_EQ(recA["message"], "");
    EXPECT_EQ(recA["duration"], 17040679);
    EXPECT_EQ(recA["guid"], "0b81ccc8cf1f3ccefbd189f36defbfef");
    EXPECT_EQ(recA["filename"], "AOC10c_MIX_1_7ce24dd2.mgx");
    EXPECT_EQ(recA["fileType"], "record");
    EXPECT_EQ(recA["teamMode"], "1v1v1v1v1v1v1v1");
}

// Test: parsing byte stream
TEST_F(ParserTest, AIinStream)
{
    auto p = filesystem::path(__FILE__).parent_path() / "testRecords" / "Warning_aitest.mgx";
    auto filesize = filesystem::file_size(p);
    vector<uint8_t> fBuffer;
    fBuffer.resize(filesize);

    ifstream f(p, ifstream::in | ifstream::binary);

    if (!f.is_open())
        FAIL();
    f.read((char*)fBuffer.data(), filesize);
    
    loadBytes(recA, fBuffer.data(), filesize);
    EXPECT_EQ(recA["filename"], "<memory buffer>");
    EXPECT_EQ(recA["guid"], "68db2945f53a25f0ec3a2946b21ee13d");
    EXPECT_EQ(recA["version"]["code"], "HD");
    EXPECT_EQ(recA["status"], "good");
    EXPECT_TRUE(recA["includeAI"]);
    EXPECT_EQ(recA["message"], "");
    EXPECT_NEAR(recA["version"]["scenarioVersion"], 1.22, 0.00001);
}

// Test: parsing userpatch versions
TEST_F(ParserTest, UserPatchVersions)
{
    load(recA, "AOCUP15_4v4_1_11beba02.mgz");
    EXPECT_EQ(recA["version"]["code"], "UP15");
    EXPECT_EQ(recA["status"], "good");

    load(recA, "up-1.4.mgz");
    EXPECT_EQ(recA["version"]["code"], "UP14");
    EXPECT_EQ(recA["status"], "good");
}

// Test: mgx2 record
TEST_F(ParserTest, HDMgx2)
{
    load(recA, "HDEdition_4v4_1_4fbd2376.mgx2");
    EXPECT_EQ(recA["version"]["code"], "HD");
    EXPECT_EQ(recA["status"], "good");
    EXPECT_NEAR(recA["version"]["saveVer"], 12.20, 0.00001);
    EXPECT_EQ(recA["message"], "");
}

// Test: HD version detection
TEST_F(ParserTest, HDVersions)
{
    load(recA, "hd-4.6.aoe2record");
    EXPECT_EQ(recA["filename"], "hd-4.6.aoe2record");
    EXPECT_EQ(recA["version"]["code"], "HD46_7");
    EXPECT_EQ(recA["status"], "good");
    EXPECT_EQ(recA["message"], "");

    load(recA, "hd-4.7.aoe2record");
    EXPECT_EQ(recA["version"]["code"], "HD46_7");
    EXPECT_EQ(recA["status"], "good");
    EXPECT_EQ(recA["message"], "");

    load(recA, "hd-4.8.aoe2record");
    EXPECT_EQ(recA["version"]["code"], "HD48");
    EXPECT_EQ(recA["status"], "good");
    EXPECT_EQ(recA["message"], "");

    load(recA, "hd-5.0.aoe2record");
    EXPECT_EQ(recA["version"]["code"], "HD50_6");
    EXPECT_EQ(recA["status"], "good");
    EXPECT_EQ(recA["message"], "");

    load(recA, "hd-5.6.aoe2record");
    EXPECT_EQ(recA["version"]["code"], "HD50_6");
    EXPECT_EQ(recA["status"], "good");
    EXPECT_EQ(recA["message"], "");

    load(recA, "hd-5.7.aoe2record");
    EXPECT_EQ(recA["version"]["code"], "HD57");
    EXPECT_EQ(recA["status"], "good");
    EXPECT_EQ(recA["message"], "");

    load(recA, "(HD with AI)SP Replay v5.8 @2022.09.26 220819.aoe2record");
    EXPECT_EQ(recA["version"]["code"], "HD58");
    EXPECT_EQ(recA["status"], "good");
    EXPECT_TRUE(recA["includeAI"]);
    EXPECT_EQ(recA["message"], "");

    load(recA, "not-a-file.mgx");
    EXPECT_EQ(recA["filename"], "<no file>");
    EXPECT_EQ(recA["version"]["code"], "UNDEFINED");
    EXPECT_EQ(recA["status"], "Aborted");
}

// Test: de build 66692
TEST_F(ParserTest, DE66692)
{
    load(recA, "SP Replay v101.102.1156.0 #(66692) @2022.10.07 214434.aoe2record");
    EXPECT_EQ(recA["version"]["build"], 66692);
    EXPECT_EQ(recA["version"]["code"], "DE");
    EXPECT_EQ(recA["version"]["interVer"], 1000);
    EXPECT_EQ(recA["status"], "good");
    EXPECT_EQ(recA["duration"], 27790);
    EXPECT_EQ(recA["guid"], "7fc8c9d8ea8750418ebcd182bca75055");
}

// Test: Brutal search for player data position in initial section
TEST_F(ParserTest, InitialDataBrutalSearch)
{
    load(recA, "de-12.97-6byte-tile.aoe2record");
    EXPECT_EQ(recA["teamMode"], "1v1");
    EXPECT_EQ(recA["version"]["code"], "DE");
    EXPECT_EQ(recA["status"], "good");
    for (auto &p : recA["players"])
    {
        if (1 == p["index"])
        {
            EXPECT_EQ(p["name"], "AstroDog");
            EXPECT_EQ(p["DEProfileID"], 378906);
        }

        if (2 == p["index"])
        {
            EXPECT_EQ(p["name"], "HappyLeaves");
            EXPECT_EQ(p["DEProfileID"], 312663);
        }
    }
}