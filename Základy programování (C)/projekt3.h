/**
 * @file proj3.h
 * @author Sipos:Marek
 * @date Monday, December 14, 2015
 * @brief Demonstrates cluster analysis based on the single linkage method
 * @details Loads raw text file containing set of objects and creates
 * cluster for each of them. Next applies single linkage method (the closest neighbour)
 * and finally prints results and exits program freeing used memory.
 */


/**
@mainpage
This simple program tries to demonstrate cluster analysis based
on the single linkage method. This method looks for closest objects
and creates groups of them (clusters), depending on number of clusters
wanted by user.

One can use this program to learn basic work with structures,
definition of functions, their parameters and return values,
work with pointers (especially pointers to structures) and
last but not least get knowledge about dynamic memory initialisation.

<em>This program is free to use and modify. Enjoy!</em>
*/

/**
 * @struct obj_t
 * @brief      Struct holding single object and its attributes
 * @var obj_t::id
 * Object identificator (should be unique for given cluster)
 * @var obj_t::x
 * X (horizontal) coordinate of object in 2D space
 * @var obj_t::y
 * Y (vertical) coordinate of object in 2D space
 */
struct obj_t {
    int id;
    float x;
    float y;
};

/**
 * @struct cluster_t
 * @brief      Struct holding single cluster and its attributes
 * @var cluster_t::size
 * Amount of objects cluster actually contains
 * @var cluster_t::capacity
 * Amount of objects cluster can contain at most
 * @var cluster_t::*obj
 * Pointer to array of objects
 */
struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};


/**
 * @defgroup ClusterWork Cluster Work
 * @brief Functions to manipulate clusters individualy
 * 
 * @defgroup ArrayWork Array Work
 * @brief Functions to manipulate the whole clusters array at once
 * 
 * @defgroup MathFunctions Math Functions
 * @brief Another useful math functions (used for distance)
 * 
 * @defgroup StandardOutput Standard Output
 * @brief Functions used to print informations about clusters and array itself
 */

/**
 * @brief      Initialises cluster 'c' allocating memory for 'cap' objects
 *
 * @param      c     Pointer to the actual cluster
 * @param[in]  cap   Needed capacity - maximum number of objects cluster can hold
 *
 * @return void
 *
 * @pre
 * -# 'c' is not NULL
 * -# 'cap' is not negative
 * @post
 * -# Cluster 'c' is initialised and is able to hold 'cap' objects
 * -# If 'cap' was equal to 0, cluster's objects array is NULL
 *
 * @ingroup ClusterWork
 */
void init_cluster(struct cluster_t *c, int cap);

/**
 * @brief      Empties cluster removing its object array from memory
 *
 * @param      c     Pointer to the actual cluster
 *
 * @return void
 *
 * @pre
 * -# 'c' is not NULL
 * @post
 * -# Cluster 'c' is initialised as empty
 * -# Memory used by cluster's objects array is freed
 *
 * @ingroup ClusterWork
 */
void clear_cluster(struct cluster_t *c);

/// Chunk of cluster objects. Value recommended for reallocation.
extern const int CLUSTER_CHUNK;

/**
 * @brief      Resizes cluster to new maximum capacity reallocating memory for objects array
 *
 * @param      c        Pointer to the actual cluster
 * @param[in]  new_cap  New needed capacity to hold objects
 *
 * @return     Returns 'c' if reallocation was successful, else returns NULL
 *
 * @pre
 * -# 'c' is not NULL
 * -# 'new_cap' should be larger than actual capacity
 * @post
 * -# Cluster 'c' is resized to 'new_cap' if reallocation was successful
 *
 * @ingroup ClusterWork
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap);

/**
 * @brief      Adds an object to the end of cluster extending it if needed
 *
 * @param      c     Pointer to the actual cluster
 * @param[in]  obj   Object to be added to the end of cluster
 *
 * @return void
 *
 * @pre
 * -# 'c' is not NULL
 * @post
 * -# Object is added to the end of cluster
 * -# Cluster 'c' is extended appropriately
 *
 * @ingroup ClusterWork
 */
void append_cluster(struct cluster_t *c, struct obj_t obj);

