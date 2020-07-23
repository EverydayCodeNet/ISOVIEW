/*
 *--------------------------------------
 * Program Name: ISOMETRIC
 * Author: Everyday Code
 * License:
 * Description: Isometric Terrain CE
 *--------------------------------------
*/
#include <math.h>
#include <tice.h>
#include <keypadc.h>
#include <graphx.h>
#include <fileioc.h>

/* Include the converted graphics file */
#include "gfx/gfx.h"

kb_key_t key;

const char *appvarName = "slota";

unsigned int TERRAIN_WIDTH = 25;
unsigned int TERRAIN_HEIGHT = 13;

typedef struct {
    //columns and rows
    int x;
    int y;
    int topX;
    int topY;
    int x2,x3,x4;
    int y2,y3,y4;
    int z;
    int difference;
} terrain_t;

typedef struct {
    int zAvg;
    int selected;
} game_t;
game_t game;

terrain_t terrainTiles[400];

int xOffset;
int yOffset;

void WhiText(void) {
    gfx_SetTextBGColor(0);
    gfx_SetTextFGColor(255);
    gfx_SetTextTransparentColor(0);
}

void createTerrain() {
    ti_var_t slota;
    int idx = 0;
    int row;
    int column;
    if (idx < 400) {
        for (row = 0; row < 20; row++) {
            for (column = 0; column < 20; column++) {
                terrain_t* terrain = &(terrainTiles[idx]);
                terrain_t prevCol = terrainTiles[idx - 1];
                terrain_t prevRow = terrainTiles[idx - 20];
                terrain->x = column; 
                terrain->y = row;
                //(320 - 12.5 * 20)
                terrain->topX = (terrain->x - terrain->y) * (TERRAIN_WIDTH / 2) + 160;
                terrain->topY = (terrain->x + terrain->y) * (TERRAIN_HEIGHT /2) + (240 - 12.5 * 20) + 10;
                terrain->x2 = terrain->topX + (TERRAIN_WIDTH / 2);
                terrain->y2 = terrain->topY + (TERRAIN_HEIGHT / 2);
                terrain->x3 = terrain->topX;
                terrain->y3 = terrain->topY + TERRAIN_HEIGHT;
                terrain->x4 = terrain->topX - (TERRAIN_WIDTH / 2);
                terrain->y4 = terrain->topY + (TERRAIN_HEIGHT / 2);
                //terrain->z = ti_Read(&terrainTiles[idx].z, sizeof(terrain_t), 1, slota);
                if (idx == 0) {
                    terrain->z = randInt(-15,50);
                } else if (idx < 20 && idx != 0 && column != 0) {
                    terrain->z = prevCol.z + randInt(-5,5);
                } else if (idx > 19 && column != 0) {
                    terrain->z = ((prevCol.z + prevRow.z) / 2) + randInt(-5,5);
                } else if (idx > 19 && column == 0) {
                    terrain->z = prevRow.z + randInt(-5,5);
                }
                idx++;
            }
        }
    }
}

