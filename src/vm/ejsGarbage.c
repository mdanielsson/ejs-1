/**
 *  ejsGarbage.c - EJS Garbage collector.
 *
 *  This implements a non-compacting, generational mark and sweep collection algorithm with 
 *  fast pooled object allocations.
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 */
/********************************** Includes **********************************/

#include    "ejs.h"

/****************************** Forward Declarations **************************/

static void mark(Ejs *ejs, int generation);
static void markGlobal(Ejs *ejs, int generation);
static inline bool memoryUsageOk(Ejs *ejs);
static inline void pruneTypePools(Ejs *ejs);
static void resetMarks(Ejs *ejs);
static void sweep(Ejs *ejs, int generation);

#if BLD_DEBUG
/*
 *  For debugging it can be helpful to disable the pooling of objects. Because mprFree will fill freed objects in 
 *  debug mode - bad frees will show up quickly with seg faults.
 *
 *  Break on the allocation, mark and freeing of a nominated object
 *  Set ejsBreakAddr to the address of the object to watch and set a breakpoint in the debugger below.
 *  Set ejsBreakSeq to the object sequence number to watch and set a breakpoint in the debugger below.
 */
static void *ejsBreakAddr = (void*) 0;
static int ejsBreakSeq = 0;
static void checkAddr(EjsVar *addr) {
    if ((void*) addr == ejsBreakAddr) { 
        /* Set a breakpoint here */
        addr = ejsBreakAddr;
    }
    if (addr->seq == ejsBreakSeq) {
        addr->seq = ejsBreakSeq;
    }
}
/*
 *  Unique allocation sequence. Helps GC debugging.
 */
static int nextSequence;

#else /* !BLD_DEBUG */
#define checkAddr(addr)
#undef ejsAddToGcStats
#define ejsAddToGcStats(ejs, vp, id)
#endif

/************************************* Code ***********************************/
/*
 *  Create the GC service
 */
int ejsCreateGCService(Ejs *ejs)
{
    EjsGC       *gc;
    int         i;

    mprAssert(ejs);

    gc = &ejs->gc;
    gc->enabled = !(ejs->flags & (EJS_FLAG_EMPTY));
    gc->firstGlobal = ES_global_NUM_CLASS_PROP;
    gc->numPools = EJS_MAX_TYPE;
    gc->allocGeneration = EJS_GEN_ETERNAL;
    ejs->workQuota = EJS_GC_WORK_QUOTA;

    for (i = 0; i < EJS_MAX_GEN; i++) {
        gc->generations[i] = mprAllocObjZeroed(ejs->heap, EjsGen);
    }
    for (i = 0; i < EJS_MAX_TYPE; i++) {
        gc->pools[i] = mprAllocObjZeroed(ejs->heap, EjsPool);
    }
    ejs->currentGeneration = ejs->gc.generations[EJS_GEN_ETERNAL];
    return 0;
}


void ejsDestroyGCService(Ejs *ejs)
{
    EjsGC       *gc;
    EjsGen      *gen;
    EjsVar      *vp;
    MprBlk      *bp, *next;
    int         generation;
    
    gc = &ejs->gc;
    for (generation = EJS_GEN_ETERNAL; generation >= 0; generation--) {
        gen = gc->generations[generation];
        for (bp = mprGetFirstChild(gen); bp; bp = next) {
            next = bp->next;
            vp = MPR_GET_PTR(bp);
            checkAddr(vp);
            if (vp->type->needFinalize) {
                (vp->type->helpers->destroyVar)(ejs, vp);
            }
        }
    }
}


/*
 *  Allocate a new variable. Size is set to the extra bytes for properties in addition to the type's instance size.
 */
