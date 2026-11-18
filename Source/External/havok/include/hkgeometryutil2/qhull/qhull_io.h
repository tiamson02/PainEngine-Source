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

/*<html><pre>  -<a                             href="qh-io.htm"
  >-------------------------------</a><a name="TOP">-</a>

   io.h 
   declarations of Input/Output functions

   see README, qhull.h and io.c

   copyright (c) 1993-2001, The hkGeometry Center
*/

#ifndef qhDEFio
#define qhDEFio 1

/*============ constants and flags ==================*/

/*-<a                             href="qh-io.htm#TOC"
  >--------------------------------</a><a name="qh_MAXfirst">-</a>
  
  qh_MAXfirst
    maximum length of first two lines of stdin
*/
#define qh_MAXfirst  200

/*-<a                             href="qh-io.htm#TOC"
  >--------------------------------</a><a name="qh_MINradius">-</a>
  
  qh_MINradius
    min radius for Gp and Gv, fraction of maxcoord
*/
#define qh_MINradius 0.02f

/*-<a                             href="qh-io.htm#TOC"
  >--------------------------------</a><a name="qh_GEOMepsilon">-</a>
  
  qh_GEOMepsilon
    adjust outer planes for 'lines closer' and geomview roundoff.  
    This prevents bleed through.
*/
#define qh_GEOMepsilon 2e-3f

/*-<a                             href="qh-io.htm#TOC"
  >--------------------------------</a><a name="qh_WHITESPACE">-</a>
  
  qh_WHITESPACE
    possible values of white space
*/
#define qh_WHITESPACE " \n\t\v\r\f"


/*-<a                             href="qh-io.htm#TOC"
  >--------------------------------</a><a name="RIDGE">-</a>
  
  qh_RIDGE
    to select which ridges to print in qh_eachvoronoi
*/
typedef enum
{
    qh_RIDGEall = 0, qh_RIDGEinner, qh_RIDGEouter
}
qh_RIDGE;

/*-<a                             href="qh-io.htm#TOC"
  >--------------------------------</a><a name="printvridgeT">-</a>
  
  printvridgeT
    prints results of qh_printvdiagram

  see:
    <a href="io.c#printvridge">qh_printvridge</a> for an example
*/
typedef void (*printvridgeT)(void *fp, vertexT *vertex, vertexT *vertexA, setT *centers, boolT unbounded);

/*============== -prototypes in alphabetical order =========*/

void    dfacet( unsigned id);
void    dvertex( unsigned id);
void    qh_countfacets (facetT *facetlist, setT *facets, boolT printall, 
              int *numfacetsp, int *numsimplicialp, int *totneighborsp, 
              int *numridgesp, int *numcoplanarsp, int *numnumtricoplanarsp);
pointT *qh_detvnorm (vertexT *vertex, vertexT *vertexA, setT *centers, realT *offsetp);
setT   *qh_detvridge (vertexT *vertex);
setT   *qh_detvridge3 (vertexT *atvertex, vertexT *vertex);
int     qh_eachvoronoi (void *fp, printvridgeT printvridge, vertexT *atvertex, boolT visitall, qh_RIDGE innerouter, boolT inorder);
int     qh_eachvoronoi_all (void *fp, printvridgeT printvridge, boolT isupper, qh_RIDGE innerouter, boolT inorder);
void	qh_facet2point(facetT *facet, pointT **point0, pointT **point1, realT *mindist);
setT   *qh_facetvertices (facetT *facetlist, setT *facets, boolT allfacets);
void    qh_geomplanes (facetT *facet, realT *outerplane, realT *innerplane);
void    qh_markkeep (facetT *facetlist);
setT   *qh_markvoronoi (facetT *facetlist, setT *facets, boolT printall, boolT *islowerp, int *numcentersp);
void    qh_order_vertexneighbors(vertexT *vertex);
void	qh_printafacet(void *fp, int format, facetT *facet, boolT printall);
void    qh_printbegin (void *fp, int format, facetT *facetlist, setT *facets, boolT printall);
void 	qh_printcenter (void *fp, int format, const char *string, facetT *facet);
void    qh_printcentrum (void *fp, facetT *facet, realT radius);
void    qh_printend (void *fp, int format, facetT *facetlist, setT *facets, boolT printall);
void    qh_printend4geom (void *fp, facetT *facet, int *num, boolT printall);
void    qh_printextremes (void *fp, facetT *facetlist, setT *facets, int printall);
void    qh_printextremes_2d (void *fp, facetT *facetlist, setT *facets, int printall);
void    qh_printextremes_d (void *fp, facetT *facetlist, setT *facets, int printall);
void	qh_printfacet(void *fp, facetT *facet);
void	qh_printfacet2math(void *fp, facetT *facet, int notfirst);
void	qh_printfacet2geom(void *fp, facetT *facet, realT color[3]);
void    qh_printfacet2geom_points(void *fp, pointT *point1, pointT *point2,
			       facetT *facet, realT offset, realT color[3]);
