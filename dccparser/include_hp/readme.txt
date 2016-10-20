将/opt/aCC/include_std/rw/stdex
hashmap.h hashmmap.h文件的
template <class K, class V, class Hash, class EQ, class A>
改成
template <class K, class V, class Hash, class EQ, class A = = std::allocator< pair<int,int>> >
