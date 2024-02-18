/***************************************************************
 * \file       addon_guid.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/7
 * \copyright  Copyright (c) 2020-2024
 ***************************************************************/

#include <openssl/md5.h>
#include "analyzer.h"
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
    unsigned char output_buf[MD5_DIGEST_LENGTH];
    const unsigned char *output_ref = output_buf;
    MD5_CTX ctx;

    MD5_Init(&ctx);
    MD5_Update(&ctx, version_string_, 8);
    MD5_Update(&ctx, &save_version_, 4);
    MD5_Update(&ctx, &log_version_, 4);
    MD5_Update(&ctx, &scenario_version_, 4);
    MD5_Update(&ctx, &map_size_, 4);
    MD5_Update(&ctx, &population_limit_, 4);
    MD5_Update(&ctx, &game_speed_, 4);
    MD5_Update(&ctx, &map_id_, 4); // \note Not in AOK, need a stable default value
    for (size_t i = 0; i < earlymove_count_; i++) {
        MD5_Update(&ctx, earlymove_cmd_[i], 19); // https://github.com/stefan-kolb/aoc-mgx-format/blob/master/spec/body/actions/03-move.md
        MD5_Update(&ctx, &earlymove_time_[i], 4);
    }
    for (auto &p: players) {
        if (!p.Valid())
            continue;
        MD5_Update(&ctx, p.name.data(), p.name.size());
        MD5_Update(&ctx, &p.civ_id_, 1);
        MD5_Update(&ctx, &p.index, 4);
        MD5_Update(&ctx, &p.slot, 4);
        MD5_Update(&ctx, &p.color_id_, 1);
        MD5_Update(&ctx, &p.resolved_teamid_, 1);
    }

    MD5_Final(output_buf, &ctx);

    retro_guid_ = BytesToHex(output_ref, MD5_DIGEST_LENGTH);
}