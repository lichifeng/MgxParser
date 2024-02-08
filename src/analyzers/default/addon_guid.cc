/***************************************************************
 * \file       addon_guid.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/7
 * \copyright  Copyright (c) 2020-2024
 ***************************************************************/

#include "analyzer.h"
#include "md5/md5.h"
#include "bytestohex.h"

/**
 * \brief      Guid is presented in HD/DE records, but not in older versions. Guid can be used to identify different views of same game.
 * \details    This function will combine some stable info among different views of same game and do a md5 digesting to get an uniform code
 * for them. It is used mainly in records of older versions which have no native guid embeded.
 * MgxParser uses following data to generate guid of a game:
 *     | versionStr
 *     | saveVersion
 *     | log_version_
 *     | scenarioVersion
 *     | mapSize
 *     | popLimit
 *     | gameSpeed
 *     | First 20 bytes of early MOVE actions data in body
 *     | Happen time of early MOVE actions
 *     | PlayerName + civID + index + slot + colorID + teamID by slot sequence

 * Note: Duration is normally different among records of same game, 'cuz player
 *       quit at different times. That's why duration cannot be a factor of this
 *       and only the very early action data can be used.
 *
 * \return     string              Guid of this record
 */
void DefaultAnalyzer::CalcRetroGuid(int debug_flag) {
    status_.debug_flag_ = debug_flag;

    std::vector<uint8_t> input;
    uint8_t output_buf[16];
    const uint8_t *output_ref = output_buf;
    MGXPARSER_MD5::MD5_CTX ctx;

    MGXPARSER_MD5::md5_init(&ctx);
    MGXPARSER_MD5::md5_update(&ctx, (uint8_t *) version_string_, 8);
    MGXPARSER_MD5::md5_update(&ctx, (uint8_t *) &save_version_, 4);
    MGXPARSER_MD5::md5_update(&ctx, (uint8_t *) &log_version_, 4);
    MGXPARSER_MD5::md5_update(&ctx, (uint8_t *) &scenario_version_, 4);
    MGXPARSER_MD5::md5_update(&ctx, (uint8_t *) &map_size_, 4);
    MGXPARSER_MD5::md5_update(&ctx, (uint8_t *) &population_limit_, 4);
    MGXPARSER_MD5::md5_update(&ctx, (uint8_t *) &game_speed_, 4);
    MGXPARSER_MD5::md5_update(&ctx, (uint8_t *) &map_id_, 4); // \note Not in AOK, need a stable default value
    for (size_t i = 0; i < earlymove_count_; i++) {
        MGXPARSER_MD5::md5_update(&ctx, earlymove_cmd_[i], 19); // https://github.com/stefan-kolb/aoc-mgx-format/blob/master/spec/body/actions/03-move.md
        MGXPARSER_MD5::md5_update(&ctx, (uint8_t *) &earlymove_time_[i], 4);
    }
    for (auto &p: players) {
        if (!p.Valid())
            continue;
        // \note Encoding-related operations will change name bytes, caution!
        MGXPARSER_MD5::md5_update(&ctx, (uint8_t *) p.name.data(), p.name.size());
        MGXPARSER_MD5::md5_update(&ctx, &p.civ_id_, 1);
        MGXPARSER_MD5::md5_update(&ctx, (uint8_t *) &p.index, 4);
        MGXPARSER_MD5::md5_update(&ctx, (uint8_t *) &p.slot, 4);
        MGXPARSER_MD5::md5_update(&ctx, &p.color_id_, 1);
        MGXPARSER_MD5::md5_update(&ctx, &p.resolved_teamid_, 1);
    }
    // \note Raw map data may have some difference, but generated map file have same md5 digest?

    MGXPARSER_MD5::md5_final(&ctx, output_buf);

    retro_guid_ = BytesToHex(output_ref, 16);
}