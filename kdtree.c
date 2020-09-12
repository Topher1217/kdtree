#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#include "kdtree.h"
#include "location.h"

#define infinity 999999999

typedef struct node{
	location loc;
	struct node *left;
	struct node *right;
}node;

struct _kdtree{
	node *root;
}; 
// ---------------------------------------------------------------------------------------
void merge(int n1, const location a1[], int n2, const location a2[], location out[], int param){
	int i1; // keeps track of index of first array
    int i2; // keeps track of index of second array
    int iout;
	i1 = i2 = iout = 0; 
	if (param == 1){ // if sorting by x
		while(i1 < n1 || i2 < n2) { // while there is still stuff to sort
			if(i2 >= n2 || ((i1 < n1) && (a1[i1].x < a2[i2].x))) { // a1[i1].x is smaller
			out[iout++] = a1[i1++];
			} 
			else if (i1 >= n1 || ((i2 < n2) && (a1[i1].x > a2[i2].x))){ // a2[i2].x is smaller
				out[iout++] = a2[i2++];
			} 
			else{ // a1[i1].x == a2[i2].x
				if (a1[i1].y < a2[i2].y){
					out[iout++] = a1[i1++]; 
				}
				else{
					out[iout++] = a2[i2++];
				}
			}
		}
	}
	else { // if sorting by y
		while(i1 < n1 || i2 < n2) {
			if(i2 >= n2 || ((i1 < n1) && (a1[i1].y < a2[i2].y))) { // a1[i1].y is smaller
				out[iout++] = a1[i1++];
			} 
			else if (i1 >= n1 || ((i2 < n2) && (a1[i1].y > a2[i2].y))){ // a2[i2].y is smaller
				out[iout++] = a2[i2++];
			} 
			else{
				if (a1[i1].x < a2[i2].x){
					out[iout++] = a1[i1++]; 
				}
				else{
					out[iout++] = a2[i2++];
				}
			}
		}
	}
}
// ---------------------------------------------------------------------------------------
void mergeSort(int n, const location a[], location out[], int param){
	location *a1;
    location *a2;
	if(n < 2) {
		memcpy(out, a, sizeof(location) * n); // 0 or 1 elements is already sorted
	} 
	else {
		/* sort into temp arrays */
		a1 = malloc(sizeof(location) * (n/2));
		a2 = malloc(sizeof(location) * (n - n/2));
		mergeSort(n/2, a, a1, param);
    	mergeSort(n - n/2, a + n/2, a2, param);
    	/* merge results */
    	merge(n/2, a1, n - n/2, a2, out, param);
    	/* free the temp arrays */
		free(a1);
		free(a2); 
	}
}

