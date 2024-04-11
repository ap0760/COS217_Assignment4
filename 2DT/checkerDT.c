/*--------------------------------------------------------------------*/
/* checkerDT.c                                                        */
/* Author: Yoni and Ariella                                           */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "checkerDT.h"
#include "dynarray.h"
#include "path.h"

/* see checkerDT.h for specification. Returns a boolean - TRUE
if oNNode is a valid node and FALSE otherwise. */
boolean CheckerDT_Node_isValid(Node_T oNNode)
{
   Node_T oNParent;
   Path_T oPNPath;
   Path_T oPPPath;

   /* Sample check: a NULL pointer is not a valid node */
   if (oNNode == NULL)
   {
      fprintf(stderr, "A node is a NULL pointer\n");
      return FALSE;
   }

   /* Check Node invariants - if path is NULL then parent must be NULL
   and child count must be 0 */
   oNParent = Node_getParent(oNNode);
   oPNPath = Node_getPath(oNNode);

   if (oPNPath == NULL)
   {
      if ((oNParent != NULL) || (Node_getNumChildren(oNParent) != 0))
         return FALSE;
   }

   /* Sample check: parent's path must be the longest possible
      proper prefix of the node's path */
   if (oNParent != NULL)
   {
      oPPPath = Node_getPath(oNParent);

      if (Path_getSharedPrefixDepth(oPNPath, oPPPath) !=
          Path_getDepth(oPNPath) - 1)
      {
         fprintf(stderr, "P-C nodes don't have P-C paths: (%s) (%s)\n",
                 Path_getPathname(oPPPath), Path_getPathname(oPNPath));
         return FALSE;
      }
   }
   return TRUE;
}

/* Check that if a node oNNode has multiple children, those children
are unique and are in lexicographic order. Return a boolean - TRUE if 
this is the case and FALSE otherwise. */
static boolean CheckerDT_siblingsCorrect(Node_T oNNode)
{
   size_t ulnumChildren;
   size_t i;
   size_t j;

   ulnumChildren = Node_getNumChildren(oNNode);
   if (ulnumChildren <= 1)
      return TRUE;

   for (i = 0; i < ulnumChildren; i++)
   {
      for (j = i + 1; j < ulnumChildren; j++)
      {
         int icompareVal = 0;
         Node_T oNChild1 = NULL;
         Node_T oNChild2 = NULL;
         Node_getChild(oNNode, i, &oNChild1);
         Node_getChild(oNNode, j, &oNChild2);

         icompareVal = Path_comparePath(Node_getPath(oNChild1),
                                        Node_getPath(oNChild2));
         if (icompareVal == 0)
         {
            fprintf(stderr,
                    "Two nodes have the same absolute path name\n");
            return FALSE;
         }
         else if (icompareVal > 0)
         {
            fprintf(stderr, "Nodes are not in lexicographic order\n");
            return FALSE;
         }
      }
   }
   return TRUE;
}

/*
   Performs a pre-order traversal of the tree rooted at oNNode.
   Returns FALSE if a broken invariant is found and
   returns TRUE otherwise. Updates value stored at *pulNodeCount 
   each time a valid Node is found.
*/
static boolean CheckerDT_treeCheck(Node_T oNNode, size_t *pulNodeCount)
{
   size_t ulIndex;

   assert (pulNodeCount != NULL);

   if (oNNode != NULL)
   {

      /* Sample check on each node: node must be valid */
      /* If not, pass that failure back up immediately */
      if (!CheckerDT_Node_isValid(oNNode))
         return FALSE;

      /* Check that if a node has multiple children, those children are unique */
      if (!CheckerDT_siblingsCorrect(oNNode))
         return FALSE;

      /* increment amount of Nodes each time before recurs */
      (*pulNodeCount)++;

      /* Recur on every child of oNNode */
      for (ulIndex = 0; ulIndex < Node_getNumChildren(oNNode); ulIndex++)
      {
         Node_T oNChild = NULL;

         int iStatus = Node_getChild(oNNode, ulIndex, &oNChild);

         if (iStatus != SUCCESS)
         {
            fprintf(stderr, "getNumChildren claims more children than getChild returns\n");
            return FALSE;
         }

         /* if recurring down one subtree results in a failed check
            farther down, passes the failure back up immediately */
         if (!CheckerDT_treeCheck(oNChild, pulNodeCount))
            return FALSE;
      }
   }
   return TRUE;
}

/* Sample check on a top-level data structure invariant:
      if the DT is not initialized, its ulCount should be 0 and oNRoot
      should be NULL */
static boolean CheckerDT_bNotInitialized(Node_T oNRoot, size_t ulCount)
{
   if (ulCount != 0)
   {
      fprintf(stderr, "Not initialized, but count is not 0\n");
      return FALSE;
   }
   if (oNRoot != NULL)
   {
      fprintf(stderr, "Not initialized, but root is not NULL\n");
      return FALSE;
   }

   return TRUE;
}

/* If DT is initialized, then ulCount cannot be zero and oNRoot
   cannot be NULL */
static boolean CheckerDT_bIsInitialized(Node_T oNRoot, size_t ulCount)
{
   if ((ulCount == 0) && (oNRoot != NULL))
   {
      fprintf(stderr, "Has a root, but count is not 0\n");
      return FALSE;
   }
   if ((ulCount != 0) && (oNRoot == NULL))
   {
      fprintf(stderr, "Has no root, but count is not 0\n");
      return FALSE;
   }
   return TRUE;
}

/* see checkerDT.h for specification */
boolean CheckerDT_isValid(boolean bIsInitialized, Node_T oNRoot,
                          size_t ulCount)
{
   boolean iStatus;
   size_t ulNodeCount = 0;


   if (!bIsInitialized)
   {
      if (!CheckerDT_bNotInitialized(oNRoot, ulCount))
         return FALSE;
   }

   if (!CheckerDT_bIsInitialized(oNRoot, ulCount))
      return FALSE;

   /* Now checks invariants recursively at each node from the root. */
   iStatus = CheckerDT_treeCheck(oNRoot, &ulNodeCount);
   if (iStatus == FALSE)
      return iStatus;

   /* Checks if NodeCount hasn't been updated correctly, and has
      become out of sync with ulCount */
   if (ulNodeCount != ulCount)
   {
      fprintf(stderr, "Node Count is not being tracked correctly\n");
      return FALSE;
   }

   return iStatus;
}
