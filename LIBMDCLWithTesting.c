#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/*
Author: Perengrine
Purpose: Modular Double Circular Linked List
Dependancies: stdbool.h (to get proper return values). Struct Definitions need to be included in the header file
Examples: (Note that CreateList must always be used first and DeleteList must ALWAYS be used before termination of a program) (EVERY LIST MUST BE DELETED TO FREE THE MEMORY)

	Function:	CreateList(int NodeCount, const char* ListID)
	Example:	struct NodeManager* MyListManager = CreateList(10, "MyListName");

	Function:	InsertNodeAtEnd(struct NodeManager*);
	Example:	struct Node* MyNewEndNode = InsertNodeAtEnd(MyListManager);

	Function:	InsertNodeAtBeginning(struct NodeManager*);
	Example:	struct Node* MyNewBeginningNode = InsertNodeAtBeginning(MyListManager);

	Function:	InsertNodeAtPosition(struct NodeManager*, int position);
	Example:	struct Node* MyNewPositionedNode = InsertNodeAtPosition(MyListManager, 3);

	Function:	SetNodeData(struct NodeManager*, int position, void* data);
	Example:	int MyDataIWantToStoreInANode = 432
				bool SuccessOnTrueFailiureOnFalse = SetNodeData(MyListManager, 7, &MyDataIWantToStoreInANode);

	Function:	EmptyNodeData(struct NodeManager*, int position);
	Example:	bool SuccessOnTrueFailiureOnFalse = EmptyNodeData(MyListManager, 7);

	Function:	DeleteNodeAtEnd(struct NodeManager*);
	Example:	bool SuccessOnTrueFailiureOnFalse = DeleteNodeAtEnd(MyListManager);

	Function:	DeleteNodeAtBeginning(struct NodeManager*);
	Example:	bool SuccessOnTrueFailiureOnFalse = DeleteNodeAtBeginning(MyListManager);

	Function:	DeleteNodeAtPosition(struct NodeManager*);
	Example:	bool SuccessOnTrueFailiureOnFalse = DeleteNodeAtPosition(MyListManager, 4);

	Function:	DeleteList(struct NodeManager*);
	Example:	bool SuccessOnTrueFailiureOnFalse = DeleteList(MyListManager);

*/


/* Structure Definitions */

struct Node  				// the struct containing all the information a Node will store
{
	void* data;			// dynamically allocated variable to store strings
	int position;			// node position in list
	struct Node* next;		// the next node in list 0 being all the way at the left, next would point to 1 on the right, if tail node, this node will point to head
	struct Node* prev;		// the previous node in the list. If this node is the head node, it will point to the tail
};

struct NodeManager  			// this is the wrapper for all nodes in an instance of NodeManager. Each array of nodes (Determined at list creation) is managed through this
{
	char ListID[30];		// used to Identify the specific list
	int NodeCount;			// the amount of nodes to be created by the create list function
	struct Node* head;		// should always point to the 0 index of the node array
	struct Node* tail;		// should always point to the last index of the node array
	struct Node* NodeArray;		// this is a pointer to the node array for this instance of NodeManager
};


/* Function Prototypes */

struct NodeManager* CreateList		(int NodeCount, const char* ListID);			// creates a list (an instance of NodeManager) with the specified amount of nodes
struct Node* InsertNodeAtEnd		(struct NodeManager*);					// inserts a node at the end of the list (to the right), this becomes the tail
struct Node* InsertNodeAtBeginning	(struct NodeManager*);					// inserts a node at the beginning of the list (to the left), this becomes the head
struct Node* InsertNodeAtPosition	(struct NodeManager*, int position);			// inserts a node at a given position which corrosponds to the node's index
bool DeleteList				(struct NodeManager*);					// deletes a given list and returns whether the operaiton was successful or not
bool DeleteNodeAtEnd			(struct NodeManager*);					// deletes the tail and returns whether the operaiton was successful or not
bool DeleteNodeAtBeginning		(struct NodeManager*);					// deletes the head and returns whether the operaiton was successful or not
bool DeleteNodeAtPosition		(struct NodeManager*, int position);			// deletes a node at a specific node and returns the success of the operation
bool SetNodeData			(struct NodeManager*, int position, void* data);	// sets the char data of a node at the specified position for the given list
bool EmptyNodeData			(struct NodeManager*, int position);			// sets the char data of a given node to 0s for the given list

