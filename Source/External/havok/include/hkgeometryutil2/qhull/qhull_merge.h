/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */

//----------------------------------------------------------------
//
// The following changes have been made to the QHull Code:
//
// 1. All filenames have been changed from xxx to qhull_xxx
//
// 2. All uses of NULL have been replaced with HK_NULL	
//
// 3. All i/o statments have been replaced as follows:
//		fprintf() has been replaced with an empty function FPRINTF_REPLACEMENT
//		fputs() has been replaced with an empty function FPUTS_REPLACEMENT
//		fgets() has been replaced with an empty function FGETS_REPLACEMENT
//		freopen() has been replaced with an empty function FREOPEN_REPLACEMENT
//
// 4. The memory management has been changed to use havok's memory management
//		malloc() has been replaced with the function hkAllocate()
//		free() has been replaced with the function hkDeallocate()
//		The implementations of hkAllocate and hkDeallocate may be found in hkBase/memory
//
// By: Nick Gray
// Date: 19/10/2001
//
// 5. 'char*' has been changed to 'const char*' where appropriate
// Stephen Kennedy 05/06/2002
//
//----------------------------------------------------------------
/*<html><pre>  -<a                             href="qh-merge.htm"
  >-------------------------------</a><a name="TOP">-</a>

   merge.h 
   header file for merge.c

   see qh-merge.htm and merge.c

   copyright (c) 1993-2001, The hkGeometry Center
*/

#ifndef qhDEFmerge
#define qhDEFmerge 1


/*============ -constants- ==============*/

/*-<a                             href="qh-merge.htm#TOC"
  >--------------------------------</a><a name="qh_ANGLEredundant">-</a>

  qh_ANGLEredundant
    indicates redundant merge in mergeT->angle
*/
#define qh_ANGLEredundant 6.0f

/*-<a                             href="qh-merge.htm#TOC"
  >--------------------------------</a><a name="qh_ANGLEdegen">-</a>
  
  qh_ANGLEdegen
    indicates degenerate facet in mergeT->angle
*/
#define qh_ANGLEdegen     5.0f

/*-<a                             href="qh-merge.htm#TOC"
  >--------------------------------</a><a name="qh_ANGLEconcave">-</a>
  
  qh_ANGLEconcave
    offset to indicate concave facets in mergeT->angle
  
  notes:
    concave facets are assigned the range of [2,4] in mergeT->angle
    roundoff error may make the angle less than 2
*/
#define qh_ANGLEconcave  1.5f

/*-<a                             href="qh-merge.htm#TOC"
  >--------------------------------</a><a name="MRG">-</a>
  
  MRG... (mergeType)
    indicates the type of a merge (mergeT->type)
*/
typedef enum {	/* in sort order for facet_mergeset */
  MRGnone= 0,
  MRGcoplanar,		/* centrum coplanar */
  MRGanglecoplanar,	/* angle coplanar */
  			/* could detect half concave ridges */
  MRGconcave,		/* concave ridge */
  MRGflip,		/* flipped facet. facet1 == facet2 */
  MRGridge,		/* duplicate ridge (qh_MERGEridge) */
                        /* degen and redundant go onto degen_mergeset */
  MRGdegen,		/* degenerate facet (not enough neighbors) facet1 == facet2 */
  MRGredundant,		/* redundant facet (vertex subset) */
  			/* merge_degenredundant assumes degen < redundant */
  MRGmirror,	        /* mirror facet from qh_triangulate */
  ENDmrg
} mergeType;

/*-<a                             href="qh-merge.htm#TOC"
  >--------------------------------</a><a name="qh_MERGEapex">-</a>
  
  qh_MERGEapex
    flag for qh_mergefacet() to indicate an apex merge  
*/
#define qh_MERGEapex     True

/*============ -structures- ====================*/

/*-<a                             href="qh-merge.htm#TOC"
  >--------------------------------</a><a name="mergeT">-</a>
     
  mergeT
    structure used to merge facets
*/

typedef struct mergeT mergeT;
struct mergeT {		/* initialize in qh_appendmergeset */
  realT   angle;        /* angle between normals of facet1 and facet2 */
  facetT *facet1; 	/* will merge facet1 into facet2 */
  facetT *facet2;
  mergeType type;
};


/*=========== -macros- =========================*/

/*-<a                             href="qh-merge.htm#TOC"
  >--------------------------------</a><a name="FOREACHmerge_">-</a>
     
  FOREACHmerge_( merges ) {...}
    assign 'merge' to each merge in merges
       
  notes:
    uses 'mergeT *merge, **mergep;'
    if qh_mergefacet(),
      restart since qh.facet_mergeset may change
    see <a href="qset.h#FOREACHsetelement_">FOREACHsetelement_</a>
*/
#define FOREACHmerge_( merges ) FOREACHsetelement_(mergeT, merges, merge)

/*============ prototypes in alphabetical order after pre/postmerge =======*/