void drawTerrain() {
    int idx;

    for (idx = 0; idx < 400; idx++) {
        terrain_t terrain = terrainTiles[idx];
        terrain_t prev = terrainTiles[idx - 1];
        //terrain_t next = terrainTiles[idx + 1];
        terrain_t nextRow = terrainTiles[idx + 20];
        terrain_t prevRow = terrainTiles[idx - 20];
        //tile borders
        /*gfx_SetColor(255);
        gfx_Line(terrain.topX - xOffset,terrain.topY - yOffset,terrain.x2 - xOffset,terrain.y2 - yOffset);
        gfx_Line(terrain.x2 - xOffset,terrain.y2 - yOffset,terrain.x3 - xOffset,terrain.y3 - yOffset);
        gfx_Line(terrain.x3 - xOffset,terrain.y3 - yOffset,terrain.x4 - xOffset,terrain.y4 - yOffset);
        gfx_Line(terrain.x4 - xOffset,terrain.y4 - yOffset,terrain.topX - xOffset,terrain.topY - yOffset);*/
        
        //draw height
        //back right
        gfx_SetColor(224);
        if (terrain.z < 0) gfx_SetColor(17);
        gfx_FillTriangle(terrain.topX - xOffset,terrain.topY - yOffset,
                    terrain.topX - xOffset,terrain.topY - terrain.z - yOffset,
                    terrain.x2 - xOffset, terrain.y2 - terrain.z - yOffset
                );
        gfx_FillTriangle(terrain.topX - xOffset,terrain.topY - yOffset,
                        terrain.x2 - xOffset,terrain.y2 - yOffset,
                        terrain.x2 - xOffset, terrain.y2 - terrain.z - yOffset
                    );
        
        //back left - works
        gfx_SetColor(64);
        if (terrain.z < 0) gfx_SetColor(17);
        if (prev.z > terrain.z) {
            gfx_FillTriangle(terrain.x4 - xOffset,terrain.y4 - yOffset,
                        terrain.x4 - xOffset,terrain.y4-terrain.z - yOffset,
                        terrain.topX - xOffset,terrain.topY - yOffset);
            gfx_FillTriangle(terrain.topX - xOffset,terrain.topY -terrain.z - yOffset,
                            terrain.topX - xOffset,terrain.topY - yOffset,
                            terrain.x4 - xOffset,terrain.y4-terrain.z - yOffset);
        }   

        gfx_SetColor(224);
        if (terrain.z < 0) gfx_SetColor(17);
        //front left
        //if next row is deeper than row, don't display front/left
        //last row should always show face
        if (terrain.z > nextRow.z || idx > 379) {
            gfx_FillTriangle(terrain.x4 - xOffset,terrain.y4 - yOffset,
            terrain.x4- xOffset,terrain.y4 - yOffset - terrain.z,
            terrain.x3- xOffset,terrain.y3 - yOffset - terrain.z);
            gfx_FillTriangle(terrain.x4- xOffset,terrain.y4 - yOffset,
            terrain.x3- xOffset,terrain.y3 - yOffset,
            terrain.x3- xOffset,terrain.y3 - yOffset - terrain.z);
        }

        //right
        gfx_SetColor(64);
        if (terrain.z < 0) gfx_SetColor(17);
        gfx_FillTriangle(terrain.x3 - xOffset,terrain.y3 - yOffset,
                        terrain.x3 - xOffset,terrain.y3 - yOffset - terrain.z, 
                        terrain.x2 - xOffset,terrain.y2 - yOffset - terrain.z);
        gfx_FillTriangle(terrain.x3 - xOffset,terrain.y3 - yOffset,
                        terrain.x2 - xOffset,terrain.y2 - yOffset, 
                        terrain.x2 - xOffset,terrain.y2 - yOffset - terrain.z);
        
        //top
        gfx_SetColor(96);
        if (terrain.z < 0) gfx_SetColor(17);
        //snow
        if (terrain.z > 25) gfx_SetColor(255);
        gfx_FillTriangle(terrain.x4 - xOffset, terrain.y4 - yOffset - terrain.z,
                        terrain.topX - xOffset, terrain.topY - yOffset - terrain.z,
                        terrain.x3 - xOffset, terrain.y3 - yOffset - terrain.z);

        gfx_FillTriangle(terrain.x3 - xOffset,terrain.y3 - terrain.z - yOffset,
                        terrain.topX - xOffset, terrain.topY - yOffset - terrain.z,
                        terrain.x2 - xOffset, terrain.y2 - yOffset - terrain.z);
    }
}

void dispStats() {
    terrain_t terrain = terrainTiles[game.selected];
    WhiText();
    gfx_PrintStringXY("topX: ",10,10);
    gfx_PrintInt(terrain.topX,1);
    gfx_PrintStringXY("Top Y:",10,20);
    gfx_PrintInt(terrain.topY,1);
    gfx_PrintStringXY("row: ",10,30);
    gfx_PrintInt(terrain.y,1);
    gfx_PrintStringXY("col: ",10,40);
    gfx_PrintInt(terrain.x,1);
    gfx_PrintStringXY("HEIGHT: ",10,60);
    gfx_PrintInt(terrain.z,1);
}

void shiftMap() {
    gfx_SetDrawBuffer();
    gfx_FillScreen(0);
    drawTerrain();
}

void handleKeys() {
    //only shift plate if item is not selected
    key = kb_Data[7];
    if (kb_Data[1] == kb_Yequ) game.selected--;
    if (kb_Data[1] == kb_Graph) game.selected++;

    if (key & kb_Right) {
        xOffset = xOffset + 20;
        shiftMap();
        gfx_SwapDraw();
    } else if (key & kb_Left) {
        xOffset = xOffset - 20;
        shiftMap();
        gfx_SwapDraw();
    } else if (key & kb_Up) {
        yOffset = yOffset - 20;
        shiftMap();
        gfx_SwapDraw();
        
    } else if (key & kb_Down) {
        yOffset = yOffset + 20;
        shiftMap();
        gfx_SwapDraw();
    }
}

void main(void)
{   
    srand(rtc_Time());

    createTerrain();
    gfx_Begin();
    gfx_FillScreen(0);
    drawTerrain();
    //dispStats();
    
    do {
        kb_Scan();
        handleKeys();
        //dispStats();
    } while(kb_Data[6] != kb_Clear);
    gfx_End();
}