/* Functions */

struct NodeManager* CreateList(int NodeCount, const char* ListID)
{
	// error checking node count
	if (NodeCount <= 0) 							// this is <= 0 and not just <0 because of how calloc works when allocating memory
	{
		fprintf(stderr, "\nInvalid NodeCount\n");
		return NULL;
	}

	// creating an instance of NodeManager called manager
	struct NodeManager* manager = calloc(1, sizeof(struct NodeManager));	// allocating memory for our node manager on the heap

	// error checking NodeManager
	if (manager == NULL)
	{
		perror("Failed to allocate memory for NodeManager");
		return NULL;
	}

	// creating array of struct nodes
	struct Node* NodeArray = calloc(NodeCount, sizeof(struct Node));	// allocating memory for our array of nodes on the heap

	// error checking NodeArray
	if (NodeArray == NULL)
	{
		perror("Failed to allocate memory for NodeArray");
		free(manager);
		return NULL;
	}

	// initializing NodeManager variables
	strncpy(manager->ListID, ListID, sizeof(manager->ListID) - 1);	// copying the passed in list id to the variable in the nodemanager instance
	manager->ListID[sizeof(manager->ListID) - 1] = '\0'; 		// ensuring there is a null terminator at the end of ListID
	manager->NodeCount = NodeCount;
	manager->head = &NodeArray[0];
	manager->tail = &NodeArray[NodeCount - 1];
	manager->NodeArray = NodeArray;

	// initializing array of nodes/setting links
	for (int i = 0; i < NodeCount; i++)
	{
		if (i == 0) 						// if the current node in the loop is the head node
		{
			NodeArray[i].prev = &NodeArray[NodeCount - 1];
			NodeArray[i].next = &NodeArray[i + 1];
		}
		else if (i == NodeCount - 1) 				// if the current node in the loop is the tail node
		{
			NodeArray[i].prev = &NodeArray[i - 1];
			NodeArray[i].next = &NodeArray[0];
		}
		else
		{
			NodeArray[i].prev = &NodeArray[i - 1];
			NodeArray[i].next = &NodeArray[i + 1];
		}

		NodeArray[i].position = i;
	}


	return manager;							// returns a pointer to the manager struct which contains the needed information to deal with the list
}


struct Node* InsertNodeAtEnd(struct NodeManager* NodeManager) 		// Realloc for expanding, do error checking
{
	// error checking passed in NodeManager pointer
	if (NodeManager == NULL)
	{
		fprintf(stderr, "\nNodeManager in InsertNodeAtEnd Function is NULL\n");
		return NULL;
	}

	// setting local variables to NodeManager's NodeCount and NodeArray for readability
	int NodeCount = NodeManager->NodeCount;
	struct Node* NodeArray = NodeManager->NodeArray;

	// reallocating memory for the new node
	struct Node* TempNodeArray = realloc(NodeArray, (NodeCount + 1) * sizeof(struct Node));

	// error checking our newly allocated memory
	if (TempNodeArray == NULL)
	{
		perror("Failed to allocate memory for TempNodeArray");
		return NULL;
	}

	// updating NodeManager variables and updating our corrosponding variables to the new NodeManager ones
	NodeManager->NodeCount += 1;
	NodeManager->NodeArray = TempNodeArray;
	NodeCount = NodeManager->NodeCount;
	NodeArray = NodeManager->NodeArray;
	