EjsVar *ejsAllocVar(Ejs *ejs, EjsType *type, int extra)
{
    EjsVar      *vp;

    mprAssert(type);

    if ((vp = (EjsVar*) mprAllocZeroed(ejsGetAllocCtx(ejs), type->instanceSize + extra)) == 0) {
        ejsThrowMemoryError(ejs);
        return 0;
    }
    vp->type = type;
    vp->master = (ejs->master == 0);
    ejsAddToGcStats(ejs, vp, type->id);
    if (++ejs->workDone >= ejs->workQuota) {
        ejs->gcRequired = 1;
        ejs->attention = 1;
    }
    return (EjsVar*) vp;
}


EjsVar *ejsAllocPooledVar(Ejs *ejs, int id)
{
    EjsPool     *pool;
    EjsVar      *vp;
    MprBlk      *bp, *gp;

    if (id < ejs->gc.numPools) {
        pool = ejs->gc.pools[id];
        if ((bp = mprGetFirstChild(pool)) != NULL) {
            /*
             *  Transfer from the pool to the current generation. Inline for speed.
             */
            gp = MPR_GET_BLK(ejs->currentGeneration);
            if (bp->prev) {
                bp->prev->next = bp->next;
            } else {
                bp->parent->children = bp->next;
            }
            if (bp->next) {
                bp->next->prev = bp->prev;
            }
            bp->parent = gp;
            if (gp->children) {
                gp->children->prev = bp;
            }
            bp->next = gp->children;
            gp->children = bp;
            bp->prev = 0;

            vp = MPR_GET_PTR(bp);
            memset(vp, 0, pool->type->instanceSize);
            vp->type = pool->type;
            vp->master = (ejs->master == 0);
#if BLD_DEBUG
            vp->seq = nextSequence++;
            checkAddr((EjsVar*) vp);
            pool->reuse++;
            pool->count--;
            mprAssert(pool->count >= 0);
            ejsAddToGcStats(ejs, vp, id);
#endif
            if (++ejs->workDone >= ejs->workQuota) {
                ejs->gcRequired = 1;
                ejs->attention = 1;
            }
            return vp;
        }
    }
    return 0;
}


/*
 *  Free a variable. This is should only ever be called by the destroyVar helpers to free a object or recycle the 
 *  object to a type specific free pool. 
 */
void ejsFreeVar(Ejs *ejs, EjsVar *vp, int id)
{
    EjsType     *type;
    EjsPool     *pool;
    EjsGC       *gc;
    MprBlk      *bp, *pp;

    mprAssert(vp);
    checkAddr(vp);

    gc = &ejs->gc;
    type = vp->type;
    if (id < 0) {
        id = type->id;
    }
    pool = gc->pools[id];

    if (!vp->noPool && !type->dontPool && 0 <= id && id < gc->numPools && pool->count < EJS_MAX_TYPE_POOL) {
        /*
         *  Transfer from the current generation back to the pool. Inline for speed.
         */
        pool->type = vp->type; 
        pp = MPR_GET_BLK(pool);
        bp = MPR_GET_BLK(vp);
        if (bp->prev) {
            bp->prev->next = bp->next;
        } else {
            bp->parent->children = bp->next;
        }
        if (bp->next) {
            bp->next->prev = bp->prev;
        }
        if (bp->children) {
            mprFreeChildren(vp);
        }
        /*
         *  Add to the pool
         */
        bp->parent = pp;
        if (pp->children) {
            pp->children->prev = bp;
        }
        bp->next = pp->children;
        pp->children = bp;
        bp->prev = 0;

#if BLD_DEBUG
        vp->type = (void*) -1;
        pool->allocated--;
        mprAssert(pool->allocated >= 0);
        pool->count++;
        if (pool->count > pool->peakCount) {
            pool->peakCount = pool->count;
        }
#endif
    } else {
#if BLD_DEBUG
        vp->type = (void*) -1;
        if (0 <= id && id < gc->numPools) {
            pool = gc->pools[id];
            pool->allocated--;
            mprAssert(pool->allocated >= 0);
        }
#endif
        mprFree(vp);
    }
}


/*
 *  Collect the garbage. This is a mark and sweep over all possible objects. If an object is not referenced, it and 
 *  all contained properties will be freed. Collection is done in generations.
 */
