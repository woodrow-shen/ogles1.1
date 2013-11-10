#ifndef DVR_IMPL__H_INCLUDED
#define DVR_IMPL__H_INCLUDED

// ===================================================================
// CASLAB 3D Graphics Group
// OpenGL ES 1.1 API layer implementation
// -------------------------------------------------------------------
//
// Version : 0.1
// File Name : dvr_impl.h
// Purpose : driver functions declaration for tile-based
//
// Copyright (c) 2008, Shye-Tzeng Shen. All rights reserved.
// -------------------------------------------------------------------
//
// 25-08-2008   Shye-Tzeng  Shen    initial version
//
// ===================================================================

#include "ogles.h"
#include "context.h"
#include "geometry.h"
#include "rasterizer.h"
#include "fixed.h"

/***********************************************************
    Tile-based State Management Process
    ----------------------------------------------------
    propose: add a node into the tiled displayList
    kind: 0x1-> Render state instructions
          0x2-> two-pass instructions with a 32bit data
          0x3-> Triangle base address
    data: instructions / address
***********************************************************/
void add(unsigned char kind, unsigned int data, unsigned int index);

/*
 *  A linked-list of recording state information
 *  --------------------------------------------
 *  id: GL identifier
 *  value: The content of render state for RM
 */
RenderState* addState(unsigned short id, unsigned int value, RenderState *top);

void release_display_list(void);

void refresh_display_list(bool last=false);

void tile_render();

void draw_init_table(void);

void drawarrays_fixed_pipeline(void);

void drawelements_fixed_pipeline(void);

void deallocate(void);

extern bool isRenderDone;

#endif // DVR_IMPL__H_INCLUDED
