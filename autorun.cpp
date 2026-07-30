// **************************************************************************
//
// autorun.cpp
//
// function to run trains in auto mode.
// 
//
// Vimal Parikh 
// April, 2026
//
//
// **************************************************************************
//
#include "vThrottle.h"
#include "autorun.h"

// saves current index of the locomotive in the block that it occupies
// initially set via a dropdown menu from UI
// unoccupied blocks have index of -1
int locoInBlock[NUM_BLOCK_SENSORS] = {
  -1, -1, -1, -1, -1, 
  -1, -1, -1, -1, -1, 
};
static bool locoBlksInitialized = false;

// Block connetivity of the track layout
// Each block entry show the previous blocks it is connected to
// as the loco moves forward.
// This table is used to determine which block the locomotve was in
// before it entered the current block.
block_connect_entry_t   blockLayout[NUM_BLOCK_SENSORS] = {
  {TURNOUT_TR0,   TOWN_LOOP_BLK2, BLOCK_NONE},      // block 0
  {TURNOUT_TL0,   TOWN_LOOP_BLK0, OUT1_LOOP_BLK3},  // block 1
  {TURNOUT_NONE,  TOWN_LOOP_BLK1, BLOCK_NONE},      // block 2
  {TURNOUT_TR0,   OUT1_LOOP_BLK5, TOWN_LOOP_BLK2},  // block 3
  {TURNOUT_NONE,  OUT1_LOOP_BLK3, BLOCK_NONE},      // block 4
  {TURNOUT_XC,    OUT1_LOOP_BLK4, OUT2_LOOP_BLK7},  // block 5
  {TURNOUT_NONE,  OUT2_LOOP_BLK8, BLOCK_NONE},      // block 6
  {TURNOUT_TL2,   OUT2_LOOP_BLK6, SIDEING_BLK9},    // block 7
  {TURNOUT_XC,    OUT2_LOOP_BLK7, OUT1_LOOP_BLK4},  // block 8
  {TURNOUT_NONE,  OUT2_LOOP_BLK8, BLOCK_NONE},      // block 9
};

//------------------------------------------------------------
// Set the starting block index where the loco is located.
//------------------------------------------------------------

void setLocoStartBlock(uint loco_idx, uint start_blk_idx)
{
  locoInBlock[start_blk_idx] = loco_idx;
  DEBUG_PRINTF("setLocoStartBlock: loco = %d, blk = %d\n", loco_idx, start_blk_idx);
  locoBlksInitialized = true;
}

//------------------------------------------------------------
// Update the locomotives block assignment.
// This called when a new block sensor goes active.
// The block connectivity table is used to set the next block
// for the loco based on current block.
//------------------------------------------------------------

void updateLocoBlock(uint new_block_idx)
{
  uint prev_block_idx;
  int  locoIdx;
  uint turnout_idx = blockLayout[new_block_idx].turnoutIdx;
  bool turnoutThrown = false;
  
  if (!locoBlksInitialized) return;

  if (turnout_idx == TURNOUT_NONE) {
    prev_block_idx = blockLayout[new_block_idx].blkIdxClosed;
  }
  else {
    uint16_t turnout_addr = turnoutList[turnout_idx];
    Turnout* turnout = dccexProtocol.getTurnoutById(turnout_addr);
    turnoutThrown = turnout->getThrown();
    prev_block_idx = (turnoutThrown) ? blockLayout[new_block_idx].blkIdxThrown 
                                     : blockLayout[new_block_idx].blkIdxClosed;
                                
    if (prev_block_idx == BLOCK_NONE)
      DEBUG_PRINTF("*** ERROR: newblk=%d, previous block %d is undefined\n", new_block_idx, prev_block_idx);
  }

  DEBUG_PRINTF("updateLocoBlock: newblk=%d, prevBlk=%d, turnIdx=%d, thrown=%d\n", new_block_idx, prev_block_idx,
              turnout_idx, turnoutThrown);
    
  if (prev_block_idx != BLOCK_NONE) {
    locoIdx = locoInBlock[prev_block_idx];
    if (locoIdx == -1)
      DEBUG_PRINTF("*** ERROR: No locomotive found in previous block\n");
    else if (locoInBlock[new_block_idx] != -1 ) 
      DEBUG_PRINTF("*** ERROR: New block is occupied for loco to move\n");
    
    if (locoIdx >= 0) {
      locoInBlock[prev_block_idx] = -1;
      locoInBlock[new_block_idx] = locoIdx;
      DEBUG_PRINTF("  locInBox[%d]=%d, locoInBox[%d]=%d\n", prev_block_idx, locoInBlock[prev_block_idx],
                  new_block_idx, locoInBlock[new_block_idx]);
    }
  }

  DEBUG_PRINTF(" LocoInBlk[]={");
  for (int b = 0; b < NUM_BLOCK_SENSORS; ++b) {
    DEBUG_PRINTF("%d,", locoInBlock[b]);
  }
  DEBUG_PRINTF("\n");
}