	// reassigning head, tail, prev, and next addresses incase of a new memory block when reallocating memory
	NodeManager->head = &NodeArray[0];
	NodeManager->tail = &NodeArray[NodeCount - 1];
	for (int i = NodeCount - 1; i >= 0; i--)
	{

		if (i == 0)						// updating head node
		{
			
			NodeArray[i].position = 0;
			NodeArray[i].prev = &NodeArray[NodeCount - 1];
			NodeArray[i].next = &NodeArray[i + 1];
		}
		else if (i == NodeCount - 1)				// creating our new node
		{
			NodeArray[i].next = &NodeArray[0];
			NodeArray[i].prev = &NodeArray[i - 1];
			NodeArray[i].position = i;
		}
		else 							// updating all other nodes
		{	
			NodeArray[i].next = &NodeArray[i + 1];
			NodeArray[i].prev = &NodeArray[i - 1];
			NodeArray[i].position = i;
		}

	}


	// returning pointer to the new end node
	return NodeManager->tail;
}


struct Node* InsertNodeAtBeginning(struct NodeManager* NodeManager)
{
	// error checking passed in NodeManager pointer
	if (NodeManager == NULL)
	{
		fprintf(stderr, "\nNodeManager in InsertNodeAtBeginning Function is NULL\n");
		return NULL;
	}

	// setting local variables to NodeManager's NodeCount and NodeArray for readability
	int NodeCount = NodeManager->NodeCount;
	struct Node* NodeArray = NodeManager->NodeArray;

	// reallocating memory for the new node
	struct Node* TempNodeArray = realloc(NodeArray, (NodeCount + 1) * sizeof(struct Node));

	// error checking our newly allocated memory
	if (TempNodeArray == NULL)
	{
		perror("Failed to allocate memory for TempNodeArray");
		return NULL;
	}

	// updating NodeManager variables and updating our corrosponding variables to the new NodeManager ones
	NodeManager->NodeCount += 1;
	NodeManager->NodeArray = TempNodeArray;
	NodeCount = NodeManager->NodeCount;
	NodeArray = NodeManager->NodeArray;

	// reassigning head and tail addresses incase of a new memory block when reallocating memory
	NodeManager->head = &NodeArray[0];
	NodeManager->tail = &NodeArray[NodeCount - 1];


	// shifting loop
	for (int i = NodeCount - 1; i > 0; i--)
	{

		NodeArray[i - 1].position += 1;
		NodeArray[i] = NodeArray[i - 1];

	}

	// reassigning head, tail, prev, and next addresses incase of a new memory block when reallocating memory
	for (int i = NodeCount - 1; i >= 0; i--)
	{

		if (i == 0) // creating our new node
		{
			
			NodeArray[i].position = 0;
			NodeArray[i].prev = &NodeArray[NodeCount - 1];
			NodeArray[i].next = &NodeArray[i + 1];
		}
		else if (i == NodeCount - 1) // updating tail node
		{
			NodeArray[i].next = &NodeArray[0];
			NodeArray[i].prev = &NodeArray[i - 1];
			NodeArray[i].position = i;
		}
		else // updating all other nodes
		{
			NodeArray[i].next = &NodeArray[i + 1];
			NodeArray[i].prev = &NodeArray[i - 1];
			NodeArray[i].position = i;
		}

	}


	// returning pointer to the new beginning node
	return NodeManager->head;
}


struct Node* InsertNodeAtPosition(struct NodeManager* NodeManager, int position)
{
	// error checking passed in NodeManager pointer
	if (NodeManager == NULL)
	{
		fprintf(stderr, "\nNodeManager in InsertNodeAtPosition Function is NULL\n");
		return NULL;
	}


	// setting local variables to NodeManager's NodeCount and NodeArray for readability
	int NodeCount = NodeManager->NodeCount;
	struct Node* NodeArray = NodeManager->NodeArray;

	if (position < 0 || position > NodeCount)
	{
		fprintf(stderr, "\nPosition arguement is out of bounds\n");
		return NULL;
	}

