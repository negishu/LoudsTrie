#include ".\\Trie\\loudsTrie.h"

int build_main(int argc, char **argv)
{
    if (argc < 3) {
        return 0;
    }

    LoudsTrie trie;
    trie.read(argv[1],10);
    trie.save(argv[2]);

    cout << "labels.size(): " << trie.labels.size() << endl;
    cout << "louds.getSize(): " << trie.louds.getSize() << endl;
    cout << "terminal.getSize(): " << trie.terminal.getSize() << endl;
    cout << "Total size: " << trie.size() << endl;

    return 0;
}

int lookup_main(int argc, char **argv)
{
    if (argc < 2) {
        return 0;
    }

    LoudsTrie trie;
    trie.load(argv[1]);

    string query;
    while (cin >> query) {
        uint32_t result = trie.lookup(query);
        cout << result << endl;
    }
    
    return 0;
}

int traverse_main(int argc, char **argv)
{
    if (argc < 2) {
        return 0;
    }
    int times = argc > 2 ? atoi(argv[2]) : 1;

    LoudsTrie trie;
    trie.load(argv[1]);

    for (int i = 0; i < times; ++i) {
        trie.traverse();
    }

    return 0;
}

int main(int argc, char **argv)
{
    {
        char* argvalues[] = { "this", "wordlist.txt", "data.out" };
        build_main(sizeof(argvalues)/sizeof(char*), argvalues);
    }
    {
        char* argvalues[] = { "this", "data.out" };
        lookup_main(sizeof(argvalues) / sizeof(char*), argvalues);
    }
    {
        char* argvalues[] = { "this", "data.out" };
        traverse_main(sizeof(argvalues) / sizeof(char*), argvalues);
    }

    return 0;
}

