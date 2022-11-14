/***************************************************************
 * \file       map_parser.h
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/8
 * \copyright  Copyright (c) 2020-2022
 ***************************************************************/

#ifndef MGXPARSER_MAP_PARSER_H_
#define MGXPARSER_MAP_PARSER_H_

#define cimg_use_png
// https://cimg.eu/reference/structcimg__library_1_1CImgDisplay.html
#define cimg_display 0

#include <cstdio>
#include <string>

#include "CImg/CImg.h"
#include "map_colors.h"
#include "map_tiles.h"

template <typename T, typename M>
void ParseMapTile(
    FILE *save_dest,
    T *analyzer,
    uint32_t width = 300, uint32_t height = 150,
    bool hd = false) {
    M *mapdata_ptr = (M *)analyzer->mapdata_ptr_;

    int elevation, cur_pos, rb_pos;  // rb = right bottom

    // Init map
    cimg_library::CImg<unsigned char> img(analyzer->map_coord_[0], analyzer->map_coord_[1], 1, 4, 0xff);

    // populate bits
    cimg_forXY(img, x, y) {
        cur_pos = y * analyzer->map_coord_[0] + x;
        rb_pos = (y + 1) * analyzer->map_coord_[0] + (x + 1);

        elevation = 1;
        if (x < (analyzer->map_coord_[0] - 1) && y < (analyzer->map_coord_[1] - 1)) {
            if (mapdata_ptr[cur_pos].elevation > mapdata_ptr[rb_pos].elevation)
                elevation = 0;
            else if (mapdata_ptr[cur_pos].elevation < mapdata_ptr[rb_pos].elevation)
                elevation = 2;
        }

        img(x, y, 0) = MAP_COLORS[mapdata_ptr[cur_pos].terrainType][elevation][0];
        img(x, y, 1) = MAP_COLORS[mapdata_ptr[cur_pos].terrainType][elevation][1];
        img(x, y, 2) = MAP_COLORS[mapdata_ptr[cur_pos].terrainType][elevation][2];
        img(x, y, 3) = 0xff;
    }

    // // draw objects
    // unsigned char RGB[4];
    // for (auto &obj : _gaiaObjs)
    // {
    //     // NOTE: See this: https://stackoverflow.com/questions/5134614/c-const-map-element-access
    //     RGB[0] = gaiaColors.at(obj.id).r;
    //     RGB[1] = gaiaColors.at(obj.id).g;
    //     RGB[2] = gaiaColors.at(obj.id).b;
    //     RGB[3] = 0xff;
    //     img.draw_rectangle(obj.pos[0] - 1, obj.pos[1] - 1, obj.pos[0] + 1, obj.pos[1] + 1, RGB, 1);
    // }

    // for (auto &obj : _playerObjs)
    // {
    //     Realm *realm = playerData.getRealm(obj.owner);
    //     if (realm->valid() && obj.isGate())
    //     {
    //         img.draw_rectangle(obj.pos[0] - 1, obj.pos[1] - 1, obj.pos[0] + 1, obj.pos[1] + 1, playerColors[realm->color], 1);
    //     }
    // }

    float factor = analyzer->map_coord_[0] / 220.0;
    float tcX, tcY;

    // Zoom the map to 3x original size
    if (hd) {
        img.resize_tripleXY();
        factor *= 2.5;
    }

    for (auto &p : analyzer->players) {
        if (!p.Valid() && p.init_camera_[0] != 0.0 && p.init_camera_[1] != 0.0)
            continue;
        tcX = hd ? p.init_camera_[0] * 3 : p.init_camera_[0];
        tcY = hd ? p.init_camera_[1] * 3 : p.init_camera_[1];
        img.draw_circle(tcX, tcY, factor * 8, playerColors[p.color_id_], 0.3);
        img.draw_circle(tcX, tcY, factor * 4, playerColors[p.color_id_], 1);
    }

    // rotate
    img.rotate(-45);

    // resize
    img.resize(width, height);

    // save png
    img.save_png(save_dest);

    // img.display();
}

#endif  // MGXPARSER_MAP_PARSER_H_