	// reallocating memory for the new node
	struct Node* TempNodeArray = realloc(NodeArray, (NodeCount + 1) * sizeof(struct Node));

	// error checking our newly allocated memory
	if (TempNodeArray == NULL)
	{
		perror("Failed to allocate memory for TempNodeArray");
		return NULL;
	}

	// updating NodeManager variables and updating our corrosponding variables to the new NodeManager ones
	NodeManager->NodeCount += 1;
	NodeManager->NodeArray = TempNodeArray;
	NodeCount = NodeManager->NodeCount;
	NodeArray = NodeManager->NodeArray;

	// end and beginning edge case handling
	if (position == NodeCount - 1)
	{
		InsertNodeAtEnd(NodeManager);
		return false;
	}
	else if (position == 0)
	{
		InsertNodeAtBeginning(NodeManager);
		return false;
	}

	// reassigning head and tail addresses incase of a new memory block when reallocating memory
	NodeManager->head = &NodeArray[0];
	NodeManager->tail = &NodeArray[NodeCount - 1];


	// shifting loop
	for (int i = NodeCount - 1; i > position; i--)
	{
		NodeArray[i] = NodeArray[i - 1];
		NodeArray[i].position = i;
	}

	// reassigning head, tail, prev, and next addresses incase of a new memory block when reallocating memory
	for (int i = NodeCount - 1; i >= 0; i--)
	{

		if (i == 0) // creating new node, add a edge case for positions that are at 0 or tail
		{
			NodeManager->head = &NodeArray[i];
			NodeManager->tail = &NodeArray[NodeCount - 1];
			NodeArray[i].prev = &NodeArray[NodeCount - 1];
			NodeArray[i].next = &NodeArray[i + 1];
		}
		else if (i == NodeCount - 1) // updating tail node
		{
			NodeArray[i].next = &NodeArray[0];
			NodeArray[i].prev = &NodeArray[i - 1];
			NodeArray[i].position = i;
		}
		else // updating all other nodes
		{
			NodeArray[i].next = &NodeArray[i + 1];
			NodeArray[i].prev = &NodeArray[i - 1];
			NodeArray[i].position = i;
		}

	}


	return &NodeManager->NodeArray[position];
}


bool DeleteList(struct NodeManager* NodeManager)
{
	// error checking passed in NodeManager pointer
	if (NodeManager == NULL)
	{
		fprintf(stderr, "\nNodeManager in DeleteList Function is NULL\n");
		return false;
	}
	
	// freeing nodemanager memory	
	free(NodeManager);

	// setting nodemanager to NULL for error checking purposes
	NodeManager = NULL;

	return true;
}


bool DeleteNodeAtEnd(struct NodeManager* NodeManager)
{
	// error checking passed in NodeManager pointer
	if (NodeManager == NULL)
	{
		fprintf(stderr, "\nNodeManager in DeleteNodeAtEnd Function is NULL\n");
		return false;
	}

	// setting local variables to NodeManager's NodeCount and NodeArray for readability
	NodeManager->NodeCount -= 1;
	int NodeCount = NodeManager->NodeCount;
	struct Node* NodeArray = NodeManager->NodeArray;

		// shrinking memory
	struct Node* TempNodeArray = realloc(NodeArray, (NodeCount) * sizeof(struct Node));

	// error checking our newly allocated memory
	if (TempNodeArray == NULL)
	{
		perror("Failed to allocate memory for TempNodeArray");
		return false;
	}

	NodeManager->NodeArray = TempNodeArray;
	NodeArray = NodeManager->NodeArray;

	for (int i = NodeCount - 1; i >= 0; i--)
	{

		if (i == 0)						// updating head node
		{
			NodeArray[i].prev = &NodeArray[NodeCount - 1];
			NodeArray[i].next = &NodeArray[i + 1];
		}
		else if (i == NodeCount - 1)				// setting the new tail node
		{
			NodeArray[i].next = &NodeArray[0];
			NodeArray[i].prev = &NodeArray[i - 1];
			NodeArray[i].position = i;
			NodeManager->tail = &NodeArray[i];
		}
		else							// updating all other nodes
		{
			NodeArray[i].next = &NodeArray[i + 1];
			NodeArray[i].prev = &NodeArray[i - 1];
			NodeArray[i].position = i;
		}
	}


	return true;
}


