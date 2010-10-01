/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2007 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

const char *yaffs_checkptrw_c_version =
	"$Id: yaffs_checkptrw.c,v 1.22 2009/11/03 02:36:30 charles Exp $";


#include "yaffs_checkptrw.h"
#include "yaffs_getblockinfo.h"


static void deleteBlockChk(yaffs_Device *dev, int blk)
{
	int erasedOk, previousState, previousPages; 
	yaffs_ExtendedTags tags;
	yaffs_BlockInfo *bi = yaffs_GetBlockInfo(dev, blk);
	__u8 *blkBits;
	previousState = bi->blockState;		
	previousPages = bi->pagesInUse;
	
	dev->nDeletions = dev->nDeletions + dev->nChunksPerBlock;
	dev->nUnmarkedDeletions = dev->nUnmarkedDeletions + dev->nChunksPerBlock;
	dev->nFreeChunks = dev->nFreeChunks + dev->nChunksPerBlock; 
	bi->pagesInUse = 0;
	
	if ((!bi->needsRetiring) && (bi->blockState != YAFFS_BLOCK_STATE_DEAD)) {
		bi->blockState = YAFFS_BLOCK_STATE_DIRTY;
		erasedOk = yaffs_EraseBlockInNAND(dev, blk);
		if (!erasedOk) {
			dev->nErasureFailures++;
			T(YAFFS_TRACE_ALWAYS, (TSTR("FAIL  Erasure failed %d" TENDSTR), blk));
		}
	}
	if (erasedOk) {
		bi->blockState = YAFFS_BLOCK_STATE_EMPTY;
		dev->nErasedBlocks++;
		bi->pagesInUse = 0;
		bi->softDeletions = 0;
		bi->hasShrinkHeader = 0;
		bi->skipErasedCheck = 1;  
		bi->gcPrioritise = 0;
		blkBits = dev->chunkBits + (dev->chunkBitmapStride * (blk - dev->internalStartBlock));
		memset(blkBits, 0, dev->chunkBitmapStride);
		//T(YAFFS_TRACE_ALWAYS,
		//(("\nINFO  Erased block %d, ex-state %d, ex-pgInUse %d" TENDSTR), 
		//blk, previousState, previousPages));
	} else {
		dev->nFreeChunks -= dev->nChunksPerBlock;	/* We lost a block of free space */

		//MC block retirement ...
		if (yaffs_MarkBlockBad(dev, blk) != YAFFS_OK) {
			if (yaffs_EraseBlockInNAND(dev, blk) != YAFFS_OK) {
				T(YAFFS_TRACE_ALWAYS, (TSTR("yaffs: Failed to mark bad and erase block %d"
					TENDSTR), blk));
			} else {
				int chunkId = blk * dev->nChunksPerBlock;
				__u8 *buffer = yaffs_GetTempBuffer(dev, __LINE__);

				memset(buffer, 0xff, dev->nDataBytesPerChunk);
				yaffs_InitialiseTags(&tags);
				tags.sequenceNumber = YAFFS_SEQUENCE_BAD_BLOCK;
				if (dev->writeChunkWithTagsToNAND(dev, chunkId - dev->chunkOffset, buffer, &tags)
					!= YAFFS_OK)
					T(YAFFS_TRACE_ALWAYS, (TSTR("yaffs: Failed to " 
					TCONT("write bad block marker to block %d")TENDSTR), blk));

				yaffs_ReleaseTempBuffer(dev, buffer, __LINE__);
			}
		}

		bi->blockState = YAFFS_BLOCK_STATE_DEAD;
		bi->gcPrioritise = 0;
		bi->needsRetiring = 0;
		dev->nRetiredBlocks++;		
		T(YAFFS_TRACE_ALWAYS,(TSTR("WARN  Block %d retired" TENDSTR), blk));
	}	
}


static int yaffs_CheckpointSpaceOk(yaffs_Device *dev)
{
	int blocksAvailable = dev->nErasedBlocks - dev->nReservedBlocks;

	T(YAFFS_TRACE_CHECKPOINT,
		(TSTR("checkpt blocks available = %d" TENDSTR),
		blocksAvailable));

	return (blocksAvailable <= 0) ? 0 : 1;
}


