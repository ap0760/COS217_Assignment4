/*--------------------------------------------------------------------*/
/* nodeFT.h                                                           */
/* Author: Ariella and Yoni                                           */
/*--------------------------------------------------------------------*/

#ifndef NODE_INCLUDED
#define NODE_INCLUDED

#include <stddef.h>
#include "a4def.h"

/* A Node_T is a node in a Directory Tree */
typedef struct node *Node_T;

/*
  Creates a new node in the Directory Tree, with path oPPath and
  parent oNParent. Returns an int SUCCESS status and sets *poNResult
  to be the new node if successful. Otherwise, sets *poNResult to NULL
  and returns status:
  * MEMORY_ERROR if memory could not be allocated to complete request
  * CONFLICTING_PATH if oNParent's path is not an ancestor of oPPath
  * NO_SUCH_PATH if oPPath is of depth 0
                 or oNParent's path is not oPPath's direct parent
                 or oNParent is NULL but oPPath is not of depth 1
  * ALREADY_IN_TREE if oNParent already has a child with this path
*/
int Node_new(const char *pcPath, Node_T oNParent, void *pvContents, size_t ulLength, boolean bisFile, Node_T *poNResult);

/*
  Destroys and frees all memory allocated for the subtree rooted at
  oNNode, i.e., deletes this node and all its descendents. Returns the
  number of nodes deleted.
*/
size_t Node_free(Node_T oNNode);

/* Returns the path object representing oNNode's absolute path. */
Path_T Node_getPath(Node_T oNNode);

/*
  Returns TRUE if oNParent has a child with path oPPath. Returns
  FALSE if it does not.

  If oNParent has such a child, stores in *pulChildID the child's
  identifier (as used in Node_getChild). If oNParent does not have
  such a child, stores in *pulChildID the identifier that such a
  child _would_ have if inserted.
*/
boolean Node_hasChild(const char *pcPath, Node_T oNParent,
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
int Node_compareNode(Node_T oNFirst, Node_T oNSecond);
int Node_compareString(Node_T oNFirst, char *pcSecond);

/*
  Returns a string representation for oNNode, or NULL if
  there is an allocation error.

  Allocates memory for the returned string, which is then owned by
  the caller!
*/
char *Node_toString(Node_T oNNode);

/* new functions for nodeFT */
/* Check if a given node represents a file or a directory.
Returns TRUE if pcPath is a file, FALSE if it is a directory. */
boolean Node_isFile(Node_T oNNode);

/* Return the contents of a given file node, or NULL if
unable to complete the request for any reason */
void *Node_getFileContents(Node_T oNNode);

/* Return the size of a given file node, or NULL if
unable to complete the request for any reason */
size_t Node_getFileSize(Node_T oNNode);

/* Replaces current contents of the file with absolute path pcPath with
  the parameter pvNewContents of size ulNewLength bytes.
  Returns the old contents if successful. (Note: contents may be NULL.)
  Returns NULL if unable to complete the request for any reason. */
void *Node_replaceFileContents(Node_T oNNode, void *pvNewContents,
                               size_t ulNewLength);


/* these two functions are basically just path functions in the node
module. if we end up using path objects in ft then we wont need them */
/*
  Returns the string version of the component of oPPath at level
  ulLevel. This count is from 0, so with level 0 the root of oPPath
  would be returned.
  Returns NULL if ulLevel is greater than oPPath's maxium level.
*/
const char *Node_getPathComponent(Node_T oNNode, size_t ulLevel);
// const char *Path_getComponent(Path_T oPPath, size_t ulLevel);

/*
  Returns the number of separate levels (components) in oPPath.
  For example, the absolute path "someRoot" has depth 1, and
  "someRoot/aChild/aGrandChild/aGreatGrandChild" has depth 4.
*/
size_t Node_getPathDepth(Node_T oNNode);
//size_t Path_getDepth(Path_T oPPath);

#endif