bool DeleteNodeAtBeginning(struct NodeManager* NodeManager)
{
	// error checking passed in NodeManager pointer
	if (NodeManager == NULL)
	{
		fprintf(stderr, "\nNodeManager in DeleteNodeAtBeginning Function is NULL\n");
		return false;
	}

	// setting local variables to NodeManager's NodeCount and NodeArray for readability
	NodeManager->NodeCount -= 1;
	int NodeCount = NodeManager->NodeCount;
	struct Node* NodeArray = NodeManager->NodeArray;


	// shifting loop
	for (int i = 0; i < NodeCount; i++)
	{
		NodeArray[i] = NodeArray[i + 1];
		NodeArray[i].position = i;
	}


	// shrinking memory
	struct Node* TempNodeArray = realloc(NodeArray, (NodeCount) * sizeof(struct Node));

	// error checking our newly allocated memory
	if (TempNodeArray == NULL)
	{
		perror("Failed to allocate memory for TempNodeArray");
		return false;
	}

	NodeManager->NodeArray = TempNodeArray;
	NodeArray = NodeManager->NodeArray;

	for (int i = NodeCount - 1; i >= 0; i--)
	{

		if (i == 0)						// updating head node
		{
			
			NodeArray[i].position = 0;
			NodeArray[i].prev = &NodeArray[NodeCount - 1];
			NodeArray[i].next = &NodeArray[i + 1];
			NodeManager->head = &NodeArray[i];

		}
		else if (i == NodeCount - 1)				// setting the new tail node
		{
			NodeArray[i].next = &NodeArray[0];
			NodeArray[i].prev = &NodeArray[i - 1];
			NodeArray[i].position = i;
			NodeManager->tail = &NodeArray[i];
		}
		else							// updating all other nodes
		{
			NodeArray[i].next = &NodeArray[i + 1];
			NodeArray[i].prev = &NodeArray[i - 1];
			NodeArray[i].position = i;
		}
	}

	return true;
}


bool DeleteNodeAtPosition(struct NodeManager* NodeManager, int position)
{
	// error checking passed in NodeManager pointer
	if (NodeManager == NULL)
	{
		fprintf(stderr, "\nNodeManager in DeleteNodeAtBeginning Function is NULL\n");
		return false;
	}

	// setting local variables to NodeManager's NodeCount and NodeArray for readability
	NodeManager->NodeCount -= 1;
	int NodeCount = NodeManager->NodeCount;
	struct Node* NodeArray = NodeManager->NodeArray;

	// Making sure position is within bound
	if (position < 0 || position > NodeCount)
	{
		fprintf(stderr, "\nPosition arguement is out of bounds\n");
		return false;
	}

	// end and beginning edge case handling
	if (position == NodeCount - 1)
	{
		DeleteNodeAtEnd(NodeManager);
		return false;
	}
	else if (position == 0)
	{
		DeleteNodeAtBeginning(NodeManager);
		return false;
	}

	// shifting loop
	for (int i = position; i < NodeCount; i++)
	{
		NodeArray[i] = NodeArray[i + 1];
		NodeArray[i].position = i;
	}

	// shrinking memory
	struct Node* TempNodeArray = realloc(NodeArray, (NodeCount) * sizeof(struct Node));

	// error checking our newly allocated memory
	if (TempNodeArray == NULL)
	{
		perror("Failed to allocate memory for TempNodeArray");
		return false;
	}

	// Updating NodeArray in NodeManager and our copy
	NodeManager->NodeArray = TempNodeArray;
	NodeArray = NodeManager->NodeArray;

	// Updating all nodes in case of new memory block
	for (int i = NodeCount - 1; i >= 0; i--)
	{

		if (i == 0)						// updating head node
		{
			
			NodeArray[i].position = 0;
			NodeArray[i].prev = &NodeArray[NodeCount - 1];
			NodeArray[i].next = &NodeArray[i + 1];
			NodeManager->head = &NodeArray[i];

		}
		else if (i == NodeCount - 1)				// setting the new tail node
		{
			NodeArray[i].next = &NodeArray[0];
			NodeArray[i].prev = &NodeArray[i - 1];
			NodeArray[i].position = i;
			NodeManager->tail = &NodeArray[i];
		}
		else							// updating all other nodes
		{
			NodeArray[i].next = &NodeArray[i + 1];
			NodeArray[i].prev = &NodeArray[i - 1];
			NodeArray[i].position = i;
		}
	}


	return true;
}