static int yaffs_CheckpointErase(yaffs_Device *dev)
{
	int i, chk = 0, old1 = 0, old2 = 0, yscData = 0, chunk, realignedChunk;
	yaffs_ExtendedTags tags;

	if (!dev->eraseBlockInNAND)
		return 0;
	
	for (i = dev->internalStartBlock; i <= dev->internalEndBlock; i++) {
		yaffs_BlockInfo *bi = yaffs_GetBlockInfo(dev, i);
		chunk = i * dev->nChunksPerBlock;
		realignedChunk = chunk - dev->chunkOffset;
		dev->readChunkWithTagsFromNAND(dev, realignedChunk,	NULL, &tags);
		if (tags.sequenceNumber == YAFFS_SEQUENCE_CHECKPOINT_DATA){
			yscData++;		
			if (bi->blockState == YAFFS_BLOCK_STATE_CHECKPOINT) 	chk++;
			if (bi->blockState == YAFFS_BLOCK_STATE_OLD1CHECKPOINT) old1++;
			if (bi->blockState == YAFFS_BLOCK_STATE_OLD2CHECKPOINT) old2++;
		}
		else{
			if ((bi->blockState == YAFFS_BLOCK_STATE_CHECKPOINT)	|| 
			   (bi->blockState == YAFFS_BLOCK_STATE_OLD1CHECKPOINT) || 
			   (bi->blockState == YAFFS_BLOCK_STATE_OLD2CHECKPOINT))
				bi->blockState = YAFFS_BLOCK_STATE_DIRTY;
		}
	} 
	//T(YAFFS_TRACE_ALWAYS,
	//(TSTR("\nINFO  STATISTICS BEFORE checkpointErase(), chk %d old1 %d old2 %d yscdata %d"TENDSTR),
	//chk, old1, old2, yscData));
	
	for (i = dev->internalStartBlock; i <= dev->internalEndBlock; i++) {
		yaffs_BlockInfo *bi = yaffs_GetBlockInfo(dev, i);
		if ((bi->blockState == YAFFS_BLOCK_STATE_OLD2CHECKPOINT) && 
		     ((chk != 0) || (old1 != 0))) {  

			 chunk = i * dev->nChunksPerBlock;
			 realignedChunk = chunk - dev->chunkOffset;
			 dev->readChunkWithTagsFromNAND(dev, realignedChunk, NULL, &tags);
			 if (tags.sequenceNumber == YAFFS_SEQUENCE_CHECKPOINT_DATA) {
				//MC this is really an old2 checkpoint block
				//T(YAFFS_TRACE_ALWAYS, (TSTR("INFO  Erasing OLD2 checkpt block %d"TENDSTR), i));
				deleteBlockChk(dev, i);
			}
		}
	
		if (bi->blockState == YAFFS_BLOCK_STATE_OLD1CHECKPOINT) { 
			//T(YAFFS_TRACE_ALWAYS, (TSTR("INFO  Block %d OLD1 -> OLD2"TENDSTR), i));
			bi->blockState = YAFFS_BLOCK_STATE_OLD2CHECKPOINT;
		}
		
		if ((bi->blockState == YAFFS_BLOCK_STATE_CHECKPOINT)&&
		    !(old2 > 0 && chk > 0) && !(old1 > 0 && chk > 0)) {  
			//MC avoid chkErase at boot time after uncommitted chkpt
			//T(YAFFS_TRACE_ALWAYS, (TSTR("INFO  Block %d CHK -> OLD1"TENDSTR), i));
			bi->blockState = YAFFS_BLOCK_STATE_OLD1CHECKPOINT;
			
		}
	}

	//MC stats after checkpointErase()
	//chk = 0, old1 = 0, old2 = 0, yscData = 0;
	//for (i = dev->internalStartBlock; i <= dev->internalEndBlock; i++) {
	//	yaffs_BlockInfo *bi = yaffs_GetBlockInfo(dev, i);
	//	if (bi->blockState == YAFFS_BLOCK_STATE_CHECKPOINT) chk++;
	//	if (bi->blockState == YAFFS_BLOCK_STATE_OLD1CHECKPOINT) old1++;
	//	if (bi->blockState == YAFFS_BLOCK_STATE_OLD2CHECKPOINT) old2++;
	//	chunk = i * dev->nChunksPerBlock;
	//	realignedChunk = chunk - dev->chunkOffset;
	//	dev->readChunkWithTagsFromNAND(dev, realignedChunk,	NULL, &tags);
	//	if (tags.sequenceNumber == YAFFS_SEQUENCE_CHECKPOINT_DATA) yscData++;
	//}
	//dev->blocksInCheckpoint = yscData;
	//T(YAFFS_TRACE_ALWAYS, (TSTR("\nINFO  STATISTICS AFTER checkpointErase(), chk %d old1 %d old2 %d yscdata %d"TENDSTR), chk, old1, old2, yscData));

	return 1;
}