void	qh_printfacet3math (void *fp, facetT *facet, int notfirst);
void	qh_printfacet3geom_nonsimplicial(void *fp, facetT *facet, realT color[3]);
void	qh_printfacet3geom_points(void *fp, setT *points, facetT *facet, realT offset, realT color[3]);
void	qh_printfacet3geom_simplicial(void *fp, facetT *facet, realT color[3]);
void	qh_printfacet3vertex(void *fp, facetT *facet, int format);
void	qh_printfacet4geom_nonsimplicial(void *fp, facetT *facet, realT color[3]);
void	qh_printfacet4geom_simplicial(void *fp, facetT *facet, realT color[3]);
void	qh_printfacetNvertex_nonsimplicial(void *fp, facetT *facet, int id, int format);
void	qh_printfacetNvertex_simplicial(void *fp, facetT *facet, int format);
void    qh_printfacetheader(void *fp, facetT *facet);
void    qh_printfacetridges(void *fp, facetT *facet);
void	qh_printfacets(void *fp, int format, facetT *facetlist, setT *facets, boolT printall);
void	qh_printhelp_degenerate(void *fp);
void	qh_printhelp_singular(void *fp);
void	qh_printhyperplaneintersection(void *fp, facetT *facet1, facetT *facet2,
  		   setT *vertices, realT color[3]);
void	qh_printneighborhood (void *fp, int format, facetT *facetA, facetT *facetB, boolT printall);
void    qh_printline3geom (void *fp, pointT *pointA, pointT *pointB, realT color[3]);
void	qh_printpoint(void *fp, const char *string, pointT *point);
void	qh_printpointid(void *fp, const char *string, int dim, pointT *point, int id);
void    qh_printpoint3 (void *fp, pointT *point);
void    qh_printpoints_out (void *fp, facetT *facetlist, setT *facets, int printall);
void    qh_printpointvect (void *fp, pointT *point, coordT *normal, pointT *center, realT radius, realT color[3]);
void    qh_printpointvect2 (void *fp, pointT *point, coordT *normal, pointT *center, realT radius);
void	qh_printridge(void *fp, ridgeT *ridge);
void    qh_printspheres(void *fp, setT *vertices, realT radius);
void    qh_printvdiagram (void *fp, int format, facetT *facetlist, setT *facets, boolT printall);
int     qh_printvdiagram2 (void *fp, printvridgeT printvridge, setT *vertices, qh_RIDGE innerouter, boolT inorder);
void	qh_printvertex(void *fp, vertexT *vertex);
void	qh_printvertexlist (void *fp, const char* string, facetT *facetlist,
                         setT *facets, boolT printall);
void	qh_printvertices (void *fp, const char* string, setT *vertices);
void    qh_printvneighbors (void *fp, facetT* facetlist, setT *facets, boolT printall);
void    qh_printvoronoi (void *fp, int format, facetT *facetlist, setT *facets, boolT printall);
void    qh_printvnorm (void *fp, vertexT *vertex, vertexT *vertexA, setT *centers, boolT unbounded);
void    qh_printvridge (void *fp, vertexT *vertex, vertexT *vertexA, setT *centers, boolT unbounded);
void	qh_produce_output(void);
void    qh_projectdim3 (pointT *source, pointT *destination);
int     qh_readfeasible (int dim, char *remainder);
coordT *qh_readpoints(int *numpoints, int *dimension, boolT *ismalloc);
void    qh_setfeasible (int dim);
boolT	qh_skipfacet(facetT *facet);

#endif /* qhDEFio */

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