bool SetNodeData(struct NodeManager* NodeManager, int position, void* data)
{
	// error checking passed in NodeManager pointer
	if (NodeManager == NULL)
	{
		fprintf(stderr, "\nNodeManager in SetNodeData Function is NULL\n");
		return NULL;
	}


	// setting local variables to NodeManager's NodeCount and NodeArray for readability
	int NodeCount = NodeManager->NodeCount;
	struct Node* NodeArray = NodeManager->NodeArray;

	// error checking position value
	if (position > NodeCount -1 || position < 0)
	{
		fprintf(stderr, "\nPosition arguement is out of bounds\n");
		return false;
	}

	NodeArray[position].data = data;

	// error checking data
	if (data ==  NULL)
	{
		fprintf(stderr, "\nData in SetNodeData Function is NULL\n");
		return false;
	}


	return true;
}


bool EmptyNodeData(struct NodeManager* NodeManager, int position)
{
		// error checking passed in NodeManager pointer
	if (NodeManager == NULL)
	{
		fprintf(stderr, "\nNodeManager in EmptyNodeData Function is NULL\n");
		return NULL;
	}

	// setting local variables to NodeManager's NodeCount and NodeArray for readability
	int NodeCount = NodeManager->NodeCount;
	struct Node* NodeArray = NodeManager->NodeArray;

	// error checking position value
	if (position > NodeCount -1 || position < 0)
	{
		fprintf(stderr, "\nPosition arguement is out of bounds\n");
		return false;
	}

	// emptying the data
	NodeArray[position].data = NULL;

	return true;
}


/* Testing (Feel Free To Delete) */

