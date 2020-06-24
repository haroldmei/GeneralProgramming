#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct Book {
	char title[50];
	char author[50];
	char category[50];
	int price;
	int id;
	struct Book* next;
};

struct List {
	int count;
	struct Book* list;
};

struct List* bookList;

void printBook(struct Book book)
{
	printf("Title : %s\n", book.title);
	printf("Author : %s\n", book.author);
	printf("Category : %s\n", book.category);
	printf("Price : %d\n", book.price);
	printf("Id : %d\n", book.id);

	book.next = NULL;
}

int addBook(struct Book* book)
{
	int index = 0;
	struct Book* list;

	book->next = 0;

	if (bookList == NULL) {
		bookList = (struct List*)malloc(sizeof(struct List));
		bookList->count = 1;
		bookList->list = book;

		return 1;
	}

	list = bookList->list;
	printf("%d", bookList->count);
	for (index = 1; index < bookList->count; index++)
		list = list->next;

	list->next = book;
	//list = book;
	bookList->count++;

	return bookList->count;
}

int main3()
{
	struct Book b;
	struct Book* pB;

	/* First Book */
	strcpy_s(b.title, "Title 1");
	strcpy_s(b.author, "Author 1");
	strcpy_s(b.category, "category 1");
	b.price = 111;
	b.id = 1;

	addBook(&b);

	/* Second Book */
	strcpy_s(b.title, "Title 2");
	strcpy_s(b.author, "Author 2");
	strcpy_s(b.category, "category 2");
	b.price = 222;
	b.id = 2;

	addBook(&b);

	/* Third Book */
	strcpy_s(b.title, "Title 3");
	strcpy_s(b.author, "Author 3");
	strcpy_s(b.category, "category 3");
	b.price = 333;
	b.id = 3;

	addBook(&b);

	pB = bookList->list;

	while (pB != NULL) {
		printBook(*pB);
		pB = pB->next;
	}
	return 0;
}