static void yaffs_CheckpointFindNextErasedBlock(yaffs_Device *dev)
{
	int  i;
	int blocksAvailable = dev->nErasedBlocks - dev->nReservedBlocks;
	T(YAFFS_TRACE_CHECKPOINT,
		(TSTR("allocating checkpt block: erased %d reserved %d avail %d next %d "TENDSTR),
		dev->nErasedBlocks, dev->nReservedBlocks, blocksAvailable, dev->checkpointNextBlock));

	if (dev->checkpointNextBlock >= 0 &&
			dev->checkpointNextBlock <= dev->internalEndBlock &&
			blocksAvailable > 0) {

		for (i = dev->checkpointNextBlock; i <= dev->internalEndBlock; i++) {
			yaffs_BlockInfo *bi = yaffs_GetBlockInfo(dev, i);
			if (bi->blockState == YAFFS_BLOCK_STATE_EMPTY && 
			    bi->pagesInUse == 0) {                           //MC this condition should avoid 
				dev->checkpointNextBlock = i + 1;                //   uncorrectable errors ... 
				dev->checkpointCurrentBlock = i;
				//T(YAFFS_TRACE_ALWAYS, (("allocating checkpt block %d pgInUse %d"TENDSTR),
				//i, bi->pagesInUse));
				return;
			}
		}
	}
	T(YAFFS_TRACE_CHECKPOINT, (TSTR("out of checkpt blocks"TENDSTR)));

	dev->checkpointNextBlock = -1;
	dev->checkpointCurrentBlock = -1;
}

static void yaffs_CheckpointFindNextCheckpointBlock(yaffs_Device *dev, int recovery)
{
	int  i, countzz, skip;
	yaffs_ExtendedTags tags;
		
	if (dev->blocksInCheckpoint < dev->checkpointMaxBlocks)
		for (i = dev->checkpointNextBlock; i <= dev->internalEndBlock; i++) {
			yaffs_BlockInfo *bi = yaffs_GetBlockInfo(dev, i);
			int chunk = i * dev->nChunksPerBlock;
			int realignedChunk = chunk - dev->chunkOffset;
			
			dev->readChunkWithTagsFromNAND(dev, realignedChunk, NULL, &tags);
			T(YAFFS_TRACE_CHECKPOINT, 
			(TSTR(" next checkpt block: search: block %d status %d oid %d TAG %d eccr %d" TENDSTR),
			i, bi->blockState, tags.objectId, tags.sequenceNumber, tags.eccResult));
			
			skip = 0;						//MC don't skip yet the current block 
			
			if (dev->old2Masked[0] != -2)   //MC reliability control only in case of recovery
				for(countzz = 0; countzz < YAFFS_MAX_CHECKPOINT_BLOCKS; countzz++)
					if (dev->old2Masked[countzz] == i) {
						skip = 1;			//MC this block is masked, skip it
						break;
					}
					
			if (dev->old1Masked[0] != -2)   //MC reliability control only in case of recovery
				for(countzz = 0; countzz < YAFFS_MAX_CHECKPOINT_BLOCKS; countzz++)
					if (dev->old1Masked[countzz] == i) {
						skip = 1;			//MC this block is masked, skip it
						break;
					}					
			
			if ((skip) || (bi->blockState == YAFFS_BLOCK_STATE_ALMOST_DIRTY)) {  //MC strange case 
				//T(YAFFS_TRACE_ALWAYS, 
				//(TSTR("\nINFO  Skipped block %d because is masked"TENDSTR), i));
				continue; 					//MC don't use as checkpoint a block 
			}							    //   that has been masked in recovery 
											//   phase. 
				
			if (tags.sequenceNumber == YAFFS_SEQUENCE_CHECKPOINT_DATA) {
				/* Right kind of block */
				dev->checkpointNextBlock = tags.objectId;
				dev->checkpointCurrentBlock = i;
				dev->checkpointBlockList[dev->blocksInCheckpoint] = i;
				dev->blocksInCheckpoint++;
			
				//T(YAFFS_TRACE_ALWAYS, (TSTR("\nINFO  Found checkpt block %d blockSN %d "TENDSTR),
				//i, bi->sequenceNumber));
				return;
			}
		}

	T(YAFFS_TRACE_CHECKPOINT, (TSTR("found no more checkpt blocks"TENDSTR)));

	dev->checkpointNextBlock = -1;
	dev->checkpointCurrentBlock = -1;
}