void ejsCollectGarbage(Ejs *ejs, int gen)
{
    EjsGC       *gc;
    
    gc = &ejs->gc;
    if (!gc->enabled || gc->collecting || !ejs->initialized) {
        return;
    }
    gc->collecting = 1;

    mark(ejs, gen);
    sweep(ejs, gen);
    if (!memoryUsageOk(ejs)) {
        pruneTypePools(ejs);
    }
    ejs->workDone = 0;
    ejs->gcRequired = 0;
    gc->collecting = 0;
#if BLD_DEBUG
    gc->totalSweeps++;
#if REPORT
    ejsPrintAllocReport(ejs);
    mprPrintAllocReport(ejs, "Memory Report");
#endif
#endif
}


/*
 *  Mark phase. Mark objects that are still in use and should not be collected.
 */
static void mark(Ejs *ejs, int generation)
{
    EjsModule       *mp;
    EjsGC           *gc;
    EjsBlock        *block;
    EjsVar          *vp, **sp, **top;
    int             next;

    gc = &ejs->gc;
    gc->collectGeneration = generation;

    resetMarks(ejs);
    markGlobal(ejs, generation);

    if (ejs->result) {
        ejsMarkVar(ejs, NULL, ejs->result);
    }
    if (ejs->exception) {
        ejsMarkVar(ejs, NULL, ejs->exception);
    }
    if (ejs->exceptionArg) {
        ejsMarkVar(ejs, NULL, ejs->exceptionArg);
    }
    if (ejs->memoryCallback) {
        ejsMarkVar(ejs, NULL, (EjsVar*) ejs->memoryCallback);
    }
    if (ejs->sessions) {
        ejsMarkVar(ejs, NULL, (EjsVar*) ejs->sessions);
    }

    /*
     *  Mark initializers
     */
    for (next = 0; (mp = (EjsModule*) mprGetNextItem(ejs->modules, &next)) != 0;) {
        if (mp->initializer) {
            ejsMarkVar(ejs, NULL, (EjsVar*) mp->initializer);
        }
    }

    /*
     *  Mark blocks. This includes frames and blocks.
     */
    for (block = ejs->state->bp; block; block = block->prev) {
        ejsMarkVar(ejs, NULL, (EjsVar*) block);
    }

    /*
     *  Mark the evaluation stack
     */
    top = ejs->state->stack;
    for (sp = ejs->state->stackBase; sp <= top; sp++) {
        if ((vp = *sp) != NULL) {
            ejsMarkVar(ejs, NULL, vp);
        }
    }
}


/*
 *  Sweep up the garbage for a given generation
 */
static void sweep(Ejs *ejs, int maxGeneration)
{
    EjsVar      *vp;
    EjsGC       *gc;
    EjsGen      *gen;
    MprBlk      *bp, *next;
    int         destroyed, generation;
    
    /*
     *  Go from oldest to youngest incase moving objects to elder generations and we clear the mark.
     */
    gc = &ejs->gc;
    for (generation = maxGeneration; generation >= 0; generation--) {
        gc->collectGeneration = generation;
        gen = gc->generations[generation];

        for (destroyed = 0, bp = mprGetFirstChild(gen); bp; bp = next) {
            next = bp->next;
            vp = MPR_GET_PTR(bp);
            checkAddr(vp);
            if (!vp->marked && !vp->permanent) {
                (vp->type->helpers->destroyVar)(ejs, vp);
                destroyed++;
            }
        }
#if BLD_DEBUG
        gc->allocatedObjects -= destroyed;
        gc->totalReclaimed += destroyed;
        gen->totalReclaimed += destroyed;
        gen->totalSweeps++;
#endif
    }
}


/*
    Reset all marks prior to doing a mark/sweep
 */
