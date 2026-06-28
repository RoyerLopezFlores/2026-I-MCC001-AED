// btree.h

#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include <vector>
#include <iterator>
#include <cstddef>
#include <utility>
#include "BTreePage.h"
#include "types.h"
#include "basetrait.h"

#define DEFAULT_BTREE_ORDER 3

template <typename keyType, typename ObjIDType = long, typename Compare = std::less<keyType>>
struct BTreeTraits: public BaseBtreeTraits<keyType, ObjIDType>
{
       using Comp = Compare;
};

template <typename keyType, typename ObjIDType = long>
struct AscendingBTreeTraits: public BaseBtreeTraits<keyType, ObjIDType>, public AscendingTrait<keyType>
{
};

template <typename keyType, typename ObjIDType = long>
struct DescendingBTreeTraits: public BaseBtreeTraits<keyType, ObjIDType>, public DescendingTrait<keyType>
{
};

template <typename keyType, typename ObjIDType = long, typename Compare = std::less<keyType>>
using BTree = BTreeT<BTreeTraits<keyType, ObjIDType, Compare>>;


template <typename Traits>
class BTreeT 
// this is the full version of the BTree
{
       using value_type = typename Traits::value_type;
       using objid_type = typename Traits::objid_type;
       using Node       = typename Traits::Node;
       using Comp       = typename Traits::Comp;
       using BTPage     = CBTreePage<Traits>;
       /*struct Node
       {
               keyType first;
               long    second;
               Node *&operator->() { return this; }
       };*/

public:
       //typedef Node iterator;
       //typedef typename BTNode::Node NodeRaiz;

       class iterator
       {
       public:
              using iterator_category = std::forward_iterator_tag;
              using value_type = Node;
              using difference_type = std::ptrdiff_t;
              using pointer = Node*;
              using reference = Node&;

              iterator(std::vector<Node*>* pNodes, std::size_t index)
                     : m_pNodes(pNodes), m_index(index) {}

              reference operator*() const { return *(*m_pNodes)[m_index]; }
              pointer operator->() const { return (*m_pNodes)[m_index]; }

              iterator& operator++() { ++m_index; return *this; }
              iterator operator++(int) { iterator tmp(*this); ++(*this); return tmp; }

              bool operator==(const iterator& other) const
              {
                     return m_pNodes == other.m_pNodes && m_index == other.m_index;
              }

              bool operator!=(const iterator& other) const
              {
                     return !(*this == other);
              }

       private:
              std::vector<Node*>* m_pNodes;
              std::size_t m_index;
       };

public:
       BTreeT(int order = DEFAULT_BTREE_ORDER, bool unique = true);
       ~BTreeT();
       //int           Open (char * name, int mode);
       //int           Create (char * name, int mode);
       //int           Close ();
       TOBT            Insert (const value_type key, const objid_type ObjID);
       TOBT            Remove (const value_type key, const objid_type ObjID);
       objid_type      Search (const value_type key);
       TOBT            size()  { return m_NumKeys; }
       TOBT            height() { return m_Height;      }
       TOBT            GetOrder() { return m_Order;     }

       void            Print (ostream &os);
       template <typename Func, typename... Args>
       void            ForEach(Func func, Args &&... args);
       iterator        begin();
       iterator        end();

       
       template <typename Func, typename... Args>
       Node*     FirstThat(Func func, Args&&... args);
       //typedef               Node iterator;

protected:
       BTPage        m_Root;
       TOBT             m_Height;  // height of tree
       TOBT             m_Order;   // order of tree
       TOBT            m_NumKeys; // number of keys
       TOBT            m_Unique;  // Accept the elements only once ?

private:
       static void CollectNodeForIteration(Node &info, void *pExtra1);
       void RebuildTraversalCache();
       std::vector<Node*> m_TraversalCache;
       bool m_TraversalCacheDirty;
};



const TOBT MaxHeight = 5; //??
template <typename Traits>
BTreeT<Traits>::BTreeT(int order, bool unique)
                               : m_Unique(unique),
                                 m_Order(order),
                                 m_Root(2 * order  + 1, unique),
                                                                                                                 m_NumKeys(0),
                                                                                                                 m_TraversalCacheDirty(true)
{
       m_Root.SetMaxKeysForChilds(order);
       m_Height = 1;
}

template <typename Traits>
BTreeT<Traits>::~BTreeT()
{
}

template <typename Traits>
TOBT BTreeT<Traits>::Insert(const value_type key, const objid_type ObjID)
{
       bt_ErrorCode error = m_Root.Insert(key, ObjID);
       if( error == bt_duplicate )
               return false;
       m_NumKeys++;
       m_TraversalCacheDirty = true;
       if( error == bt_overflow )
       {
               m_Root.SplitRoot();
               m_Height++;
       }
       return true;
}

template <typename Traits>
TOBT BTreeT<Traits>::Remove (const value_type key, const objid_type ObjID)
{
       bt_ErrorCode error = m_Root.Remove(key, ObjID);
       if( error == bt_duplicate || error == bt_nofound )
               return false;
       m_NumKeys--;
       m_TraversalCacheDirty = true;

       if( error == bt_rootmerged )
               m_Height--;
       return true;
}

template <typename Traits>
typename BTreeT<Traits>::objid_type BTreeT<Traits>::Search (const value_type key)
{
       objid_type ObjID = -1;
       m_Root.Search(key, ObjID);
       return ObjID;
}


template <typename Traits>
template <typename Func, typename... Args>
void BTreeT<Traits>::ForEach(Func func, Args&&... args)
{
       m_Root.ForEach(func, std::forward<Args>(args)...);
}

template <typename Traits>
template <typename Func, typename... Args>
typename BTreeT<Traits>::Node *
BTreeT<Traits>::FirstThat(Func func, Args&&... args)
{
       return m_Root.FirstThat(func, std::forward<Args>(args)...);
}

template <typename Traits>
void BTreeT<Traits>::Print(ostream &os){
       m_Root.Print(os);
}

template <typename Traits>
void BTreeT<Traits>::CollectNodeForIteration(Node &info, void *pExtra1)
{
       std::vector<Node*> *pNodes = static_cast<std::vector<Node*> *>(pExtra1);
       pNodes->push_back(&info);
}

template <typename Traits>
void BTreeT<Traits>::RebuildTraversalCache()
{
       if( !m_TraversalCacheDirty )
               return;

       m_TraversalCache.clear();
       m_TraversalCache.reserve(m_NumKeys);
              m_Root.ForEach(&BTreeT<Traits>::CollectNodeForIteration, &m_TraversalCache);
       m_TraversalCacheDirty = false;
}

template <typename Traits>
typename BTreeT<Traits>::iterator BTreeT<Traits>::begin()
{
       RebuildTraversalCache();
       return iterator(&m_TraversalCache, 0);
}

template <typename Traits>
typename BTreeT<Traits>::iterator BTreeT<Traits>::end()
{
       RebuildTraversalCache();
       return iterator(&m_TraversalCache, m_TraversalCache.size());
}






#endif