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
    return (filesystem::path(__FILE__).parent_path() / "test_records" / f).generic_string();
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
    void load(json &j, string f, MgxParser::MapType mapType = MgxParser::NO_MAP, string mapName = "testmap.png")
    {
        auto path = genPath(f);
        MgxParser::Settings _ = {.input_path = path, .map_type = mapType, .map_path = mapName};
        string rawret = MgxParser::parse(_);
        j = json::parse(rawret);
    }

    void loadBytes(json &j, const uint8_t *b, uint32_t size, MgxParser::MapType mapType = MgxParser::NO_MAP, string mapName = "testmap.png")
    {
        MgxParser::Settings _ = {.input_stream = b, .input_size = size, .map_type = mapType, .map_path = mapName};
        string rawret = MgxParser::parse(_);
        j = json::parse(rawret);
    }

    json recA;
    json recB;
};

// Test: parsing aoc10 record & handling .zip archive
TEST_F(ParserTest, AOC10ZipwithMap)
{
    // this record takes < 20ms to parse on synology ds1621+ docker container without md5 calculation
    // and ~60ms with md5 calc.
    load(recA, "AOC10_4v4_5_5e3b2a7e.mgx");
    EXPECT_EQ(recA["version"]["code"], "AOC10");
    EXPECT_EQ(recA["guid"], "d46a6ae13bea04e1744043f5017f9786");
    EXPECT_EQ(recA["filemd5"], "5e3b2a7e604f71c8a3793d41f522639c");
    EXPECT_EQ(recA["duration"], 8035485);
    EXPECT_EQ(recA["message"], "");

    string mapName = "testmap.png";
    load(recB, "AOC10_4v4_3_192a8268.zip", MgxParser::HD_MAP, mapName);
    // A map file is successfully generated,
    EXPECT_TRUE(filesystem::exists(filesystem::path(mapName)));
    // and is a valid png file.
    EXPECT_TRUE(isPng(mapName));
    // Now delete is after testing.
    remove(mapName.c_str());

    // Test for different views of a same game
    EXPECT_EQ(recB["guid"], recA["guid"]);
    EXPECT_EQ(recB["filemd5"], "192a8268f8e188190837c2ff08d1ca6e");
}

// Test: parsing aoc10c record & handling .zip archive
TEST_F(ParserTest, AOC10CZip)
{
    load(recA, "AOC10c_2v2_4_717cd3fc.zip");
    EXPECT_EQ(recA["version"]["code"], "AOC10C");
    EXPECT_EQ(recA["message"], "");
    EXPECT_EQ(recA["duration"], 3223100);
    EXPECT_EQ(recA["guid"], "7bc37bf3d04c3a9c91c78cf36a08870a");
}

// Test: parsing zipped aok record
TEST_F(ParserTest, AOKwithMap)
{
    string mapName = "中文名地图.png";
    load(recA, "AOK_1v1_2_64b2d6dd.zip", MgxParser::NORMAL_MAP, mapName);
    EXPECT_EQ(recA["version"]["code"], "AOK");
    EXPECT_EQ(recA["message"], "");
    EXPECT_EQ(recA["duration"], 2364525);
    //EXPECT_EQ(recA["guid"], "c4616f6dce68f7649ded5a2c3706d080");
    EXPECT_EQ(recA["extractedName"], "AOK_1v1_2_64b2d6dd.mgl");
    EXPECT_EQ(recA["filetype"], ".zip");

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
    EXPECT_EQ(recA["message"], "");
    EXPECT_EQ(recA["duration"], 17040679);
    EXPECT_EQ(recA["guid"], "07a47274f462a2487fc96ad81be8ebe1");
    EXPECT_EQ(recA["filename"], "AOC10c_MIX_1_7ce24dd2.mgx");
    EXPECT_EQ(recA["filetype"], ".mgx");
    EXPECT_EQ(recA["teamMode"], "1v1v1v1v1v1v1v1");
}

// Test: parsing byte stream
TEST_F(ParserTest, AIinStream)
{
    auto p = filesystem::path(__FILE__).parent_path() / "test_records" / "Warning_aitest.mgx";
    auto filesize = filesystem::file_size(p);
    vector<uint8_t> fBuffer;
    fBuffer.resize(filesize);

    ifstream f(p, ifstream::in | ifstream::binary);

    if (!f.is_open())
        FAIL();
    f.read((char*)fBuffer.data(), filesize);
    
    loadBytes(recA, fBuffer.data(), filesize);
    EXPECT_EQ(recA["filename"], "<memory stream>");
    EXPECT_EQ(recA["guid"], "f35e36abd7dcb31ccb493c36cf83caa0");
    EXPECT_EQ(recA["version"]["code"], "HD");
    EXPECT_TRUE(recA["includeAI"]);
    EXPECT_EQ(recA["message"], "");
    EXPECT_NEAR(recA["version"]["scenarioVersion"], 1.22, 0.00001);
}

// Test: parsing userpatch versions
TEST_F(ParserTest, UserPatchVersions)
{
    load(recA, "AOCUP15_4v4_1_11beba02.mgz");
    EXPECT_EQ(recA["version"]["code"], "UP15");

    load(recA, "up-1.4.mgz");
    EXPECT_EQ(recA["version"]["code"], "UP14");
    EXPECT_EQ(recA["status"], "perfect");
}