static void resetMarks(Ejs *ejs)
{
    EjsGen      *gen;
    EjsGC       *gc;
    EjsVar      *vp;
    EjsBlock    *block, *b;
    MprBlk      *bp;
    int         i;

    gc = &ejs->gc;
    for (i = 0; i < EJS_MAX_GEN; i++) {
        gen = gc->generations[i];
        for (bp = mprGetFirstChild(gen); bp; bp = bp->next) {
            vp = MPR_GET_PTR(bp);
            vp->marked = 0;
        }
    }
    for (block = ejs->state->bp; block; block = block->prev) {
        block->obj.var.marked = 0;
        if (block->prevException) {
            block->prevException->marked = 0;
        }
        for (b = block->scopeChain; b; b = b->scopeChain) {
            b->obj.var.marked = 0;
        }
    }
}

    
/*
    Mark the global object
 */
static void markGlobal(Ejs *ejs, int generation)
{
    EjsGC       *gc;
    EjsObject   *obj;
    EjsBlock    *block;
    EjsVar      *item;
    MprHash     *hp;
    int         i, next;

    gc = &ejs->gc;

    obj = (EjsObject*) ejs->global;
    obj->var.marked = 1;

    if (generation == EJS_GEN_ETERNAL) {
        for (i = 0; i < obj->numProp; i++) {
            ejsMarkVar(ejs, NULL, obj->slots[i]);
        }
        for (hp = 0; (hp = mprGetNextHash(ejs->standardSpaces, hp)) != 0; ) {
            ejsMarkVar(ejs, NULL, (EjsVar*) hp->data);
        }

    } else {
        for (i = gc->firstGlobal; i < obj->numProp; i++) {
            ejsMarkVar(ejs, NULL, obj->slots[i]);
        }
    }
    block = ejs->globalBlock;
    if (block->prevException) {
        ejsMarkVar(ejs, (EjsVar*) block, (EjsVar*) block->prevException);
    }
    if (block->namespaces.length > 0) {
        for (next = 0; ((item = (EjsVar*) ejsGetNextItem(&block->namespaces, &next)) != 0); ) {
            ejsMarkVar(ejs, (EjsVar*) block, item);
        }
    }
}


/*
 *  Mark a variable as used. All variable marking comes through here.
 *  NOTE: The container is not used by anyone (verified).
 */
void ejsMarkVar(Ejs *ejs, EjsVar *container, EjsVar *vp)
{
    if (vp && !vp->marked) {
        checkAddr(vp);
        vp->marked = 1;
        (vp->type->helpers->markVar)(ejs, container, vp);
    }
}


static inline bool memoryUsageOk(Ejs *ejs)
{
    MprAlloc    *alloc;
    int64        memory;

    memory = mprGetUsedMemory(ejs);
    alloc = mprGetAllocStats(ejs);
    return memory < alloc->redLine;
}


static inline void pruneTypePools(Ejs *ejs)
{
    EjsPool     *pool;
    EjsGC       *gc;
    EjsVar      *vp;
    MprAlloc    *alloc;
    MprBlk      *bp, *next;
    int64       memory;
    int         i;

    gc = &ejs->gc;

    /*
     *  Still insufficient memory, must reclaim all objects from the type pools.
     */
    for (i = 0; i < gc->numPools; i++) {
        pool = gc->pools[i];
        if (pool->count) {
            for (bp = mprGetFirstChild(pool); bp; bp = next) {
                next = bp->next;
                vp = MPR_GET_PTR(bp);
                mprFree(vp);
            }
            pool->count = 0;
        }
    }
    gc->totalRedlines++;

    memory = mprGetUsedMemory(ejs);
    alloc = mprGetAllocStats(ejs);

    if (memory >= alloc->maxMemory) {
        /*
         *  Could not provide sufficient memory. Go into graceful degrade mode
         */
        ejsThrowMemoryError(ejs);
        ejsGracefulDegrade(ejs);
    }
}


/*
 *  Make all eternal allocations permanent. This prevents an eternal GC from collecting core essential values like
 *  ejs->zeroValue. Do this to keep markGlobal() simple, otherwise it would have to enumerate values like this.
 */
