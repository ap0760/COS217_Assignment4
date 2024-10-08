/*--------------------------------------------------------------------*/
/* nodeFT.c                                                           */
/* Author: Yoni and Ariella                                        */
/*--------------------------------------------------------------------*/

#include <assert.h>
#include <string.h>
#include "nodeFT.h"
#include "dynarray.h"

/* A node in a DT */
struct node
{
   /* the object corresponding to the node's absolute path */
   Path_T oPPath;
   /* this node's parent */
   Node_T oNParent;
   /* the object containing links to this node's children,
   if it's a directory */
   DynArray_T oDChildren;
   /* a pointer to the file contents, if it's a file */
   void *pvContents;
   /* file length, if it's a file */
   size_t ulLength;
   /* a boolean to determine if the node represents a file or directory */
   boolean bIsFile;
};

/*
  Links new child oNChild into oNParent's children array at index
  ulIndex. Returns SUCCESS if the new child was added successfully,
  or  MEMORY_ERROR if allocation fails adding oNChild to the array.
*/
static int Node_addChild(Node_T oNParent, Node_T oNChild,
                         size_t ulIndex)
{
   assert(oNParent != NULL);
   assert(oNChild != NULL);

   if (oNParent->bIsFile)
      return NOT_A_DIRECTORY;
   else if (DynArray_addAt(oNParent->oDChildren, ulIndex, oNChild))
      return SUCCESS;
   else
      return MEMORY_ERROR;
}

/*
  Compares the string representation of oNfirst with a string
  pcSecond representing a node's path.
  Returns <0, 0, or >0 if oNFirst is "less than", "equal to", or
  "greater than" pcSecond, respectively.
*/
static int Node_compareString(const Node_T oNFirst,
                              const char *pcSecond)
{
   assert(oNFirst != NULL);
   assert(pcSecond != NULL);

   return Path_compareString(oNFirst->oPPath, pcSecond);
}

/*
  Compares oNFirst and oNSecond lexicographically based on their paths.
  Returns <0, 0, or >0 if onFirst is "less than", "equal to", or
  "greater than" oNSecond, respectively.
*/
static int Node_compare(Node_T oNFirst, Node_T oNSecond)
{
   assert(oNFirst != NULL);
   assert(oNSecond != NULL);

   return Path_comparePath(oNFirst->oPPath, oNSecond->oPPath);
}

int Node_new(const char *pcPath, Node_T oNParent, void *pvContents,
             size_t ulLength, boolean bIsFile, Node_T *poNResult)
{
   Node_T oNNewNode;
   Path_T oPParentPath = NULL;
   Path_T oPNewPath = NULL;
   size_t ulParentDepth;
   size_t ulIndex = 0;
   int iStatus;

   assert(pcPath != NULL);
   assert(poNResult != NULL);

   /* allocate space for a new node */
   oNNewNode = malloc(sizeof(struct node));
   if (oNNewNode == NULL)
   {
      *poNResult = NULL;
      return MEMORY_ERROR;
   }

   /* set the new node's path */
   iStatus = Path_new(pcPath, &oPNewPath);
   if (iStatus != SUCCESS)
   {
      free(oNNewNode);
      *poNResult = NULL;
      return iStatus;
   }
   oNNewNode->oPPath = oPNewPath;

   /* validate and set the new node's parent */
   if (oNParent != NULL)
   {
      size_t ulSharedDepth;

      oPParentPath = oNParent->oPPath;
      ulParentDepth = Path_getDepth(oPParentPath);
      ulSharedDepth = Path_getSharedPrefixDepth(oNNewNode->oPPath,
                                                oPParentPath);
      /* parent must be an ancestor of child */
      if (ulSharedDepth < ulParentDepth)
      {
         Path_free(oNNewNode->oPPath);
         free(oNNewNode);
         *poNResult = NULL;
         return CONFLICTING_PATH;
      }

      /* parent must be exactly one level up from child */
      if (Path_getDepth(oNNewNode->oPPath) != ulParentDepth + 1)
      {
         Path_free(oNNewNode->oPPath);
         free(oNNewNode);
         *poNResult = NULL;
         return NO_SUCH_PATH;
      }

      /* parent must not already have child with this path */
      if (Node_hasChild(oNParent, pcPath, &ulIndex))
      {
         Path_free(oNNewNode->oPPath);
         free(oNNewNode);
         *poNResult = NULL;
         return ALREADY_IN_TREE;
      }
   }
   else
   {
      /* new node must be root */
      /* can only create one "level" at a time */
      if (Path_getDepth(oNNewNode->oPPath) != 1)
      {
         Path_free(oNNewNode->oPPath);
         free(oNNewNode);
         *poNResult = NULL;
         return NO_SUCH_PATH;
      }
      /* a file cannot be the root */
      if (bIsFile)
      {
         Path_free(oNNewNode->oPPath);
         free(oNNewNode);
         *poNResult = NULL;
         return CONFLICTING_PATH;
      }
   }
   oNNewNode->oNParent = oNParent;

   /* initialize the new node */
   if (bIsFile) /* file initialization */
   {
      oNNewNode->pvContents = (char *)pvContents;
      oNNewNode->ulLength = ulLength;
      oNNewNode->bIsFile = TRUE;
   }
   else /* directory initialization */
   {
      oNNewNode->bIsFile = FALSE;
      oNNewNode->oDChildren = DynArray_new(0);
      if (oNNewNode->oDChildren == NULL)
      {
         Path_free(oNNewNode->oPPath);
         free(oNNewNode);
         *poNResult = NULL;
         return MEMORY_ERROR;
      }
   }

   /* Link into parent's children list */
   if (oNParent != NULL)
   {
      iStatus = Node_addChild(oNParent, oNNewNode, ulIndex);
      if (iStatus != SUCCESS)
      {
         Path_free(oNNewNode->oPPath);
         free(oNNewNode);
         *poNResult = NULL;
         return iStatus;
      }
   }

   *poNResult = oNNewNode;

   return SUCCESS;
}

