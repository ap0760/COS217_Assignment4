/*--------------------------------------------------------------------*/
/* checkerDT.c                                                        */
/* Author:                                                            */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "checkerDT.h"
#include "dynarray.h"
#include "path.h"

/* see checkerDT.h for specification */
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

   /* Sample check: parent's path must be the longest possible
      proper prefix of the node's path */
   oNParent = Node_getParent(oNNode);
   if (oNParent != NULL)
   {
      oPNPath = Node_getPath(oNNode);
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

/*
   Performs a pre-order traversal of the tree rooted at oNNode.
   Returns FALSE if a broken invariant is found and
   returns TRUE otherwise.

   You may want to change this function's return type or
   parameter list to facilitate constructing your checks.
   If you do, you should update this function comment.
*/
static boolean CheckerDT_treeCheck(Node_T oNNode)
{
   size_t ulIndex;

   if (oNNode != NULL)
   {

      /* Sample check on each node: node must be valid */
      /* If not, pass that failure back up immediately */
      if (!CheckerDT_Node_isValid(oNNode))
         return FALSE;

      /* Check that if a node has multiple children, those children are unique */
      if (Node_getNumChildren(oNNode) > 1)
      {
         size_t numChildren;
         size_t i;
         size_t j;

         numChildren = Node_getNumChildren(oNNode);
         for (i = 0; i < numChildren; i++)
         {
            for (j = i + 1; j < numChildren; j++)
            {
               Node_T oNChild1 = NULL;
               Node_T oNChild2 = NULL;
               Node_getChild(oNNode, i, &oNChild1);
               Node_getChild(oNNode, j, &oNChild2);
               if (Node_compare(oNChild1, oNChild2) == 0)
               {
                  fprintf(stderr, "Two nodes have the same absolute path name\n");
                  return FALSE;
               }
               /*else if (Node_compare(oNChild1, oNChild2) < 0)
               {
                  fprintf(stderr, "Nodes are not in lexicographic order\n");
                  return FALSE;
               }*/
            }
         }
      }

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
         if (!CheckerDT_treeCheck(oNChild))
            return FALSE;
      }
   }
   return TRUE;
}

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
   else
      return TRUE;
}

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
   else
      return TRUE;
}

/* see checkerDT.h for specification */
boolean CheckerDT_isValid(boolean bIsInitialized, Node_T oNRoot,
                          size_t ulCount)
{

   /* Sample check on a top-level data structure invariant:
      if the DT is not initialized, its count should be 0 and oNRoot
      should be NULL */
   if (!bIsInitialized)
   {
      if (CheckerDT_bNotInitialized(oNRoot, ulCount) == FALSE)
         return FALSE;
   }
   else
   {
      if (CheckerDT_bIsInitialized(oNRoot, ulCount) == FALSE)
         return FALSE;
   }

   /* Now checks invariants recursively at each node from the root. */
   return CheckerDT_treeCheck(oNRoot);
}
