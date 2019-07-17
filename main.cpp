#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <string>
#if defined (__linux__)
#include <unistd.h>
#endif
// [1]
void BitPrint(int val)
{
	const int bit_sz = sizeof(val) * 8;
	char str[bit_sz + 1];
	memset(str, 0, sizeof(str));

	for (int i=0; i < bit_sz; ++i) {
		sprintf(&str[i], "%u", (val >> (bit_sz - i - 1)) & 1);
	}
	printf("%s\n", str);
}
// [2]
void RemoveDups(char *pStr)
{
	if ( !pStr ) {
		return;
	}
	const char* s = pStr;
	auto size = strlen(pStr);
	char* res = static_cast<char*>(malloc(size));
	char* p = res;
	char l = 0;

	while ( *s != '\0' && *s != '\n' ) {
		if (l != *s) {
			l = *s;
			*p = *s;
			++p;
		} else if (l == 0) {
			l = *s;
			*p = *s;
			++p;
		}
		++s;
	}
	*p = 0;

	const auto res_size = strlen(res);
	memcpy(pStr, res, res_size);
	memset(pStr + strlen(res), 0, size - res_size);
	free(res);
}
// [3]
struct ListNode {
	ListNode	*prev;
	ListNode	*next;
	ListNode	*rand; // указатель на произвольный элемент данного списка либо NULL
	std::string	data;
};

class List {
public:
	List();
	~List();

	void Serialize(FILE *file); // сохранение в файл (файл открыт с помощью fopen(path, "wb"))
	void Deserialize(FILE *file); // загрузка из файла (файл открыт с помощью fopen(path, "rb"))

	ListNode* Append(const std::string& val);
	void Link(ListNode *from, ListNode *to);
	void clear();

private:
	ListNode	*head;
	ListNode	*tail;
	int			count;
};

List::List():
	head(nullptr), tail(nullptr), count(0)
{
}

List::~List()
{
	clear();
}

int GetPos(ListNode* head, ListNode* it)
{
	int pos = 0;
	ListNode* node = head;
	while ( node ) {
		if ( node == it ) {
			return pos;
		}
		node = node->next;
		++pos;
	}
	return -1;
}

ListNode* FromPos(ListNode* head, int pos)
{
	int it = 0;
	ListNode* node = head;
	while ( node ) {
		if ( it == pos ) {
			return node;
		}
		node = node->next;
		++it;
	}
	return nullptr;
}

void List::Serialize(FILE* file)
{
	if ( !file || !count ) {
		return;
	}

	const ListNode* node = head;
	int data_sz, rand, rand_sz = 0;

	fwrite(&count, sizeof(count), 1, file);
	fwrite(&rand_sz, sizeof(rand_sz), 1, file);

	while ( node ) {
		data_sz = node->data.size();
		fwrite(&data_sz, sizeof(data_sz), 1, file);
		fwrite(node->data.c_str(), 1, data_sz, file);

		rand = -1;
		if ( node->rand ) {
			rand = GetPos(head, node->rand);
		}
		if (rand != -1) {
			++rand_sz;
		}

		fwrite(&rand, sizeof(rand), 1, file);
		node = node->next;
	}

	fseek(file, sizeof(count), SEEK_SET);
	fwrite(&rand_sz, sizeof(rand_sz), 1, file);
}

void List::Deserialize(FILE* file)
{
	if ( !file ) {
		return;
	}

	int data_sz, rand, rand_sz = 0;
	int count_;
	size_t rdbytes = 0;

	rdbytes = fread(&count_, sizeof(char), sizeof(count), file);
	if ( !rdbytes ) {
		return;
	}
	rdbytes = fread(&rand_sz, sizeof(char), sizeof(rand_sz), file);
	if ( !rdbytes ) {
		return;
	}
	struct link_t {
		ListNode* orig;
		int rand_pos;
	};
	struct link_t* links = new struct link_t[rand_sz];
	struct link_t* link = links;
	std::string strval;

	while ( !feof(file) ) {
		rdbytes = fread(&data_sz, sizeof(char), sizeof(data_sz), file);

		if ( !rdbytes ) {
			break;
		}
		strval.resize(data_sz);
		rdbytes = fread(&strval[0], sizeof(char), data_sz, file);
		if ( !rdbytes ) {
			break;
		}
		ListNode* it = Append(strval);
		rdbytes = fread(&rand, sizeof(char), sizeof(rand), file);
		if ( !rdbytes ) {
			break;
		}
		if ( rand != -1 ) {
			link->orig = it;
			link->rand_pos = rand;
			++link;
		}
	}
	assert(count_ == count);
	
	link = links;
	rand = rand_sz;

	while ( rand ) {
		link->orig->rand = FromPos(head, link->rand_pos);
		++link;
		--rand;
	}
	delete[] links;
}

ListNode* List::Append(const std::string& val)
{
	ListNode* node = new ListNode;
	node->prev = nullptr;
	node->next = nullptr;
	node->rand = nullptr;
	node->data = val;

	if ( !count ) {
		head = node;
		tail = head;
	} else {
		ListNode* last = tail;
		node->prev = last;
		last->next = node;
		tail = node;
		tail->next = nullptr;
	}
	++count;
	return node;
}

void List::Link(ListNode* from, ListNode* to)
{
	if ( !from || !to ) {
		return;
	}
	from->rand = to;
}

void List::clear()
{
	ListNode *node = head, *next = node;
	while ( next ) {
		next = node->next;

		node->next = nullptr;
		node->prev = nullptr;
		node->rand = nullptr;
		node->data.clear();
		delete node;

		node = next;
	}
	head = tail = nullptr;
	count = 0;
}
// [0]
int main()
{
#if defined (__linux__)
	void* p_before = sbrk(0);
#endif
	{
		printf("Hello, World!\n");
		// [1]
		int val = static_cast<int>(-1e4);
		BitPrint(val);
		// [2]
		char str[] = "HEEEEEELLLLO WWWORRRRRRLLLDDDDDD!!!!!";
		printf("source str: %s\n", str);
		RemoveDups(str);
		printf("result str: %s\n", str);
		// [3]
		FILE* pfile;
		List list;

		std::string strval = "A1";
		ListNode* it1 = list.Append(strval);
		strval = "B22";
		ListNode* it2 = list.Append(strval);
		strval = "C333";
		ListNode* it3 = list.Append(strval);
		strval = "D4444";
		ListNode* it4 = list.Append(strval);
		strval = "E55555";
		ListNode* it5 = list.Append(strval);

		list.Link(it2, it4);
		list.Link(it5, it2);
		list.Link(it3, it5);
		list.Link(it4, it1);

		// write list
		pfile = fopen("./list.bin", "wb");
		list.Serialize(pfile);
		fclose(pfile);

		list.clear();

		// load list
		pfile = fopen("./list.bin", "rb");
		list.Deserialize(pfile);
		fclose(pfile);
	}
#if defined (__linux__)
	void* p_after = sbrk(0);
	printf("\n\tmemory before:\t%lu\n", (uint64_t)p_before);
	printf("\tmemory after:\t%lu\n\n", (uint64_t)p_after);
#endif
	return 0;
}