void ejsMakeEternalPermanent(Ejs *ejs)
{
    EjsGen      *gen;
    EjsVar      *vp;
    MprBlk      *bp;

    gen = ejs->gc.generations[EJS_GEN_ETERNAL];
    for (bp = mprGetFirstChild(gen); bp; bp = bp->next) {
        vp = MPR_GET_PTR(bp);
        vp->permanent = 1;
    }
}


/*
 *  Permanent objects are never freed
 */
void ejsMakePermanent(Ejs *ejs, EjsVar *vp)
{
    vp->permanent = 1;
}


void ejsMakeTransient(Ejs *ejs, EjsVar *vp)
{
    vp->permanent = 0;
}


/*
 *  Return true if there is time to do a garbage collection and if we will benefit from it.
 *  Currently not used.
 */
int ejsIsTimeForGC(Ejs *ejs, int timeTillNextEvent)
{
    EjsGC       *gc;

    if (timeTillNextEvent < EJS_MIN_TIME_FOR_GC) {
        /*
         *  This is a heuristic where we want a good amount of idle time so that a proactive garbage collection won't 
         *  delay any I/O events.
         */
        return 0;
    }

    /*
     *  Return if we haven't done enough work to warrant a collection. Trigger a little short of the work quota to try 
     *  to run GC before a demand allocation requires it.
     */
    gc = &ejs->gc;
    if (!gc->enabled || ejs->workDone < (ejs->workQuota - EJS_GC_SHORT_WORK_QUOTA)) {
        return 0;
    }
    mprLog(ejs, 7, "Time for GC. Work done %d, time till next event %d", ejs->workDone, timeTillNextEvent);
    return 1;
}


int ejsEnableGC(Ejs *ejs, bool on)
{
    int     old;

    old = ejs->gc.enabled;
    ejs->gc.enabled = on;
    return old;
}


/*
 *  On a memory allocation failure, go into graceful degrade mode. Set all slab allocation chunk increments to 1 
 *  so we can create an exception block to throw.
 */
void ejsGracefulDegrade(Ejs *ejs)
{
    mprLog(ejs, 1, "WARNING: Memory almost depleted. In graceful degrade mode");
    ejs->gc.degraded = 1;
    mprSignalExit(ejs);
}


int ejsSetGeneration(Ejs *ejs, int generation)
{
    int     old;
    
    old = ejs->gc.allocGeneration;
    ejs->gc.allocGeneration = generation;
    ejs->currentGeneration = ejs->gc.generations[generation];
    return old;
}


#undef ejsAddToGcStats

/*
 *  Update GC stats for a new object allocation
 */
void ejsAddToGcStats(Ejs *ejs, EjsVar *vp, int id)
{
#if BLD_DEBUG
    EjsPool     *pool;
    EjsGC       *gc;

    gc = &ejs->gc;
    if (id < ejs->gc.numPools) {
        pool = ejs->gc.pools[id];
        pool->allocated++;
        mprAssert(pool->allocated >= 0);
        if (pool->allocated > pool->peakAllocated) {
            pool->peakAllocated = pool->allocated;
        }
    }
    gc->totalAllocated++;
    gc->allocatedObjects++;
    if (gc->allocatedObjects >= gc->peakAllocatedObjects) {
        gc->peakAllocatedObjects = gc->allocatedObjects;
    }
    if (vp->type == ejs->typeType) {
        gc->allocatedTypes++;
        if (gc->allocatedTypes >= gc->peakAllocatedTypes) {
            gc->peakAllocatedTypes = gc->allocatedTypes;
        }
    }
    /* Convenient place for this */
    vp->seq = nextSequence++;
    checkAddr(vp);
#endif
}


