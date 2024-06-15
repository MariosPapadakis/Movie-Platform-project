/*****************************************************
 * @file   Movie.c                                   *
 * @author Paterakis Giorgos <geopat@csd.uoc.gr>     *
 *                                                   *
 * @brief Implementation for Movie.h 				 *
 * Project: Winter 2023						         *
 *****************************************************/

/*
 *	TO RUN THE MAKE AND TESTS,
 *	1) make all
 *	2) run <input_file> 
 *
 *	TO CLEAN THE DIRECTORY,
 *	1) make clean
 */

#include "Movie.h"

int p, a, b;	/* Parameters for the hash function */
int flag = 0;

void InOrder(movie_t *root){
	if(root==NULL || root->movieID==-1){
		return;
	}
	InOrder(root->lc);
	printf("[%d] ", root->movieID);
	InOrder(root->rc);
}

void InOrderLO(userMovie_t *root){
	if(root==NULL){
		return;
	}
	InOrderLO(root->lc);
	if(root->lc==NULL && root->rc==NULL){
		printf("    MID: %d, SCORE: %d\n", root->movieID, root->score);
	}
	InOrderLO(root->rc);
}

/**
 * @brief Find the next prime.
 * Finds the next prime number after max_id.
 * 
 * @param max_id The maximum user id
 * 
 * @return i on success
 * 	      -1 on failure
*/
int next_prime(int max_id){
	int i, j, flag;
	for(i=max_id+1; i<2*max_id; i++){
		flag = 0;
		for(j=2; j<=i/2; j++){
			if(i%j==0){
				flag = 1;
				break;
			}
		}
		if(flag==0){
			return i;
		}
	}	
	return -1;
}

/**
 * @brief Prints the user chain.
 * Prints the user chain of the hashtable.
 * 
 * @param index The index of the hashtable
*/
void printUserChain(int index){
	user_t *temp = user_hashtable_p[index];
	printf("  Chain %d of Users: ", index);
	while(temp!=NULL){
		printf("[%d] ", temp->userID);
		temp = temp->next;
	}
	printf("\n");
}


/**
 * @brief Prints all the user chains.
 * Prints all the user chains of the hashtable.
 * 
 * @param index The index of the hashtable
 */
void testAllChains(void){

	printf("User Hash Table: \n");
	int i;
	for(i=0; i<hashtable_size; i++){
		printUserChain(i);
	}
	printf("DONE\n\n");
}

/**
 * @brief Hash function.
 * Hash function for the hashtable.
 * 
 * @param userID The user id
 * 
 * @return the index of the hashtable
 * 	      
 */
int hashFunction(int userID){
	return ((a*userID + b) % p)  % hashtable_size;
}