/**
 * @brief      Adds all objects from cluster 'c2' to 'c1' leaving 'c2' untouched.
 *               Extends 'c1' if needed and sorts its objects by id after all.
 *
 * @param      c1    Pointer to the target cluster
 * @param      c2    Pointer to the source cluster
 *
 * @return void
 *
 * @pre
 * -# 'c1' and 'c2' are not NULL
 * @post
 * -# Cluster 'c1' contains all objects of 'c2'
 * -# Cluster 'c1' is extended appropriately
 * -# Objects of 'c1' are sorted in ascending order by their identificators
 *
 * @ingroup ClusterWork
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2);

/**
 * @brief      Removes cluster with index 'idx' from cluster array 'carr' and clears that cluster.
 *               Shifts other clusters in memory to avoid blank space inside array.
 *
 * @param      carr  Pointer to the cluster array
 * @param[in]  narr  Current amount of clusters in cluster array
 * @param[in]  idx   Index of cluster to be removed in cluster array
 *
 * @return     New amount of clusters in cluster array
 *
 * @pre
 * -# 'carr' is not NULL
 * -# 'narr' is positive integer
 * -# 'idx' is less than 'narr'
 * @post
 * -# Removed cluster is cleared and memory used by its object array freed
 * -# Remaining clusters inside clusters array are shifted appropriately
 *
 * @ingroup ArrayWork
 */
int remove_cluster(struct cluster_t *carr, int narr, int idx);

/**
 * @brief      Returns distance between two objects in 2D space.
 *
 * @param      o1    Pointer to the first object
 * @param      o2    Pointer to the second object
 *
 * @return     Decimal distance
 *
 * @pre
 * -# 'o1' and 'o2' are not NULL
 *
 * @ingroup MathFunctions
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2);

/**
 * @brief      Returns cluster distance based on single linkage method.
 *
 * @param      c1    Pointer to the first cluster
 * @param      c2    Pointer to the second cluster
 *
 * @return     Decimal distance
 *
 * @pre
 * -# 'c1' and 'c2' are not NULL
 * -# 'c1' and 'c2' are not empty
 *
 * @ingroup MathFunctions
 */
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2);

/**
 * @brief      Finds the two closest clusters and saves their indexes in cluster array.
 *
 * @param           carr  Pointer to the cluster array
 * @param[in]       narr  Current amount of clusters in cluster array
 * @param[out]      c1    Pointer to the integer to save first cluster's index into
 * @param[out]      c2    Pointer to the integer to save second cluster's index into
 *
 * @return void
 *
 * @pre
 * -# 'carr' is not NULL
 * -# 'narr' is positive integer
 * @post
 * -# If 'c1' or 'c2' are not NULL, found cluster's indexes are saved there
 *
 * @ingroup MathFunctions
 */
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2);

/**
 * @brief      Sorts objects in cluster in ascending order by their identificators.
 *
 * @param      c     Pointer to the actual cluster
 *
 * @return void
 *
 * @pre
 * -# 'c' is not NULL
 * @post
 * -# Objects of cluster 'c' are sorted in memory in ascending order by their identificators
 *
 * @ingroup ClusterWork
 */
void sort_cluster(struct cluster_t *c);

/**
 * @brief      Prints cluster to standard output.
 *
 * @param      c     Pointer to the actual cluster
 *
 * @return void
 *
 * @pre
 * -# 'c' is not NULL
 * @post
 * -# Brief informations of cluster 'c' are directed to standard output
 *
 * @ingroup StandardOutput
 */
void print_cluster(struct cluster_t *c);

/**
 * @brief      Reads a file, loads objects and creates clusters for them,
 *               allocating memory needed for cluster array and adding clusters to it.
 *
 * @param      filename  The name of file to read objects from
 * @param      arr       Pointer to the adress of memory holding cluster array to write into
 *
 * @return     Amount of objects properly read and loaded
 *
 * @pre
 * -# File specified in 'filename' exists
 * -# First line of file 'filename' contains amount of objects to be loaded in format "count=N"
 * -# Other lines of file 'filename' contain objects attributes in format "ID X Y"
 * -# 'arr' is not NULL
 * @post
 * -# Properly sized cluster array is made
 * -# All objects all loaded and their clusters are saved in cluster array
 *
 * @ingroup ArrayWork
 */
int load_clusters(char *filename, struct cluster_t **arr);

/**
 * @brief      Prints 'narr' of clusters from cluster array to standard output.
 *
 * @param      carr  Pointer to the cluster array
 * @param[in]  narr  Amount of clusters to be print
 *
 * @return void
 *
 * @pre
 * -# 'carr' is not NULL
 * @post
 * -# Brief informations of first 'narr' clusters from cluster array 'narr' are directed to standard output
 *
 * @ingroup StandardOutput
 */
void print_clusters(struct cluster_t *carr, int narr);