void ejsPrintAllocReport(Ejs *ejs)
{
#if BLD_DEBUG
    EjsType         *type;
    EjsGC           *gc;
    EjsPool         *pool;
    int             i, typeMemory, count, peakCount, freeCount, peakFreeCount, reuseCount;

    gc = &ejs->gc;
    
    /*
     *  EJS stats
     */
    mprLog(ejs, 0, "\n\nEJS Memory Statistics");
    mprLog(ejs, 0, "  Types allocated        %,14d", gc->allocatedTypes / 2);
    mprLog(ejs, 0, "  Objects allocated      %,14d", gc->allocatedObjects);
    mprLog(ejs, 0, "  Peak objects allocated %,14d", gc->peakAllocatedObjects);

    /*
     *  Per type
     */
    mprLog(ejs, 0, "\nObject Cache Statistics");
    mprLog(ejs, 0, "------------------------");
    mprLog(ejs, 0, "Name                TypeSize  ObjectSize  ObjectCount  PeakCount  FreeList  PeakFreeList   ReuseCount");
    
    typeMemory = 0;
    count = peakCount = freeCount = peakFreeCount = reuseCount = 0;
    for (i = 0; i < gc->numPools; i++) {
        pool = gc->pools[i];
        type = ejsGetType(ejs, i);
        if (type == 0) {
            continue;
        }
        if (type->id != i) {
            /* Skip type alias (string == String) */
            continue;
        }
        mprLog(ejs, 0, "%-22s %,5d %,8d %,10d  %,10d, %,9d, %,10d, %,14d", type->qname.name, ejsGetTypeSize(ejs, type), 
            type->instanceSize, pool->allocated, pool->peakAllocated, pool->count, pool->peakCount, pool->reuse);

        typeMemory += ejsGetTypeSize(ejs, type);
        count += pool->allocated;
        peakCount += pool->peakAllocated;
        freeCount += pool->count;
        peakFreeCount += pool->peakCount;
        reuseCount += pool->reuse;
    }
    mprLog(ejs, 0, "%-22s                %,10d  %,10d, %,9d, %,10d, %,14d", "Total", 
        count, peakCount, freeCount, peakFreeCount, reuseCount);
    mprLog(ejs, 0, "\nTotal type memory        %,14d K", typeMemory / 1024);

    mprLog(ejs, 0, "\nEJS Garbage Collector Statistics");
    mprLog(ejs, 0, "  Total allocations      %,14d", gc->totalAllocated);
    mprLog(ejs, 0, "  Total reclaimations    %,14d", gc->totalReclaimed);
    mprLog(ejs, 0, "  Total sweeps           %,14d", gc->totalSweeps);
    mprLog(ejs, 0, "  Total redlines         %,14d", gc->totalRedlines);
    mprLog(ejs, 0, "  Object GC work quota   %,14d", ejs->workQuota);
#endif
}

/*
 *  @copy   default
 *
 *  Copyright (c) Embedthis Software LLC, 2003-2011. All Rights Reserved.
 *  Copyright (c) Michael O'Brien, 1993-2011. All Rights Reserved.
 *
 *  This software is distributed under commercial and open source licenses.
 *  You may use the GPL open source license described below or you may acquire
 *  a commercial license from Embedthis Software. You agree to be fully bound
 *  by the terms of either license. Consult the LICENSE.TXT distributed with
 *  this software for full details.
 *
 *  This software is open source; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation; either version 2 of the License, or (at your
 *  option) any later version. See the GNU General Public License for more
 *  details at: http://www.embedthis.com/downloads/gplLicense.html
 *
 *  This program is distributed WITHOUT ANY WARRANTY; without even the
 *  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  This GPL license does NOT permit incorporating this software into
 *  proprietary programs. If you are unable to comply with the GPL, you must
 *  acquire a commercial license to use this software. Commercial licenses
 *  for this software and support services are available from Embedthis
 *  Software at http://www.embedthis.com
 *
 *  Local variables:
    tab-width: 4
    c-basic-offset: 4
    End:
    vim: sw=4 ts=4 expandtab

    @end
 */
