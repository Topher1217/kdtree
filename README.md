# kdtree
Simple kd tree implementation to determine a tour of US cities based on nearest neighbor algorithm.

Creates a 2-dimensional kd tree by taking a list of coordinate pairs and inserting the pairs sequentially. Uses a merge sort implementation to order the list of coordinate pairs initially. Then uses recursion to insert the coordinate pairs and construct the complete kd tree. Also has functionality to calculate nearest neighbor pairs based on bounding boxes and can traverse the entire tree in order.