int yaffs_CheckpointOpen(yaffs_Device *dev, int forWriting)
{


	dev->checkpointOpenForWrite = forWriting;

	/* Got the functions we need? */
	if (!dev->writeChunkWithTagsToNAND ||
			!dev->readChunkWithTagsFromNAND ||
			!dev->eraseBlockInNAND ||
			!dev->markNANDBlockBad)
		return 0;

	if (forWriting && !yaffs_CheckpointSpaceOk(dev))
		return 0;

	if (!dev->checkpointBuffer)
		dev->checkpointBuffer = YMALLOC_DMA(dev->totalBytesPerChunk);
	if (!dev->checkpointBuffer)
		return 0;
    
	dev->checkpointPageSequence = 0;
	dev->checkpointByteCount = 0;
	dev->checkpointSum = 0;
	dev->checkpointXor = 0;
	dev->checkpointCurrentBlock = -1;
	dev->checkpointCurrentChunk = -1;
	dev->checkpointNextBlock = dev->internalStartBlock;

	/* Erase all the blocks in the checkpoint area ONLY in case of new chkpt write*/
	if (forWriting) {
		memset(dev->checkpointBuffer, 0, dev->nDataBytesPerChunk);
		dev->checkpointByteOffset = 0;
	
		return yaffs_CheckpointErase(dev);
		
	} else {  //MC checkpointRead, this doesn't delete anything
		int i;
		/* Set to a value that will kick off a read */
		dev->checkpointByteOffset = dev->nDataBytesPerChunk;
		/* A checkpoint block list of 1 checkpoint block per 16 block is (hopefully)
		 * going to be way more than we need */
		dev->blocksInCheckpoint = 0;
		dev->checkpointMaxBlocks = (dev->internalEndBlock - dev->internalStartBlock)/16 + 2;
		//MC rough calculation of checkpoint blocks needed
		
		dev->checkpointBlockList = YMALLOC(sizeof(int) * dev->checkpointMaxBlocks);
		if(!dev->checkpointBlockList)
			return 0;
		for (i = 0; i < dev->checkpointMaxBlocks; i++)
			dev->checkpointBlockList[i] = -1;
	}

	return 1;
}

int yaffs_GetCheckpointSum(yaffs_Device *dev, __u32 *sum)
{
	__u32 compositeSum;
	compositeSum =  (dev->checkpointSum << 8) | (dev->checkpointXor & 0xFF);
	*sum = compositeSum;
	return 1;
}