size_t Node_free(Node_T oNNode)
{
   size_t ulIndex = 0;
   size_t ulCount = 0;

   assert(oNNode != NULL);

   /* Remove from parent's list */
   if (oNNode->oNParent != NULL)
   {
      if (DynArray_bsearch(
              oNNode->oNParent->oDChildren,
              oNNode, &ulIndex,
              (int (*)(const void *, const void *))Node_compare))
         (void)DynArray_removeAt(oNNode->oNParent->oDChildren,
                                 ulIndex);
   }

   /* If it's a directory, recursively remove children */
   if (!Node_isFile(oNNode))
   {
      while (DynArray_getLength(oNNode->oDChildren) != 0)
      {
         ulCount += Node_free(DynArray_get(oNNode->oDChildren, 0));
      }
      DynArray_free(oNNode->oDChildren);
   }

   /* Remove path */
   Path_free(oNNode->oPPath);

   /* Finally, free the struct node */
   free(oNNode);
   ulCount++;
   return ulCount;
}

Path_T Node_getPath(Node_T oNNode)
{
   assert(oNNode != NULL);

   return oNNode->oPPath;
}

/* We attempted to make this simpler by using char *pcPath
   instead of Path_T oPPath */
boolean Node_hasChild(Node_T oNParent, const char *pcPath,
                      size_t *pulChildID)
{
   assert(oNParent != NULL);
   assert(pcPath != NULL);
   assert(pulChildID != NULL);

   if (Node_isFile(oNParent))
      return FALSE;

   /* *pulChildID is the index into oNParent->oDChildren */
   return DynArray_bsearch(oNParent->oDChildren, (char *)pcPath, pulChildID,
                           (int (*)(const void *, const void *))Node_compareString);
}

size_t Node_getNumChildren(Node_T oNParent)
{
   assert(oNParent != NULL);

   return DynArray_getLength(oNParent->oDChildren);
}

int Node_getChild(Node_T oNParent, size_t ulChildID,
                  Node_T *poNResult)
{

   assert(oNParent != NULL);
   assert(poNResult != NULL);

   /* ulChildID is the index into oNParent->oDChildren */
   if (ulChildID >= Node_getNumChildren(oNParent))
   {
      *poNResult = NULL;
      return NO_SUCH_PATH;
   }
   else
   {
      *poNResult = DynArray_get(oNParent->oDChildren, ulChildID);
      return SUCCESS;
   }
}

Node_T Node_getParent(Node_T oNNode)
{
   assert(oNNode != NULL);

   return oNNode->oNParent;
}

char *Node_toString(Node_T oNNode)
{
   char *copyPath;

   assert(oNNode != NULL);

   copyPath = malloc(Path_getStrLength(Node_getPath(oNNode)) + 1);
   if (copyPath == NULL)
      return NULL;
   else
      return strcpy(copyPath, Path_getPathname(Node_getPath(oNNode)));
}

/* New functions for nodeFT */
boolean Node_isFile(Node_T oNNode)
{
   assert(oNNode != NULL);
   return oNNode->bIsFile;
}

void *Node_getFileContents(Node_T oNNode)
{
   assert(oNNode != NULL);
   /* If the given node is a directory, pvContents will be NULL */
   return oNNode->pvContents;
}

size_t Node_getFileSize(Node_T oNNode)
{
   assert(oNNode != NULL);
   /* If the given node is a directory, ulLength will be 0 */
   return oNNode->ulLength;
}

void *Node_replaceFileContents(Node_T oNNode, void *pvNewContents,
                               size_t ulNewLength)
{
   void *pvOldContents;
   assert(oNNode != NULL);

   /* Prevents us from accidentally giving directories non-null
      pvContents and ulLength */
   if (!Node_isFile(oNNode))
      return NULL;

   /* Not a directory - replace pvContents and ulLength */
   pvOldContents = oNNode->pvContents;
   oNNode->pvContents = pvNewContents;
   oNNode->ulLength = ulNewLength;

   return pvOldContents;
}
