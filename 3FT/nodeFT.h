/*--------------------------------------------------------------------*/
/* nodeFT.h                                                           */
/* Author: Ariella and Yoni                                           */
/*--------------------------------------------------------------------*/

#ifndef NODE_INCLUDED
#define NODE_INCLUDED

#include <stddef.h>
#include <stdlib.h>
#include "a4def.h"
#include "path.h"

/* A Node_T is a node in a File Tree */
typedef struct node *Node_T;

/*
  Creates a new node in the File Tree, with pathname pcPath and
  parent oNParent. If bIsFile is TRUE, set pvContents and ulLength to
  the contents and length specified by the caller. If bIsFile is
  FALSE, pvContents and ulLength will be ignored.
  Returns an int SUCCESS status and sets *poNResult
  to be the new node if successful. Otherwise, sets *poNResult to NULL
  and returns status:
  * MEMORY_ERROR if memory could not be allocated to complete request
  * CONFLICTING_PATH if oNParent's path is not an ancestor of oPPath
  * NO_SUCH_PATH if oPPath is of depth 0
                 or oNParent's path is not oPPath's direct parent
                 or oNParent is NULL but oPPath is not of depth 1
  * ALREADY_IN_TREE if oNParent already has a child with this path
*/
int Node_new(const char *pcPath, Node_T oNParent, void *pvContents,
             size_t ulLength, boolean bIsFile, Node_T *poNResult);

/*
  Destroys and frees all memory allocated for the subtree rooted at
  oNNode, i.e., deletes this node and all its descendents. Returns the
  number of nodes deleted.
*/
size_t Node_free(Node_T oNNode);

/* Returns the path object representing oNNode's absolute path. */
Path_T Node_getPath(Node_T oNNode);

/*
  Returns TRUE if oNParent has a child with pathname pcPath. Returns
  FALSE if it does not.

  If oNParent has such a child, stores in *pulChildID the child's
  identifier (as used in Node_getChild). If oNParent does not have
  such a child, stores in *pulChildID the identifier that such a
  child _would_ have if inserted.
*/
boolean Node_hasChild(Node_T oNParent, const char *pcPath,
                      size_t *pulChildID);

/* Returns the number of children that oNParent has. */
size_t Node_getNumChildren(Node_T oNParent);

/*
  Returns an int SUCCESS status and sets *poNResult to be the child
  node of oNParent with identifier ulChildID, if one exists.
  Otherwise, sets *poNResult to NULL and returns status:
  * NO_SUCH_PATH if ulChildID is not a valid child for oNParent
*/
int Node_getChild(Node_T oNParent, size_t ulChildID,
                  Node_T *poNResult);

/*
  Returns a the parent node of oNNode.
  Returns NULL if oNNode is the root and thus has no parent.
*/
Node_T Node_getParent(Node_T oNNode);

/*
  Compares oNFirst and oNSecond lexicographically based on their paths.
  Returns <0, 0, or >0 if onFirst is "less than", "equal to", or
  "greater than" oNSecond, respectively.
*/
int Node_compare(Node_T oNFirst, Node_T oNSecond);

/*
  Returns a string representation for oNNode, or NULL if
  there is an allocation error.

  Allocates memory for the returned string, which is then owned by
  the caller!
*/
char *Node_toString(Node_T oNNode);

/* new functions for nodeFT */
/* Check if a given node represents a file or a directory.
Returns TRUE if oNNode is a file, FALSE if it is a directory. */
boolean Node_isFile(Node_T oNNode);

/* Return the contents of a given file node oNNode, or
NULL if unable to complete the request for any reason */
void *Node_getFileContents(Node_T oNNode);

/* Return the size of a given file node oNNode, or
NULL if unable to complete the request for any reason */
size_t Node_getFileSize(Node_T oNNode);

/* Replace current contents of the file node oNNode with
  absolute path pcPath with pvNewContents of size ulNewLength bytes.
  Return the old contents if successful. (Note: contents may be NULL.)
  Returns NULL if unable to complete the request for any reason. */
void *Node_replaceFileContents(Node_T oNNode, void *pvNewContents,
                               size_t ulNewLength);

#endif