// Test: player civ/team/color updated by postgame data
// test/test_records/AOCUP15_4v4_8_POSTGAMETEST.zip
TEST_F(ParserTest, UserPatchPostGameData)
{
    load(recA, "AOCUP15_4v4_8_POSTGAMETEST.zip");
    EXPECT_EQ(recA["version"]["code"], "UP15");

    EXPECT_EQ(recA["guid"], "0723c926d1a9571a5a3fd6d56dcc7054");
}

// Test: mgx2 record
TEST_F(ParserTest, HDMgx2)
{
    load(recA, "HDEdition_4v4_1_4fbd2376.mgx2");
    EXPECT_EQ(recA["version"]["code"], "HD");
    EXPECT_NEAR(recA["version"]["saveVer"], 12.20, 0.00001);
    EXPECT_EQ(recA["message"], "");
}

// Test: HD version detection
TEST_F(ParserTest, HDVersions)
{
    load(recA, "hd-4.6.aoe2record");
    EXPECT_EQ(recA["filename"], "hd-4.6.aoe2record");
    EXPECT_EQ(recA["version"]["code"], "HD46_7");
    EXPECT_EQ(recA["message"], "");

    load(recA, "hd-4.7.aoe2record");
    EXPECT_EQ(recA["version"]["code"], "HD46_7");
    EXPECT_EQ(recA["message"], "");

    load(recA, "hd-4.8.aoe2record");
    EXPECT_EQ(recA["version"]["code"], "HD48");
    EXPECT_EQ(recA["message"], "");

    load(recA, "hd-5.0.aoe2record");
    EXPECT_EQ(recA["version"]["code"], "HD50_6");
    EXPECT_EQ(recA["message"], "");

    load(recA, "hd-5.6.aoe2record");
    EXPECT_EQ(recA["version"]["code"], "HD50_6");
    EXPECT_EQ(recA["message"], "");

    load(recA, "hd-5.7.aoe2record");
    EXPECT_EQ(recA["version"]["code"], "HD57");
    EXPECT_EQ(recA["message"], "");

    load(recA, "(HD with AI)SP Replay v5.8 @2022.09.26 220819.aoe2record");
    EXPECT_EQ(recA["version"]["code"], "HD58");
    EXPECT_TRUE(recA["includeAI"]);
    EXPECT_EQ(recA["message"], "");

    try {
        load(recA, "not-a-file.mgx");
    } catch (std::string& s) {
        EXPECT_EQ(s, "File not exists.");
    }
}

// Test: de build 66692
TEST_F(ParserTest, DE66692)
{
    load(recA, "SP Replay v101.102.1156.0 #(66692) @2022.10.07 214434.aoe2record");
    EXPECT_EQ(recA["version"]["build"], 66692);
    EXPECT_EQ(recA["version"]["code"], "DE");
    EXPECT_EQ(recA["version"]["interVer"], 1000);
    EXPECT_EQ(recA["status"], "perfect");
    EXPECT_EQ(recA["duration"], 27790);
    EXPECT_EQ(recA["guid"], "7fc8c9d8ea8750418ebcd182bca75055");
    EXPECT_EQ(recA["gameTime"], 1665150274);
}

// Test: de build 75350
TEST_F(ParserTest, DE75350)
{
    load(recA, "de-versions/AgeIIDE_Replay_212394317.zip");
    EXPECT_EQ(recA["version"]["build"], 75350);
    EXPECT_EQ(recA["version"]["code"], "DE");
    EXPECT_EQ(recA["version"]["interVer"], 1000);
    EXPECT_EQ(recA["status"], "perfect");
    EXPECT_EQ(recA["guid"], "3cf58e536108d3459b8f62554e2159c9");
    EXPECT_EQ(recA["gameTime"], 1676955878);
}

// Test: Brutal search for player data position in initial section
TEST_F(ParserTest, InitialDataBrutalSearch)
{
    load(recA, "de-12.97-6byte-tile.aoe2record");
    EXPECT_EQ(recA["teamMode"], "1v1");
    EXPECT_EQ(recA["version"]["code"], "DE");
    EXPECT_EQ(recA["status"], "perfect");
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

// Test: Records with triggers
TEST_F(ParserTest, Triggers)
{
    load(recA, "trigger_test/AOC10_3v4_2_fc362460.zip");
    EXPECT_EQ(recA["status"], "perfect");
    load(recA, "trigger_test/AOCUP14_1v1v1v1v1v1v1v1_2_611d777b.zip");
    EXPECT_EQ(recA["status"], "perfect");
    load(recB, "trigger_test/AOCUP15_1v1v1v1v1_1_97159b00.zip");
    EXPECT_EQ(recB["status"], "perfect");
    load(recB, "trigger_test/HDEdition46_混战_1_bd52a416.zip");
    EXPECT_EQ(recB["status"], "perfect");
}

// \todo
// test filemd5: 54633e3a28299e84c700ad6e928e5521
// this records has a log version value of 0