static int yaffs_CheckpointFlushBuffer(yaffs_Device *dev)
{
	int chunk;
	int realignedChunk;

	yaffs_ExtendedTags tags;

	if (dev->checkpointCurrentBlock < 0) {
		yaffs_CheckpointFindNextErasedBlock(dev); //MC find next block where the chkpt can be written
		dev->checkpointCurrentChunk = 0;
	}

	if (dev->checkpointCurrentBlock < 0)
		return 0;  //MC this is an error

	tags.chunkDeleted = 0;
	tags.objectId = dev->checkpointNextBlock; /* Hint to next place to look */
	tags.chunkId = dev->checkpointPageSequence + 1;
	tags.sequenceNumber =  YAFFS_SEQUENCE_CHECKPOINT_DATA;
	tags.byteCount = dev->nDataBytesPerChunk;
	if (dev->checkpointCurrentChunk == 0) {
		/* First chunk we write for the block? Set block state to
		   checkpoint */
		yaffs_BlockInfo *bi = yaffs_GetBlockInfo(dev, dev->checkpointCurrentBlock);
		bi->blockState = YAFFS_BLOCK_STATE_CHECKPOINT;
		dev->blocksInCheckpoint++;  //MC this block is for checkpointing
	}

	chunk = dev->checkpointCurrentBlock * dev->nChunksPerBlock + dev->checkpointCurrentChunk;

	//T(YAFFS_TRACE_ALWAYS, (TSTR("INFO  Chkp w buffer chunk %d(currBLOCK %d: curCHUNK %d) objid %d chId %d" TENDSTR),
	//chunk, dev->checkpointCurrentBlock, dev->checkpointCurrentChunk, tags.objectId, tags.chunkId));
	
	realignedChunk = chunk - dev->chunkOffset;

	dev->nPageWrites++;
    //MC here is where the chkpt blocks are written on NAND
	dev->writeChunkWithTagsToNAND(dev, realignedChunk,
			dev->checkpointBuffer, &tags);
	dev->checkpointByteOffset = 0;
	dev->checkpointPageSequence++;
	dev->checkpointCurrentChunk++;
	if (dev->checkpointCurrentChunk >= dev->nChunksPerBlock) {
		dev->checkpointCurrentChunk = 0;
		dev->checkpointCurrentBlock = -1;
	}
	memset(dev->checkpointBuffer, 0, dev->nDataBytesPerChunk);

	return 1;
}


int yaffs_CheckpointWrite(yaffs_Device *dev, const void *data, int nBytes)
{
	int i = 0;
	int ok = 1;

	__u8 * dataBytes = (__u8 *)data;

	if (!dev->checkpointBuffer)
		return 0;

	if (!dev->checkpointOpenForWrite)
		return -1;

	while (i < nBytes && ok) {
		dev->checkpointBuffer[dev->checkpointByteOffset] = *dataBytes;
		dev->checkpointSum += *dataBytes;
		dev->checkpointXor ^= *dataBytes;
		dev->checkpointByteOffset++;
		i++;
		dataBytes++;
		dev->checkpointByteCount++;

		if (dev->checkpointByteOffset < 0 ||
		   dev->checkpointByteOffset >= dev->nDataBytesPerChunk)
			ok = yaffs_CheckpointFlushBuffer(dev);
	}

	return i;
}