/*
int main()
{

	struct NodeManager* List1Manager = CreateList(10, "List1");
	if (List1Manager == NULL)
	{
		printf("NULLLLL");
	}
	int meh = 323;
	SetNodeData(List1Manager, 2, &meh);
	printf("Data for 2 is: %d\n", *(int*)List1Manager->NodeArray[2].data);
	struct Node* NewEnd = InsertNodeAtEnd(List1Manager);
	printf("Data for 2 is: %d\n", *(int*)List1Manager->NodeArray[2].data);
	struct Node* NewBeg = InsertNodeAtBeginning(List1Manager);
	printf("Data for 3 is: %d\n", *(int*)List1Manager->NodeArray[3].data);
	struct Node* NewPos = InsertNodeAtPosition(List1Manager, 4);
	printf("Data for 4 is: %d\n", *(int*)List1Manager->NodeArray[3].data);
	printf("Position NewEnd is %d\n", NewEnd->position);
	printf("Position NewBeg is %d\n", NewBeg->position);
	printf("Position NewPos is %d\n", NewPos->position);
	DeleteNodeAtEnd(List1Manager);
	printf("Data for 3 is: %d\n", *(int*)List1Manager->NodeArray[3].data);
	DeleteNodeAtBeginning(List1Manager);
	printf("Data for 2 is: %d\n", *(int*)List1Manager->NodeArray[2].data);
	DeleteNodeAtPosition(List1Manager, 1);
	printf("Data for 1 is: %d\n", *(int*)List1Manager->NodeArray[1].data);
	DeleteNodeAtPosition(List1Manager, 2);
	printf("Data for 1 is: %d\n", *(int*)List1Manager->NodeArray[1].data);
	InsertNodeAtPosition(List1Manager, 0);
	printf("Data for 2 is: %d\n", *(int*)List1Manager->NodeArray[2].data);


	printf("ListID is: %s\n", List1Manager->ListID);	
	printf("NodeCount is: %d\n", List1Manager->NodeCount);
	printf("Head (NodeManager) is: %d\n", List1Manager->head->position);
	printf("Tail (NodeManager) is: %d\n", List1Manager->tail->position);
	printf("Position 0 is %d\n", List1Manager->NodeArray[0].position);
	printf("Position 0 (tail) is %d\n", List1Manager->NodeArray[10].next->position);
	printf("Position 0 (prev) is %d\n", List1Manager->NodeArray[1].prev->position);
	printf("Position 1 is %d\n", List1Manager->NodeArray[1].position);
	printf("Position 1 (next) is %d\n", List1Manager->NodeArray[0].next->position);
	printf("Position 1 (prev) is %d\n", List1Manager->NodeArray[2].prev->position);
	printf("Position 2 is %d\n", List1Manager->NodeArray[2].position);
	printf("Position 2 (next) is %d\n", List1Manager->NodeArray[1].next->position);
	printf("Position 2 (prev) is %d\n", List1Manager->NodeArray[3].prev->position);
	printf("Position 3 is %d\n", List1Manager->NodeArray[3].position);
	printf("Position 3 (next) is %d\n", List1Manager->NodeArray[2].next->position);
	printf("Position 3 (prev) is %d\n", List1Manager->NodeArray[4].prev->position);
	printf("Position 4 is %d\n", List1Manager->NodeArray[4].position);
	printf("Position 4 (next) is %d\n", List1Manager->NodeArray[3].next->position);
	printf("Position 4 (prev) is %d\n", List1Manager->NodeArray[5].prev->position);
	printf("Position 5 is %d\n", List1Manager->NodeArray[5].position);
	printf("Position 5 (next) is %d\n", List1Manager->NodeArray[4].next->position);
	printf("Position 5 (prev) is %d\n", List1Manager->NodeArray[6].prev->position);
	printf("Position 6 is %d\n", List1Manager->NodeArray[6].position);
	printf("Position 6 (next) is %d\n", List1Manager->NodeArray[5].next->position);
	printf("Position 6 (prev) is %d\n", List1Manager->NodeArray[7].prev->position);
	printf("Position 7 is %d\n", List1Manager->NodeArray[7].position);
	printf("Position 7 (next) is %d\n", List1Manager->NodeArray[6].next->position);
	printf("Position 7 (prev) is %d\n", List1Manager->NodeArray[8].prev->position);
	printf("Position 8 is %d\n", List1Manager->NodeArray[8].position);
	printf("Position 8 (next) is %d\n", List1Manager->NodeArray[7].next->position);
	printf("Position 8 (prev) is %d\n", List1Manager->NodeArray[9].prev->position);
	printf("Position 9 is %d\n", List1Manager->NodeArray[9].position);
	printf("Position 9 (next) is %d\n", List1Manager->NodeArray[8].next->position);
	printf("Position 9 (prev) is %d\n", List1Manager->NodeArray[10].prev->position);
	printf("Position 10 is %d\n", List1Manager->NodeArray[10].position);
	printf("Position 10 (next) is %d\n", List1Manager->NodeArray[9].next->position);
	printf("Position 10 (prev) is %d\n", List1Manager->NodeArray[0].prev->position);
	return 0;
}

*/