void    qh_premerge (vertexT *apex, realT maxcentrum, realT maxangle);
void    qh_postmerge (const char *reason, realT maxcentrum, realT maxangle, 
             boolT vneighbors);
void    qh_all_merges (boolT othermerge, boolT vneighbors);
void    qh_appendmergeset(facetT *facet, facetT *neighbor, mergeType mergetype, realT *angle);
setT   *qh_basevertices( facetT *samecycle);
void    qh_checkconnect (void /* qh new_facets */);
boolT   qh_checkzero (boolT testall);
void    qh_copynonconvex (ridgeT *atridge);
void    qh_degen_redundant_facet (facetT *facet);
void   	qh_degen_redundant_neighbors (facetT *facet, facetT *delfacet);
vertexT *qh_find_newvertex (vertexT *oldvertex, setT *vertices, setT *ridges);
void    qh_findbest_test (boolT testcentrum, facetT *facet, facetT *neighbor,
           facetT **bestfacet, realT *distp, realT *mindistp, realT *maxdistp);
facetT *qh_findbestneighbor(facetT *facet, realT *distp, realT *mindistp, realT *maxdistp);
void 	qh_flippedmerges(facetT *facetlist, boolT *wasmerge);
void 	qh_forcedmerges( boolT *wasmerge);
void	qh_getmergeset(facetT *facetlist);
void 	qh_getmergeset_initial (facetT *facetlist);
void    qh_hashridge (setT *hashtable, int hashsize, ridgeT *ridge, vertexT *oldvertex);
ridgeT *qh_hashridge_find (setT *hashtable, int hashsize, ridgeT *ridge, 
              vertexT *vertex, vertexT *oldvertex, int *hashslot);
void 	qh_makeridges(facetT *facet);
void    qh_mark_dupridges(facetT *facetlist);
void    qh_maydropneighbor (facetT *facet);
int     qh_merge_degenredundant (void);
void    qh_merge_nonconvex( facetT *facet1, facetT *facet2, mergeType mergetype);
void    qh_mergecycle (facetT *samecycle, facetT *newfacet);
void    qh_mergecycle_all (facetT *facetlist, boolT *wasmerge);
void    qh_mergecycle_facets( facetT *samecycle, facetT *newfacet);
void    qh_mergecycle_neighbors(facetT *samecycle, facetT *newfacet);
void    qh_mergecycle_ridges(facetT *samecycle, facetT *newfacet);
void    qh_mergecycle_vneighbors( facetT *samecycle, facetT *newfacet);
void 	qh_mergefacet(facetT *facet1, facetT *facet2, realT *mindist, realT *maxdist, boolT mergeapex);
void    qh_mergefacet2d (facetT *facet1, facetT *facet2);
void 	qh_mergeneighbors(facetT *facet1, facetT *facet2);
void 	qh_mergeridges(facetT *facet1, facetT *facet2);
void    qh_mergesimplex(facetT *facet1, facetT *facet2, boolT mergeapex);
void    qh_mergevertex_del (vertexT *vertex, facetT *facet1, facetT *facet2);
void    qh_mergevertex_neighbors(facetT *facet1, facetT *facet2);
void	qh_mergevertices(setT *vertices1, setT **vertices);
setT   *qh_neighbor_intersections (vertexT *vertex);
void    qh_newvertices (setT *vertices);
boolT   qh_reducevertices (void);
vertexT *qh_redundant_vertex (vertexT *vertex);
boolT   qh_remove_extravertices (facetT *facet);
vertexT *qh_rename_sharedvertex (vertexT *vertex, facetT *facet);
void	qh_renameridgevertex(ridgeT *ridge, vertexT *oldvertex, vertexT *newvertex);
void    qh_renamevertex(vertexT *oldvertex, vertexT *newvertex, setT *ridges,
			facetT *oldfacet, facetT *neighborA);
boolT 	qh_test_appendmerge (facetT *facet, facetT *neighbor);
boolT   qh_test_vneighbors (void /* qh newfacet_list */);
void    qh_tracemerge (facetT *facet1, facetT *facet2);
void    qh_tracemerging (void);
void    qh_updatetested( facetT *facet1, facetT *facet2);
setT   *qh_vertexridges (vertexT *vertex);
void    qh_vertexridges_facet (vertexT *vertex, facetT *facet, setT **ridges);
void    qh_willdelete (facetT *facet, facetT *replace);

#endif /* qhDEFmerge */

/*
* Havok Game Dynamics SDK - PUBLIC RELEASE, BUILD(#20030622)
*
* (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. The Telekinesys
* Logo, Havok and the Havok buzzsaw logo are trademarks of Telekinesys
* Research Limited.  Title, ownership
* rights, and intellectual property rights in the Software remain in
* Telekinesys Research Limited and/or its suppliers.
*
* Use of this software is subject to and indicates acceptance of the End User licence
* Agreement for this product. A copy of the license is included with this software and
* is also available from info@havok.com.
*
*/ 