int yaffs_CheckpointRead(yaffs_Device *dev, void *data, int nBytes, int recovery)
{
	int i = 0;
	int ok = 1;
	yaffs_ExtendedTags tags;
	int readErrorCheck;			
	int count, bfound = 0;	

	int chunk;
	int realignedChunk;

	__u8 *dataBytes = (__u8 *)data;

	if (!dev->checkpointBuffer)
		return 0;

	if (dev->checkpointOpenForWrite)
		return -1;

	while (i < nBytes && ok) {

		if (dev->checkpointByteOffset < 0 ||
			dev->checkpointByteOffset >= dev->nDataBytesPerChunk) {

			if (dev->checkpointCurrentBlock < 0) {
				yaffs_CheckpointFindNextCheckpointBlock(dev, recovery);
				dev->checkpointCurrentChunk = 0;
			}
			
			//MC here we take note of the chk blocks read (useful in case of recovery);
			//   the current checkpoint block is dev->checkpointCurrentBlock.

			for (count = 0; count < YAFFS_MAX_CHECKPOINT_BLOCKS; count ++) {	
				if (dev->lastChkBlocksTried[count] == dev->checkpointCurrentBlock){
				bfound = 1;
				break;
				}
			}
			if ((!bfound) && (dev->checkpointCurrentBlock != -1) && (dev->checkpointCurrentBlock != -2)) {
				for (count = 0; count < YAFFS_MAX_CHECKPOINT_BLOCKS; count ++) {
					if (dev->lastChkBlocksTried[count] == -2 ){
						dev->lastChkBlocksTried[count] = dev->checkpointCurrentBlock; 
						break;
					}
				}
			}			

			if (dev->checkpointCurrentBlock < 0)
				ok = 0;
			else {
				chunk = dev->checkpointCurrentBlock *
					dev->nChunksPerBlock +
					dev->checkpointCurrentChunk;

				realignedChunk = chunk - dev->chunkOffset;
				
				dev->nPageReads++;
				
				// read in the next chunk
				readErrorCheck = dev->readChunkWithTagsFromNAND(dev, realignedChunk, 
																dev->checkpointBuffer, &tags);
																
				if (!(readErrorCheck > 0)) { 				
					//MC uncorrectable error! let's cheat.
					T(YAFFS_TRACE_ALWAYS, (("\nFAIL  checkpointRead uncorrectable error, block %d "TENDSTR),
					dev->checkpointCurrentBlock));
					i++;
					dev->checkpointByteOffset = 0;
					dev->checkpointPageSequence++;
					dev->checkpointCurrentChunk++;
					dev->checkpointByteOffset++;
					i++;
					dataBytes++;
					dev->checkpointByteCount++;
					return i;  									
				}
																

				if (tags.chunkId != (dev->checkpointPageSequence + 1) ||
					tags.eccResult > YAFFS_ECC_RESULT_FIXED ||
					tags.sequenceNumber != YAFFS_SEQUENCE_CHECKPOINT_DATA)
					ok = 0;

				dev->checkpointByteOffset = 0;
				dev->checkpointPageSequence++;
				dev->checkpointCurrentChunk++;

				if (dev->checkpointCurrentChunk >= dev->nChunksPerBlock){
					dev->checkpointCurrentBlock = -1;}
			}
		}

		if (ok) {
			*dataBytes = dev->checkpointBuffer[dev->checkpointByteOffset];
			dev->checkpointSum += *dataBytes;
			dev->checkpointXor ^= *dataBytes;
			dev->checkpointByteOffset++;
			i++;
			dataBytes++;
			dev->checkpointByteCount++;
		}
	}

	return 	i;
}