// ---------------------------------------------------------------------------------------
node *make_subtree(node *curr, location *pts1, location *pts2, int n, int depth){
	if (n<=0){ // if the arrays passed in are empty
		free(pts1);
		free(pts2);
		return NULL;
	}
	location *a1X = malloc(sizeof(location)*(n-(n/2)-1)); // x sorted of left subtree
	location *a1Y = malloc(sizeof(location)*(n-(n/2)-1)); // y sorted of left subtree
	location *a2X = malloc(sizeof(location)*(n/2)); // x sorted of right subtree
	location *a2Y = malloc(sizeof(location)*(n/2)); // y sorted of right subtree
	int ia1 = 0;
	int ia2 = 0;
	int mid = (n-1)/2; // new root
	curr = malloc(sizeof(node));
	if (curr == NULL || a1X == NULL || a2X == NULL || a1Y == NULL || a2Y == NULL){
		free(pts1);
		free(pts2);
		return curr;
	}
	curr->loc = pts1[mid];
	if (depth%2==1){ // cutting by x
		for (int i=0;i<mid;i++){ // first half of x sorted to left subtree
			a1X[i] = pts1[i];
		}
		for (int i=mid+1;i<n;i++){ // second half of x sorted to right subtree
			a2X[i-(mid+1)] = pts1[i];
		}
		for (int i=0;i<n;i++){ // goes in y-sorted order, so the resulting 2 arrays will also end up y-sorted
			if (pts2[i].x < curr->loc.x){ // if x is smaller than middle, go to left subtree
				a1Y[ia1++] = pts2[i];
			}
			else if (pts2[i].x > curr->loc.x){ // if x is greater, go to right subtree
				a2Y[ia2++] = pts2[i];
			}
			else{
				if (pts2[i].y < curr->loc.y){ // use y as tiebreaker
					a1Y[ia1++] = pts2[i];
				}
				else if (pts2[i].y > curr->loc.y){
					a2Y[ia2++] = pts2[i];
				}
			}
		}
		free(pts1);
		free(pts2);
		curr->left = make_subtree(curr->left, a1Y, a1X, (n-(n/2)-1),depth+1); // recurse; notice that the x and y flip
		curr-> right = make_subtree(curr->right, a2Y, a2X, n/2,depth+1);
	}
	else{ // cutting by y
		for (int i=0;i<mid;i++){ // first half of y sorted to left subtree
			a1Y[i] = pts1[i];
		}
		for (int i=mid+1;i<n;i++){ // second half of y sorted to right subtree
			a2Y[i-(mid+1)] = pts1[i];
		}
		for (int i=0;i<n;i++){ // goes in x-sorted order
			if (pts2[i].y < curr->loc.y){ // same as above, just opposite dimension
				a1X[ia1++] = pts2[i];
			}
			else if (pts2[i].y > curr->loc.y){
				a2X[ia2++] = pts2[i];
			}
			else{
				if (pts2[i].x < curr->loc.x){
					a1X[ia1++] = pts2[i];
				}
				else if (pts2[i].x > curr->loc.x){
					a2X[ia2++] = pts2[i];
				}
			}
		}
		free(pts1);
		free(pts2);
		curr->left = make_subtree(curr->left, a1X, a1Y, (n-(n/2)-1),depth+1);
		curr->right = make_subtree(curr->right, a2X, a2Y, n/2,depth+1);
	}	
	return curr;
}
// ---------------------------------------------------------------------------------------
node *insert(node *curr, const location *p, int depth){
	if (curr == NULL){ // if current node is a leaf, make new node and return it
		curr = malloc(sizeof(node));
		if (curr == NULL){
			return NULL;
		}
		curr->loc.x = p->x;
		curr->loc.y = p->y;
		curr->left = NULL;
		curr->right = NULL;
		return curr;
	}
	node *garbage;
	if (depth%2==1){ // cutting by x
		if (p->x > curr->loc.x){ // if greater, go to right subtree
			garbage = insert(curr->right, p, depth+1);
			if (garbage != NULL){ // if it does not return NULL, means it was created and we can set it equal to what was returned
				curr->right = garbage;
			}
			else{ // if it's NULL, that means it's already in the tree and we don't want to set it to NULL
				return NULL;
			}
		}
		else if (p->x < curr->loc.x){ // if less than, go to left subtree
			garbage = insert(curr->left, p, depth+1);
			if (garbage != NULL){
				curr->left = garbage;
			}
			else{
				return NULL;
			}
		}
		else{
			if (p->y > curr->loc.y){ // use y dimension as a tiebreaker
				garbage = insert(curr->right, p, depth+1);
				if (garbage != NULL){
					curr->right = garbage;
				}
				else{
					return NULL;
				}
			}
			else if (p->y < curr->loc.y){
				garbage = insert(curr->left, p, depth+1);
				if (garbage != NULL){
					curr->left = garbage;
				}
				else{
					return NULL;
				}
			}
			else{
				return NULL;
			}
		}
	}
	else{ // cutting by y
		if (p->y > curr->loc.y){ // if greater, go to right subtree
			garbage = insert(curr->right, p, depth+1);
				if (garbage != NULL){
					curr->right = garbage;
				}
				else{
					return NULL;
				}
		}
		else if (p->y < curr->loc.y){ // if less than, go to left subtree
			garbage = insert(curr->left, p, depth+1);
			if (garbage != NULL){
				curr->left = garbage;
			}
			else{
				return NULL;
			}
		}
		else{
			if (p->x > curr->loc.x){ // use x dimension as tiebreaker
				garbage = insert(curr->right, p, depth+1);
				if (garbage != NULL){
					curr->right = garbage;
				}
				else{
					return NULL;
				}
			}
			else if (p->x < curr->loc.x){
				garbage = insert(curr->left, p, depth+1);
				if (garbage != NULL){
					curr->left = garbage;
				}
				else{
					return NULL;
				}
			}
			else{
				return NULL;
			}
		}
	}
	return curr;
}
// ---------------------------------------------------------------------------------------
kdtree *kdtree_create(const location *pts, int n){
	kdtree *t = malloc(sizeof(kdtree)); // malloc space for new tree
	if (t == NULL){
		return NULL;
	}
	t->root = NULL;
	if (pts == NULL || n==0){ // if the given array is empty
		return t;
	}
	location *outX = malloc(sizeof(location)*n); // array will be x-sorted
	location *outY = malloc(sizeof(location)*n); // array will be y-sorted
	if (outX == NULL || outY == NULL){
		return NULL;
	}
	mergeSort(n,pts,outX,1);
	mergeSort(n,pts,outY,2);
	t->root = make_subtree(t->root, outX, outY, n, 1); // begin making subtrees at the root
	return t; // return tree
}
// ---------------------------------------------------------------------------------------
bool kdtree_add(kdtree *t, const location *p){
	if (t->root == NULL){ // if tree is empty, add the first node
		t->root = insert(t->root, p, 1);
		return true;
	}
	return (insert(t->root, p, 1) != NULL); // if tree is not empty
}
// ---------------------------------------------------------------------------------------
bool kdtree_contains(const kdtree *t, const location *p){
	node *curr = t->root;
	int depth = 1;
	while (curr != NULL){ // traverse through tree until we find the point or get to a NULL node
		if (depth%2==1){ // cutting by x
			if (p->x > curr->loc.x){
				curr = curr->right;
			}
			else if (p->x < curr->loc.x){
				curr = curr->left;
			}
			else{
				if (p->y > curr->loc.y){
					curr = curr->right;
				}
				else if (p->y < curr->loc.y){
					curr = curr->left;
				}
				else{
					return true;
				}
			}
		}
		else{
			if (p->y > curr->loc.y){
				curr = curr->right;
			}
			else if (p->y < curr->loc.y){
				curr = curr->left;
			}
			else{
				if (p->x > curr->loc.x){
					curr = curr->right;
				}
				else if (p->x < curr->loc.x){
					curr = curr->left;
				}
				else{
					return true;
				}
			}
		}
		depth++;
	}
	return false;
}
// ---------------------------------------------------------------------------------------
void nearest(node *curr, const location *p, location *neighbor, double *smallestDistance, location lowerLeft, location upperRight, int depth){
	if (curr == NULL || smallestDistance == 0){ // if the node is NULL or the query point is present in the tree
		return;
	}
	double perpendicular = location_distance_to_rectangle(p,&lowerLeft,&upperRight); // calculates distance to bounding box
	if (perpendicular >= *smallestDistance){ // if distance to bounding box is greater than the distance to the closest point already found, skip the subtree
		return;
	}
	if (location_distance(&(curr->loc),p) < *smallestDistance){ // checks distance from curr node to query node
		*smallestDistance = location_distance(&(curr->loc),p);
		*neighbor = curr->loc;
	}
	location temp; // will hold a temporary location to pass back into the function; bounding box depends on where query point is in relation to subtree
	if (depth%2==1){ // cutting by x
		if (p->x > curr->loc.x){ // check right subtree first; query point will be in right subtree, so it's inside the bounding box; for the left subtree, the query point is outside the bounding box
			temp.x = curr->loc.x; // temp takes place of lowerLeft when passing back into nearest
			temp.y = lowerLeft.y; // don't want to actually change lowerLeft, though
			nearest(curr->right, p, neighbor, smallestDistance, temp, upperRight, depth+1);
			temp.x = curr->loc.x;
			temp.y = upperRight.y;
			nearest(curr->left, p, neighbor, smallestDistance, lowerLeft, temp, depth+1);
		}
		else if (p->x < curr->loc.x){ // check left subtree first
			temp.x = curr->loc.x;
			temp.y = upperRight.y;
			nearest(curr->left, p, neighbor, smallestDistance, lowerLeft, temp, depth+1);
			temp.x = curr->loc.x;
			temp.y = lowerLeft.y;
			nearest(curr->right, p, neighbor, smallestDistance, temp, upperRight, depth+1);
		}
		else{ // recurse both subtrees if query point lies on current boundary line, but don't update bounding box
			nearest(curr->left, p, neighbor, smallestDistance, lowerLeft, upperRight, depth+1);
			nearest(curr->right, p, neighbor, smallestDistance, lowerLeft, upperRight, depth+1);
		}
	}
	else{
		if (p->y > curr->loc.y){
			temp.x = lowerLeft.x;
			temp.y = curr->loc.y;
			nearest(curr->right, p, neighbor, smallestDistance, temp, upperRight, depth+1);
			temp.x = upperRight.x;
			temp.y = curr->loc.y;
			nearest(curr->left, p, neighbor, smallestDistance, lowerLeft, temp, depth+1);
		}
		else if (p->y < curr->loc.y){
			temp.x = upperRight.x;
			temp.y = curr->loc.y;
			nearest(curr->left, p, neighbor, smallestDistance, lowerLeft, temp, depth+1);
			temp.x = lowerLeft.x;
			temp.y = curr->loc.y;
			nearest(curr->right, p, neighbor, smallestDistance, temp, upperRight, depth+1);
		}
		else{
			nearest(curr->left, p, neighbor, smallestDistance, lowerLeft, upperRight, depth+1);
			nearest(curr->right, p, neighbor, smallestDistance, lowerLeft, upperRight, depth+1);
		}
	}
}

// ---------------------------------------------------------------------------------------
void kdtree_nearest_neighbor(kdtree *t, const location *p, location *neighbor, double *d){
	*d = INFINITY; // distance is initially infinity
	location lowerLeft = {-infinity,-infinity}; // defined my own infinity to avoid overflow errors with lccation_distance
	location upperRight = {infinity,infinity};
	nearest(t->root, p, neighbor, d, lowerLeft, upperRight, 1);
}

void traverse(node *curr, void (*f)(const location *, void *), void *arg){ // traverses each node in the tree
	if (curr == NULL){
		return;
	}
	traverse(curr->left, f, arg);
	traverse(curr->right, f, arg);
	f(&(curr->loc),arg);
}

void kdtree_for_each(const kdtree* r, void (*f)(const location *, void *), void *arg){
	traverse(r->root, f, arg); // calls function for each node in tree
}

void destroy_node(node *curr){
	if (curr == NULL){
		return;
	}
	destroy_node(curr->left); // recurse to destroy left subtree
	destroy_node(curr->right); // destroy right subtree
	free(curr); // then destroy current node
}

void kdtree_destroy(kdtree *t){
	destroy_node(t->root); // start recursive destruction at root
	free(t); // free tree
}