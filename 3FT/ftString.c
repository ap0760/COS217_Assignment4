#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "ft.h"
#include "nodeFT.h"
#include "path.h"
#include "dynarray.h"

/* String function */

/* 1. a flag for being in an initialized state (TRUE) or not (FALSE) */
static boolean bIsInitialized;
/* 2. a pointer to the root node in the hierarchy */
static Node_T oNRoot;
/* 3. a counter of the number of nodes in the hierarchy */
static size_t ulCount;

/* --------------------------------------------------------------------

  The following auxiliary functions are used for generating the
  string representation of the FT.
*/

/*
  Performs a pre-order traversal of the tree rooted at oNNode,
  inserting each payload to DynArray_T oDDynArray beginning at index ulIndex.
  Returns the next unused index in oDDynArray after the insertion(s).
*/
static size_t FT_preOrderTraversal(Node_T oNNode, DynArray_T oDDynArray, size_t ulIndex)
{
    size_t ulCurr;
    assert(oDDynArray != NULL);

    if (oNNode != NULL)
    {
        for (ulCurr = 0; ulCurr < Node_getNumChildren(oNNode); ulCurr++)
        {
            if (Node_isFile)
            {
                (void)DynArray_set(oDDynArray, ulIndex, oNNode);
                ulIndex++;
            }
        }
        
        for (ulCurr = 0; ulCurr < Node_getNumChildren(oNNode); ulCurr++)
        {
            (void)DynArray_set(oDDynArray, ulIndex, oNNode);
            ulIndex++;

            int iStatus;
            Node_T oNChild = NULL;
            iStatus = Node_getChild(oNNode, ulCurr, &oNChild);
            assert(iStatus == SUCCESS);

            ulIndex = FT_preOrderTraversal(oNChild, oDDynArray, ulIndex);
        }
    }
    return ulIndex;
}

/*
  Alternate version of strlen that uses pulAcc as an in-out parameter
  to accumulate a string length, rather than returning the length of
  oNNode's path, and also always adds one addition byte to the sum.
*/
static void FT_strlenAccumulate(Node_T oNNode, size_t *pulAcc)
{
    assert(pulAcc != NULL);

    if (oNNode != NULL)
        *pulAcc += (Path_getStrLength(Node_getPath(oNNode)) + 1);
}

/*
  Alternate version of strcat that inverts the typical argument
  order, appending oNNode's path onto pcAcc, and also always adds one
  newline at the end of the concatenated string.
*/
static void FT_strcatAccumulate(Node_T oNNode, char *pcAcc)
{
    assert(pcAcc != NULL);

    if (oNNode != NULL)
    {
        strcat(pcAcc, Path_getPathname(Node_getPath(oNNode)));
        strcat(pcAcc, "\n");
    }
}
/*--------------------------------------------------------------------*/

char *FT_toString(void)
{
    DynArray_T nodes;
    size_t totalStrlen = 1;
    char *result = NULL;

    if (!bIsInitialized)
        return NULL;

    nodes = DynArray_new(ulCount);
    (void)FT_preOrderTraversal(oNRoot, nodes, 0);

    DynArray_map(nodes, (void (*)(void *, void *))FT_strlenAccumulate,
                 (void *)&totalStrlen);

    result = malloc(totalStrlen);
    if (result == NULL)
    {
        DynArray_free(nodes);
        return NULL;
    }
    *result = '\0';

    DynArray_map(nodes, (void (*)(void *, void *))FT_strcatAccumulate,
                 (void *)result);

    DynArray_free(nodes);

    return result;
}