int yaffs_CheckpointClose(yaffs_Device *dev, int ok)
{
	int i;
	yaffs_ExtendedTags tags;
	//T(YAFFS_TRACE_ALWAYS, (TSTR("\n***** MC checkpointClose()"TENDSTR)));
	
	//MC sanity check on tags
	int chk = 0, old1 = 0, old2 = 0, yscData = 0;
	for (i = dev->internalStartBlock; i <= dev->internalEndBlock; i++) {
		yaffs_BlockInfo *bi = yaffs_GetBlockInfo(dev, i);
		int chunk = i * dev->nChunksPerBlock;
		int realignedChunk = chunk - dev->chunkOffset;
		dev->readChunkWithTagsFromNAND(dev, realignedChunk,	NULL, &tags);
		if (tags.sequenceNumber == YAFFS_SEQUENCE_CHECKPOINT_DATA){
																	yscData++;		
			if (bi->blockState == YAFFS_BLOCK_STATE_CHECKPOINT) 	chk++;
			if (bi->blockState == YAFFS_BLOCK_STATE_OLD1CHECKPOINT) old1++;
			if (bi->blockState == YAFFS_BLOCK_STATE_OLD2CHECKPOINT) old2++;
		}
		else{
			if ((bi->blockState == YAFFS_BLOCK_STATE_CHECKPOINT) 	|| 
			   (bi->blockState == YAFFS_BLOCK_STATE_OLD1CHECKPOINT) || 
			   (bi->blockState == YAFFS_BLOCK_STATE_OLD2CHECKPOINT))
				bi->blockState = YAFFS_BLOCK_STATE_DIRTY;
		}
	} 
	
	if (dev->checkpointOpenForWrite) {
		if (dev->checkpointByteOffset != 0){
			yaffs_CheckpointFlushBuffer(dev);

			if (ok) {
			for (i = dev->internalStartBlock; i <= dev->internalEndBlock; i++) {
				yaffs_BlockInfo *bi = yaffs_GetBlockInfo(dev, i);
				if ((bi->blockState == YAFFS_BLOCK_STATE_OLD2CHECKPOINT) && 
				((chk != 0) || (old1 != 0))) {  //MC we cannot delete old2 if chk = 0 and old1 = 0
			 
					//MC making sure that we're deleting really a checkpoint block, not data
					int chunk = i * dev->nChunksPerBlock;
					int realignedChunk = chunk - dev->chunkOffset;
					dev->readChunkWithTagsFromNAND(dev, realignedChunk,NULL, &tags);
					//MC FIXME here i should check if the chunk is uncorr
					if (tags.sequenceNumber == YAFFS_SEQUENCE_CHECKPOINT_DATA){
						//MC this is really an old2 checkpoint block
						//T(YAFFS_TRACE_ALWAYS, (TSTR("INFO  Erasing OLD checkpt block %d"TENDSTR), i));
						deleteBlockChk(dev, i);
					}
				}
			} //MC end old2 chkpt deletion
			} //MC end if ok
		}
		
	} else if(dev->checkpointBlockList){ //MC closing a checkpointRead...
		for (i = 0; i < dev->blocksInCheckpoint && dev->checkpointBlockList[i] >= 0; i++) {
			int blk = dev->checkpointBlockList[i];
			yaffs_BlockInfo *bi = NULL;
			if( dev->internalStartBlock <= blk && blk <= dev->internalEndBlock)
				bi = yaffs_GetBlockInfo(dev, blk);
			if (bi && bi->blockState == YAFFS_BLOCK_STATE_EMPTY){
				bi->blockState = YAFFS_BLOCK_STATE_CHECKPOINT;
				//T(YAFFS_TRACE_ALWAYS, (TSTR("INFO  Block %d is a checkpoint, bSN %d " TENDSTR),
				//blk, bi->sequenceNumber));
			}
		}

		YFREE(dev->checkpointBlockList);
		dev->checkpointBlockList = NULL;
	}
	
	dev->nFreeChunks -= dev->blocksInCheckpoint * dev->nChunksPerBlock;
	dev->nErasedBlocks -= dev->blocksInCheckpoint;

	//MC stats after checkpointClose()
	//chk = 0, old1 = 0, old2 = 0, yscData = 0;
	//for (i = dev->internalStartBlock; i <= dev->internalEndBlock; i++) {
	//	yaffs_BlockInfo *bi = yaffs_GetBlockInfo(dev, i);
	//	int chunk = i * dev->nChunksPerBlock;
	//	int realignedChunk = chunk - dev->chunkOffset;
	//	dev->readChunkWithTagsFromNAND(dev, realignedChunk,	NULL, &tags);
	//	if (tags.sequenceNumber == YAFFS_SEQUENCE_CHECKPOINT_DATA) {
	//																	yscData++;
	//		if (bi->blockState == YAFFS_BLOCK_STATE_CHECKPOINT) 		chk++;
	//		if (bi->blockState == YAFFS_BLOCK_STATE_OLD1CHECKPOINT) 	old1++;
	//		if (bi->blockState == YAFFS_BLOCK_STATE_OLD2CHECKPOINT) 	old2++;	
	//	}
	//} 
	//dev->blocksInCheckpoint = yscData;
	//T(YAFFS_TRACE_ALWAYS, (TSTR("\nINFO  STATISTICS AFTER checkpointclose(), chk %d old1 %d old2 %d yscdata %d"TENDSTR), chk, old1, old2, yscData));
	//T(YAFFS_TRACE_CHECKPOINT, (TSTR("INFO  Checkpoint byte count %d" TENDSTR),dev->checkpointByteCount));

	if (dev->checkpointBuffer) {
		/* free the buffer */
		YFREE(dev->checkpointBuffer);
		dev->checkpointBuffer = NULL;
		return 1;
	} else
		return 0;
}

int yaffs_CheckpointInvalidateStream(yaffs_Device *dev)
{
	/* Erase the checkpoint data */

	T(YAFFS_TRACE_CHECKPOINT, (TSTR("checkpoint invalidate of %d blocks"TENDSTR),
		dev->blocksInCheckpoint));

	return yaffs_CheckpointErase(dev);
}



