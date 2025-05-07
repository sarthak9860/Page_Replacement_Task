#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PAGES 1000  // maximum possible unique pages

typedef struct Node {
	int page;
	struct Node *next;
} Node;

typedef struct {
	Node *front;
	Node *rear;
	int size;
} Queue;

Queue* createQueue() {
	Queue *q = (Queue*)malloc(sizeof(Queue));
	q->front = q->rear = NULL;
	q->size = 0;
	return q;
}

void enqueue(Queue *q, int page) {
	Node *newNode = (Node*)malloc(sizeof(Node));
	newNode->page = page;
	newNode->next = NULL;

	if (q->rear) q->rear->next = newNode;
	q->rear = newNode;

	if (!q->front) q->front = newNode;
	q->size++;
}

int dequeue(Queue *q) {
	if (!q->front) return -1;

	Node *temp = q->front;
	int page = temp->page;

	q->front = q->front->next;
	if (!q->front) q->rear = NULL;

	free(temp);
	q->size--;
	return page;
}

int isInQueue(Queue *q, int page) {
	Node *cur = q->front;
	while (cur) {
		if (cur->page == page)
			return 1;
		cur = cur->next;
	}
	return 0;
}

void freeQueue(Queue *q) {
	while (q->front)
		dequeue(q);
	free(q);
}

int simulateFIFO(const char *filename, int total_frames) {
	FILE *fp = fopen(filename, "r");
	if (!fp) {
		perror("File open failed");
		exit(EXIT_FAILURE);
	}

	Queue *memory = createQueue();
	int pageFaults = 0;

	int page;
	while (fscanf(fp, "%d", &page) == 1) {
		if (!isInQueue(memory, page)) {
			pageFaults++;
			if (memory->size >= total_frames) {
				dequeue(memory);
			}
			enqueue(memory, page);
		}
	}

	fclose(fp);
	freeQueue(memory);
	return pageFaults;
}

int main(int argc, char *argv[]) {
	if (argc != 5) {
		fprintf(stderr, "Usage: %s <total_pages> <total_frames> <swap_blocks> <filename>\n", argv[0]);
		return EXIT_FAILURE;
	}

	int total_pages = atoi(argv[1]);
	int total_frames = atoi(argv[2]);
	int swap_blocks = atoi(argv[3]);
	const char *filename = argv[4];

	if (total_frames <= 0 || total_pages <= 0 || swap_blocks <= 0) {
		fprintf(stderr, "All values must be positive integers.\n");
		return EXIT_FAILURE;
	}

	int pageFaults = simulateFIFO(filename, total_frames);
	printf("Total Page Faults: %d for %d frames\n", pageFaults, total_frames);

	return EXIT_SUCCESS;
}
