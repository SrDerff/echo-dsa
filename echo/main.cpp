#include <iostream>
#include <string>
#include <cassert>
#include "HashTable.h"
using namespace std;

int main() {
    HashTable<string, int> ht;

    assert(ht.isEmpty());
    assert(ht.getSize() == 0);

    ht.insert("uno", 1);
    ht.insert("dos", 2);
    ht.insert("tres", 3);

    assert(ht.getSize() == 3);
    assert(ht.contains("uno"));
    assert(ht.contains("dos"));
    assert(ht.contains("tres"));
    assert(!ht.contains("cuatro"));

    auto val = ht.getElement("dos");
    assert(val.has_value());
    assert(val->second == 2);

    assert(ht.remove("dos"));
    assert(!ht.contains("dos"));
    assert(ht.getSize() == 2);

    ht.insert("dos", 22);
    assert(ht.contains("dos"));
    assert(ht.getElement("dos")->second == 22);

    // update existing
    ht.insert("uno", 100);
    assert(ht.getElement("uno")->second == 100);

    // test rehash
    HashTable<int, int> ht2;
    for (int i = 0; i < 200; ++i)
        ht2.insert(i, i * 10);

    assert(ht2.getSize() == 200);
    for (int i = 0; i < 200; ++i)
        assert(ht2.contains(i));

    ht2.clear();
    assert(ht2.isEmpty());
    assert(ht2.getSize() == 0);

    cout << "All tests passed!" << endl;
    return 0;
}
