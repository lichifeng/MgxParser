/**
 * \file       MapTools.h
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-03
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#define cimg_use_png
#define cimg_display 0

#include <string>
#include "CImg.h"
#include "MapColors.h"
#include "TileStructures.h"

//#include "../Analyzers/default/Analyzer.h"

using namespace std;

template <typename T, typename M>
void getMap(
    string savePath,
    T *analyzer,
    uint32_t width = 300, uint32_t height = 150,
    bool HD = false)
{
    M *mapData = (M *)analyzer->mapDataPtr;

    int elevation, curPos, rbPos; // rb = right bottom

    // Init map
    cimg_library::CImg<unsigned char> img(analyzer->mapCoord[0], analyzer->mapCoord[1], 1, 4, 0xff);

    // populate bits
    cimg_forXY(img, x, y)
    {
        curPos = y * analyzer->mapCoord[0] + x;
        rbPos = (y + 1) * analyzer->mapCoord[0] + (x + 1);

        elevation = 1;
        if (x < (analyzer->mapCoord[0] - 1) && y < (analyzer->mapCoord[1] - 1))
        {
            if (mapData[curPos].elevation > mapData[rbPos].elevation)
                elevation = 0;
            else if (mapData[curPos].elevation < mapData[rbPos].elevation)
                elevation = 2;
        }

        img(x, y, 0) = MAP_COLORS[mapData[curPos].terrainType][elevation][0];
        img(x, y, 1) = MAP_COLORS[mapData[curPos].terrainType][elevation][1];
        img(x, y, 2) = MAP_COLORS[mapData[curPos].terrainType][elevation][2];
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

    float factor = analyzer->mapCoord[0] / 220.0;
    float tcX, tcY;

    // Zoom the map to 3x original size
    if (HD)
    {
        img.resize_tripleXY();
        factor *= 2.5;
    }

    for (auto &p : analyzer->players)
    {
        if (!p.valid())
            continue;
        tcX = HD ? p.initCamera[0] * 3 : p.initCamera[0];
        tcY = HD ? p.initCamera[1] * 3 : p.initCamera[1];
        img.draw_circle(tcX, tcY, factor * 8, playerColors[p.colorID], 0.3);
        img.draw_circle(tcX, tcY, factor * 4, playerColors[p.colorID], 1);
    }

    // rotate
    img.rotate(-45);

    // resize
    img.resize(width, height);

    // save png
    img.save_png(savePath.c_str());

    // img.display();
}