/**
 * @brief Initialise the hashtable.
 * Initialise the hashtable with the parameters p, a, b. And also sets the size of the hashtable.
 * 
*/
void initialiseHashTable(void){
	srand(time(NULL));	/* Use current time as seed for random generator */
	p = next_prime(max_id);	/* Find the next prime number after max_id */
	a = rand() % (p-1) + 1;  //1- (p-1);	
	b = rand() % p;		//0- (p-1);
	hashtable_size = max_users * 0.65;
	if(hashtable_size < 1){
		hashtable_size = 1;
	}
	user_hashtable_p = (user_t **)malloc(hashtable_size * sizeof(user_t*));
    if(user_hashtable_p == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
}

/**
 * @brief Creates a new user.
 * Creates a new user with userID as its identification.
 *
 * @param userID The new user's identification
 *
 * @return 1 on success
 *         0 on failure
 */
int register_user(int userID)
{
	if(flag==0){
		initialiseHashTable();
		flag = 1;
	}

	int index = hashFunction(userID);

	user_t *myUser = (user_t *)malloc(sizeof(user_t));
	if(myUser==NULL){
		printf("Memory allocation failed\n");
		exit(1);
	}
	myUser->history = NULL;
	myUser->userID = userID;
	myUser->next = NULL;

	if(user_hashtable_p[index] != NULL){
		myUser->next = user_hashtable_p[index];
	}
	user_hashtable_p[index] = myUser;

	printf("R %d\n", userID);
	printUserChain(index);
	printf("DONE\n\n");
	return 1;
}


/**
 * @brief Deletes a history tree.
 * Deletes a history tree with root using PostOrder traversal.
 * 
 * @param root The root of the history tree
 * 
*/
void deleteHistoryTree(userMovie_t *root){
	if(root==NULL){
		return;
	}
	deleteHistoryTree(root->lc);
	deleteHistoryTree(root->rc);
	free(root);
}

/**
 * @brief Deletes a user.
 * Deletes a user with userID from the system, along with users' history tree.
 *
 * @param userID The new user's identification
 *
 * @return 1 on success
 *         0 on failure
 */
int unregister_user(int userID)
{
	int index = hashFunction(userID);
	user_t *temp = user_hashtable_p[index];
	user_t *prev = NULL;
	while(temp!=NULL){
		if(temp->userID == userID){
			userMovie_t *temp2 = temp->history;
			deleteHistoryTree(temp2);
			if(prev==NULL){
				user_hashtable_p[index] = temp->next;
			} else {
				prev->next = temp->next;
			}
			free(temp);
			printf("U %d\n", userID);
			printUserChain(index);
			printf("DONE\n\n");
			return 1;
		}
		prev = temp;
		temp = temp->next;
	}
	printf("User %d does not exist\n", userID);
	return 0;
}

/**
 * @brief Prints the new release movies.
 * Prints the new release movies using InOrder traversal.
 * 
 * @param temp The root of the new release movies tree
 * 
*/
void printNewMovies(new_movie_t *temp)
{
	if(temp==NULL){
		return;
	}
	printNewMovies(temp->lc);
	printf("[%d] ", temp->movieID);
	printNewMovies(temp->rc);
}

/**
 * @brief Helper function for creating a new movie. Used in add_new_movie.
 * Creates a new movie with movieID as its identifier.
 *
 * @param movieID The new movie's Identifier.
 * @param category The category of the movie
 * @param year The year movie released
 *
 * @return the new movie
 */
new_movie_t* createNewMovie(int movieID, int category, int year) {
    new_movie_t* newMovie = (new_movie_t *)malloc(sizeof(new_movie_t));
	if(newMovie==NULL){
		printf("Memory allocation failed\n");
		exit(1);
	}
    newMovie->movieID = movieID;
    newMovie->category = category;
    newMovie->year = year;
    newMovie->watchedCounter = 0;
    newMovie->sumScore = 0;
    newMovie->lc = NULL;
    newMovie->rc = NULL;
    return newMovie;
}

/**
 * @brief Add new movie to new release binary tree.
 * Create a node movie and insert it in 'new release' binary tree.
 *
 * @param movieID The new movie identifier
 * @param category The category of the movie
 * @param year The year movie released
 *
 * @return 1 on success
 *         0 on failure
 */
int add_new_movie(int movieID, int category, int year) {
    if(category<0 || category>5){
        return -1;
    }

    new_movie_t *temp = new_releases;
    if(temp == NULL){
        new_releases = createNewMovie(movieID, category, year);
    } else {
        while(temp != NULL){
            if(temp->movieID == movieID){
                printf("Movie already exists\n");
                return 0;
            }
            if(temp->movieID > movieID){
                if(temp->lc == NULL){
                    temp->lc = createNewMovie(movieID, category, year);
                    break;
                } else {
                    temp = temp->lc;
                }
            } else {
                if(temp->rc == NULL){
                    temp->rc = createNewMovie(movieID, category, year);
                    break;
                } else {
                    temp = temp->rc;
                }
            }
        }
    }

    printf("A %d %d %d\nNew releases Tree:\n  New Realeses: ", movieID, category, year);
    printNewMovies(new_releases);
    printf("\n");
    printf("DONE\n\n");
    return 1;
}

/**
 * @brief Helper function that counts how many movies have the same category.
 * Used in distribute_movies.
 * 
 * @param root The root of the new release movies tree
 * @param category The category of the movie
 * 
 * @return the number of movies with the same category
 * 
*/
int countNodes(new_movie_t *root, int category){	//Count how many movies have the same category.
	if(root==NULL){
		return 0;
	}
	if(root->category == category){
		return 1 + countNodes(root->lc, category) + countNodes(root->rc, category);
	} else {
		return countNodes(root->lc, category) + countNodes(root->rc, category);
	}
}

/**
 * @brief Helper function that inserts the movies with the same category in an array.
 * Used in distribute_movies.
 * 
 * @param root The root of the new release movies tree
 * @param Array The array that we store the movies with the same category
 * @param category The category of the movie
 * @param i The index of the array
 * 
*/
void Insert(new_movie_t *root, new_movie_t* Array[], int category, int *i){
    if(root==NULL){
        return;
    }
    Insert(root->lc, Array, category, i);
	if(root->category == category){
		Array[*i] = root;
		(*i)++;
	}
	Insert(root->rc, Array, category, i);
}

/**
 * @brief Helper function that creates a new empty tree.
 * Used in distribute_movies.
 * 
 * @param count The number of movies with the same category
 * @param sentinel The sentinel of the tree
 * 
 * @return the root of the new empty tree
 * 
*/
movie_t *MakeEmptyTree(int count, movie_t *sentinel){
	if(count==0){
		return NULL;
	}
	movie_t* array[count];
	int i;
	for(i=0; i<count; i++){
		array[i] = (movie_t *)malloc(sizeof(movie_t));
		if(array[i]==NULL){
			printf("Memory allocation failed\n");
			exit(1);
		}
		array[i]->movieID = 0;
		array[i]->year = 0;
		array[i]->watchedCounter = 0;
		array[i]->sumScore = 0;
		array[i]->lc = sentinel;
		array[i]->rc = sentinel;
	}
	for(i=0; i<count;i++){
		if(2*i+1 < count){
			array[i]->lc = array[2*i+1];
		}
		if(2*i+2 < count){
			array[i]->rc = array[2*i+2];
		}
	}
	return array[0];
}

/**
 * @brief Helper function that fills the tree with the movies with the same category.
 * Used in distribute_movies.
 * 
 * @param root The root of the new release movies tree
 * @param Array The array that we store the movies with the same category
 * @param sentinel The sentinel of the tree
 * @param i The index of the array
 * 
*/
void FillTree(movie_t *root, new_movie_t *Array[], movie_t *sentinel, int *i){
    if(root==NULL || root==sentinel){
        return;
    }

    FillTree(root->lc, Array, sentinel, i);
    root->movieID=Array[*i]->movieID;
	root->year=Array[*i]->year;
	root->watchedCounter=Array[*i]->watchedCounter;
	root->sumScore=Array[*i]->sumScore;
    (*i)++;
    FillTree(root->rc, Array, sentinel, i);
}

/**
 * @brief Helper function that deletes the new release movies tree using PostOrder traversal.
 * Used in distribute_movies.
 * 
 * @param root The root of the new release movies tree
 * 
*/
void deleteTree(new_movie_t *root){
	if(root==NULL){
		return;
	}
	deleteTree(root->lc);
	deleteTree(root->rc);
	free(root);
}

/**
 * @brief Distribute the movies from new release binary tree to the array of categories.
 *
 * @return 0 on success
 *         1 on failure
 */
int distribute_movies(void)
{
	int i,j;
	int index = 0;
	int index2 = 0;

	movie_t *sentinel = (movie_t *)malloc(sizeof(movie_t));
	if(sentinel==NULL){
		printf("Memory allocation failed\n");
		exit(1);
	}
	sentinel->movieID = -1;

	categoryArray = (movieCategory_t **)malloc(6 * sizeof(movieCategory_t*));
	if(categoryArray==NULL){
		printf("Memory allocation failed\n");
		exit(1);
	}

	new_movie_t **Array;
	for(i=0; i<6; i++){
        int count = countNodes(new_releases, i);
		categoryArray[i] = (movieCategory_t *)malloc(sizeof(movieCategory_t));
		if(categoryArray[i]==NULL){
			printf("Memory allocation failed\n");
			exit(1);
		}

        if(count==0){
            categoryArray[i]->root = sentinel;
        } else {
            Array = (new_movie_t**)malloc(count * sizeof(new_movie_t*));
			if(Array==NULL){
				printf("Memory allocation failed\n");
				exit(1);
			}

            Insert(new_releases, Array, i, &index);
            movie_t *root2 = MakeEmptyTree(count, sentinel);
            FillTree(root2, Array, sentinel, &index2);
            categoryArray[i]->root = root2;
        }

        index = index2 = 0;	/* Reset the indices for the next category */
    }

	deleteTree(new_releases);
	new_releases = NULL;

	printf("D\n");
	printf("Movie Category Array: \n");
	for(i=0; i<6; i++){
		printf("  Category %d: ", i);
		InOrder(categoryArray[i]->root);
		printf("\n");
	}
	printf("DONE\n\n");
	return 1;
}

/**
 * @brief Helper function that creates a new userMovie node.
 * Used in watch_movie.
 * 
 * @param tmp The node that we want to create a new node
 * 
 * @return the new userMovie node
 * 
*/
userMovie_t *createUserMovieNode(userMovie_t *tmp){
	userMovie_t *myUserMovie = (userMovie_t *)malloc(sizeof(userMovie_t));
	if(myUserMovie==NULL){
		printf("Memory allocation failed\n");
		exit(1);
	}
	myUserMovie->movieID = tmp->movieID;
	myUserMovie->category = tmp->category;
	myUserMovie->score = tmp->score;
	myUserMovie->parent = tmp;
	myUserMovie->lc = NULL;
	myUserMovie->rc = NULL;
	return myUserMovie;
}

/**
 * @brief Helper function that inserts a new userMovie node in the leaf-oriented History tree.
 * Used in watch_movie.
 * 
 * @param root The root of the leaf-oriented History tree
 * @param myUserMovie The new userMovie node
 * 
*/
void insertLeafOriented(userMovie_t **root, userMovie_t *myUserMovie){
	userMovie_t *parent = (*root)->parent;
	while(*root!=NULL){
		if(myUserMovie->movieID < (*root)->movieID){
			parent = *root;
			root = &((*root)->lc);
		} else {
			parent = *root;
			root = &((*root)->rc);
		}
	}

	userMovie_t *prevNode = createUserMovieNode(parent);	/* Create a copy of the Parent node */
	myUserMovie->parent = parent;
	if(myUserMovie->movieID < parent->movieID){
		parent->lc = myUserMovie;
		parent->rc = prevNode;
	} else {
		parent->movieID = myUserMovie->movieID;
		parent->lc = prevNode;
		parent->rc = myUserMovie;
	}

	return;
}

/**
 * @brief Helper function that finds a user with identification userID.
 * Used in watch_movie.
 * 
 * @param userID The identification of the user
 * 
 * @return the user
 * 
*/
user_t* findUser(int userID) {
    int index = hashFunction(userID);
    user_t *tmpUser = user_hashtable_p[index];
    while(tmpUser != NULL && tmpUser->userID != userID){
        tmpUser = tmpUser->next;
    }
    return tmpUser;
}

/**
 * @brief Helper function that finds a movie with identification movieID in a specific category.
 * Used in watch_movie.
 * 
 * @param category The category of the movie
 * @param movieID The identification of the movie
 * 
 * @return the movie
 * 
*/
movie_t* findMovie(int category, int movieID) {
    movie_t *tmpMovie = categoryArray[category]->root;
    while(tmpMovie->movieID != -1 && tmpMovie->movieID != movieID){
        tmpMovie = (tmpMovie->movieID > movieID) ? tmpMovie->lc : tmpMovie->rc;
    }
    return tmpMovie;
}

/**
 * @brief User rates the movie with identification movieID with score
 *
 * @param userID The identifier of the user
 * @param category The Category of the movie
 * @param movieID The identifier of the movie
 * @param score The score that user rates the movie with id movieID
 *
 * @return 1 on success
 *         0 on failure
 */
int watch_movie(int userID, int category, int movieID, int score)
{
	user_t *tmpUser = findUser(userID);
    if(tmpUser == NULL){
        printf("User %d does not exist\n", userID);
        return 0;
    }

    movie_t *tmpMovie = findMovie(category, movieID);
    if(tmpMovie->movieID == -1){
        printf("Movie %d does not exist\n", movieID);
        return 0;
    }
	
	tmpMovie->watchedCounter++;
	tmpMovie->sumScore += score;

	userMovie_t *myUserMovie = (userMovie_t *)malloc(sizeof(userMovie_t));
	if(myUserMovie==NULL){
		printf("Memory allocation failed\n");
		exit(1);
	}
	myUserMovie->category = category;
	myUserMovie->movieID = tmpMovie->movieID;
	myUserMovie->parent = NULL;
	myUserMovie->score = score;
	myUserMovie->lc = NULL;
	myUserMovie->rc = NULL;

	if(tmpUser->history == NULL){
		tmpUser->history = myUserMovie;
	} else {
		insertLeafOriented(&(tmpUser->history), myUserMovie);
	}

	printf("W %d %d %d %d\n", userID, category, movieID, score);
	printf("History Tree of user %d:\n", userID);
	InOrderLO(tmpUser->history);
	printf("DONE\n\n");

	return 1;
}

/**
 * @brief Helper function that finds the average score of a movie.
 * Used in filter_movies.
 * 
 * @param movie The movie
 * 
 * @return the average score of the movie
 * 
*/
double AverageMovieScore(movie_t *movie){
	if(movie == NULL) {
        fprintf(stderr, "Error: NULL pointer passed to AverageMovieScore.\n");
        return -1.0;
    }

    if(movie->watchedCounter == 0){ 
        return 0;
    }
    return (double) movie->sumScore / movie->watchedCounter;
}

/**
 * @brief Helper function that finds the number of movies that have Average score > score.
 * Used in filter_movies.
 * 
 * @param root The root of the new release movies tree
 * @param score The score that we want to compare with
 * @param numMovies The number of movies that have score >= score
 * 
*/
void GetNumMovies(movie_t *root, int score, int *numMovies){
	if(root==NULL || root->movieID==-1){
		return;
	}
	GetNumMovies(root->lc, score, numMovies);

	if(AverageMovieScore(root) > score && root->watchedCounter != 0){
        (*numMovies)++;
    }
	GetNumMovies(root->rc, score, numMovies);
}

/**
 * @brief Helper function that traverses the movie tree of each category and stores pointers to the nodes of the tree corresponding to movies with an average score greater than the score in the numMovies table.
 * Used in filter_movies.
 * 
 * @param root The root of the new release movies tree
 * @param array The array that we store the movies with the same category
 * @param i The index of the array
 * @param score The score that we want to compare with
 * 
*/
void TraverseAndStore(movie_t *root, movie_t **array, int *i, int score) {
    if(root == NULL) {
        return;
    }

    TraverseAndStore(root->lc, array, i, score);
	float averageScore = 0.0;
	if(root->watchedCounter != 0) {
		averageScore = AverageMovieScore(root);
    }
    if(averageScore > score) {
        array[*i] = root;
        (*i)++;
    }
    TraverseAndStore(root->rc, array, i, score);
}

/**
 * @brief Helper function that swaps two movies.
 * Used in heapSort.
 * 
 * @param a The first movie
 * @param b The second movie
 * 
*/
void swap(movie_t **a, movie_t **b) {
	movie_t *temp = *a;
	*a = *b;
	*b = temp;
}

/**
 * @brief Helper function that checks if the childIndex is larger than the largestIndex.
 * Used in heapSort.
 * 
 * @param arr The array of movies
 * @param childIndex The index of the child
 * @param largestIndex The index of the largest
 * @param n The size of the array
 * 
 * @return 1 if childIndex is larger than largestIndex
 *         0 if childIndex is not larger than largestIndex
 * 
*/
int isLargerChild(movie_t **arr, int childIndex, int largestIndex, int n) {
    return childIndex < n && arr[childIndex] && AverageMovieScore(arr[childIndex]) > AverageMovieScore(arr[largestIndex]);
}

/**
 * @brief Helper function that heapifies a subtree.
 * Used in heapSort.
 * 
 * @param arr The array of movies
 * @param n The size of the array
 * @param i The index of the array
 * 
*/
void heapify(movie_t **arr, int n, int i) {
    /* Find largest among root, left child and right child */
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    if(isLargerChild(arr, left, largest, n)) {
        largest = left;
    }

    if(isLargerChild(arr, right, largest, n)) {
        largest = right;
    }

    /* Swap and continue heapifying if root is not largest */
    if (largest != i) {
        swap(&arr[i], &arr[largest]);

        /* Heapify the affected sub-tree recursively */
        heapify(arr, n, largest);
    }
}

/**
 * @brief Heap sort the array of movies in descending order of average score.
 * 
 * @param arr The array of movies
 * @param n The size of the array
 * 
*/
void heapSort(movie_t **arr, int n) {
	/* Build max heap */
	for (int i = (n / 2) - 1; i >= 0; i--)
		heapify(arr, n, i);

	/* Heap sort */
	for (int i = n - 1; i >= 0; i--) {
		swap(&arr[0], &arr[i]);

		/* Heapify root element to get highest element at root again */
		heapify(arr, i, 0);
	}
}

/**
 * @brief Identify the best rating score movie and cluster all the movies of a category.
 *
 * @param userID The identifier of the user
 * @param score The minimum score of a movie
 *
 * @return 1 on success
 *         0 on failure
 */
int filter_movies(int userID, int score)
{
	int i;
	int numMovies = 0;
	int index2 = 0;
	user_t *tmpUser = findUser(userID);
    if(tmpUser == NULL){
        printf("User %d does not exist\n", userID);
        return 0;
    }
	
	for(i=0; i<6; i++){
		movie_t *tmpMovie = categoryArray[i]->root;
		GetNumMovies(tmpMovie, score, &numMovies);
	}

	/* Create an array of size numMovies and fill it with the movies that have Average score > score. */
	movie_t **Array = malloc(numMovies * sizeof(movie_t*));
	if(Array==NULL){
		printf("Memory allocation failed\n");
		exit(1);
	}
	
	/* Traverse the movie tree of each category and store pointers to the nodes of the tree corresponding to movies with an average score > the score in the numMovies table. */
	for(i=0; i<6; i++){
		movie_t *rootCopy = categoryArray[i]->root;
		TraverseAndStore(rootCopy, Array, &index2, score);
	}

	/* HeapSort the array of movies in descending order of average score. */
	heapSort(Array, numMovies);

	printf("F %d %d\n", userID, score);
	/* printf("Number of movies: %d\n", numMovies); */
	if(numMovies==0){
			printf("  No movies found :(\n");
	}
	else{
		for(i=0; i<numMovies; i++){
			printf("  MID: [%d], AVG SCORE: [%.3f]\n", Array[i]->movieID, AverageMovieScore(Array[i]));
		}
	}

	free(Array);

	printf("DONE\n\n");
	return 1;
}


/**
 * @brief Helper function that traverses the leaf-oriented History tree and goes up to the root.
 * Used in user_stats.
 * 
 * @param root The root of the leaf-oriented History tree
 * 
*/
void traverseUp(userMovie_t **root, userMovie_t **parentNode) {
    if (*parentNode != NULL && (*parentNode)->rc == *root) {
        *root = *parentNode;
        *parentNode = (*parentNode)->parent;
        traverseUp(root, parentNode);
    }
}

/**
 * @brief Helper function that finds the next leaf of the leaf-oriented History tree.
 * Used in user_stats.
 * 
 * @param root The root of the leaf-oriented History tree
 * @param scoreSum The sum of the scores of the movies
 * @param countOfMovies The number of movies
 * 
*/
userMovie_t *findNextLeaf(userMovie_t *root){
    userMovie_t *nextLeaf, *parentNode;

    parentNode = root->parent;
	traverseUp(&root, &parentNode);

    if(parentNode == NULL){
        return NULL;
    }

    nextLeaf = parentNode->rc;
    while(nextLeaf->lc != NULL){
        nextLeaf = nextLeaf->lc;
    }

    return nextLeaf;
}

/**
 * @brief Find movies from categories withn median_score >= score t
 *
 * @param userID The identifier of the user
 * @param category Array with the categories to search.
 * @param score The minimum score the movies we want to have
 *
 * @return 1 on success
 *         0 on failure
 */
int user_stats(int userID)
{
	int countOfMovies = 0;
	double scoreSum = 0.0;
	double avgScore;

	user_t *tmpUser = findUser(userID);
    if(tmpUser == NULL){
        printf("User %d does not exist\n", userID);
        return 0;
    }

	userMovie_t *tmp = tmpUser->history;
	if(tmp == NULL) {
        fprintf(stderr, "Error: User has no movie history.\n");
        return 0;
    }

	/* Find the Leftmost Leaf */
	while(tmp->lc!=NULL){
		tmp = tmp->lc;
	}

	while(tmp!=NULL){
		countOfMovies++;
		scoreSum += tmp->score;
		tmp = findNextLeaf(tmp);
	}

	if(countOfMovies > 0){
		avgScore = scoreSum / countOfMovies; 
		printf("Q User: %d Mean: %.1f\n", userID, avgScore);
	} else {
		printf("User: %d has not watched any movies.\n", userID);
	}
	printf("DONE\n\n");

	return 1;
}

/**
 * @brief Search for a movie with identification movieID in a specific category.
 *
 * @param movieID The identifier of the movie
 * @param category The category of the movie
 *
 * @return 1 on success
 *         0 on failure
 */
int search_movie(int movieID, int category)
{
	if(category<0 || category>5){
		return 0;
	}

	movie_t *tmpMovie = categoryArray[category]->root;
	if(tmpMovie == NULL) {
        fprintf(stderr, "Error: NULL pointer in category array.\n");
        return 0;
    }

	while(tmpMovie->movieID!=-1){
		if(tmpMovie->movieID == movieID){
			printf("I %d %d %d\n", movieID, category, tmpMovie->year);
			printf("  Movie %d Found in Category %d\n", movieID, category);
			printf("DONE\n\n");
			return 1;
		} else if(tmpMovie->movieID > movieID){
			tmpMovie = tmpMovie->lc;
		} else {
			tmpMovie = tmpMovie->rc;
		}
	}
	return 0;
}

/**
 * @brief Prints the movies in movies categories array.
 * @return 1 on success
 *         0 on failure
 */
int print_movies(void)
{
	printf("M\n");
	printf("Movie Category Array: \n");
	int i;
	for(i=0; i<6; i++){
		printf("  Category %d: ", i);
		InOrder(categoryArray[i]->root);
		printf("\n");
	}
	printf("DONE\n\n");

	return 1; 
}

/**
 * @brief Prints the users in the hashtable.
 * @param index The index of the hashtable
 */
void printUsersEvent(int index){
	user_t *temp = user_hashtable_p[index];
	printf("Chain %d of Users: \n", index);
	while(temp!=NULL){
		printf("  UID: %d \n", temp->userID);
		printf("  History Tree: \n");
		InOrderLO(temp->history);
		temp = temp->next;
	}
	printf("\n");
}

/**
 * @brief Prints the users hashtable.
 * @return 1 on success
 *         0 on failure
 */
int print_users(void)
{
	printf("P\n");
	int i;
	for(i=0; i<hashtable_size; i++){
		printUsersEvent(i);
	}
	printf("DONE\n\n");
	return 1;
}
