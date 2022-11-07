/**
 * \file       helpers.h
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief      
 * \version    0.1
 * \date       2022-10-04
 * 
 * \copyright  Copyright (c) 2020-2022
 * 
 */
#ifndef MGXPARSER_AUXILIARY_H_
#define MGXPARSER_AUXILIARY_H_

#define OP_COMMAND              0x01
#define OP_SYNC                 0x02
#define OP_VIEWLOCK             0x03
#define OP_CHAT                 0x04

#define COMMAND_RESIGN          0x0b
#define COMMAND_RESEARCH        0x65
#define COMMAND_TRAIN           0x77
#define COMMAND_TRAIN_SINGLE    0x64
#define COMMAND_BUILD           0x66
#define COMMAND_TRIBUTE         0x6c
#define COMMAND_POSTGAME        0xff
#define COMMAND_MOVE            0x03

#define RESEARCH_FEUDAL         101
#define RESEARCH_CASTLE         102
#define RESEARCH_IMPERIAL       103

#endif //MGXPARSER_AUXILIARY_H_