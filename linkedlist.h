#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include <iostream>
#include <cstddef>   // size_t
#include <string>
#include <sstream>
#include <mutex>     // mutex
#include "general_iterator.h"
#include "util.h"
#include "types.h"
#include "foreach.h"
#include "basetrait.h"
// #include <shared_mutex>
using namespace std;

// Forward iterator
template <typename Container>
class LinkedListForwardIterator : public general_iterator<Container, 
                                            LinkedListForwardIterator<Container>>{
    using MySelf = LinkedListForwardIterator<Container>;
    using Parent = general_iterator<Container, MySelf>;
    using Parent::Parent;
public:
    // TODO: Completar el operator++
    MySelf& operator++(){
        this->m_pNode = this->m_pNode->getNext();
        return *this;
    }
};

// Linked List Node
template <typename T>
class LLNode{
    using value_type = T;
    using Node       = LLNode<T>;
private:
    value_type m_data;
    Ref        m_ref;
    Node      *m_pNext;
public:
    LLNode(){}
    LLNode(value_type data, Ref ref, Node *pNext = nullptr) 
           : m_data(data), m_ref(ref), m_pNext(pNext) {}
    virtual ~LLNode() {}

    value_type      getData() const { return m_data; }
    value_type&     getDataRef()    { return m_data; }
    void            setData(value_type data) { m_data = data; }
    Ref             getRef() const  { return m_ref; }
    Ref&            getRefRef()     { return m_ref; }
    void            setRef(Ref ref) { m_ref = ref; }
    Node*           getNext() const { return m_pNext; }
    Node*&          getNextRef()    { return m_pNext; }
    void            setNext(Node *pNext) { m_pNext = pNext; }
};

template <typename T>
ostream &operator<<(ostream &os, const LLNode<T> &node){
    return os << "(" <<node.getData() << ", " << node.getRef() << ")";
}
template <typename T>
istream &operator>>(istream &is, LLNode<T> &node){
    
    char ch1, ch2, ch3;
    T a;
    Ref b;
    if (is >> ch1 && ch1 == '(' &&
        is >> a &&
        is >> ch2 && ch2 == ',' &&
        is >> b &&
        is >> ch3 && ch3 == ')') {
        node.setData(a);
        node.setRef(b);

        return is;
    }
    is.setstate(ios::failbit);
    return is;
}


template <typename T>
struct BaseLinkedListTrait : public BaseContainerTrait<T, LLNode<T>>{

};

template <typename T>
struct AscendingLinkedListTrait : public BaseLinkedListTrait<T>{
    using Comp = less<T>;
};

template <typename T>
struct DescendingLinkedListTrait : public BaseLinkedListTrait<T>{
    using Comp = greater<T>;
};

template <typename Traits>
class LinkedList{
public:
    using value_type = typename Traits::value_type;
    using Node       = typename Traits::Node;
    using Comp       = typename Traits::Comp;
    using MySelf     = LinkedList<Traits>;

    using forward_iterator = LinkedListForwardIterator<MySelf>;
    // friend forward_iterator;

private:
    Node *m_pRoot = nullptr;
    Node *m_pTail = nullptr;
    size_t m_size = 0;
    Comp   m_comp;
    mutable mutex m_mtx;
    // mutable shared_mutex m_mtx;
public:
    LinkedList() {}
    LinkedList(const LinkedList &other)  { // Copy constructor
        lock_guard<mutex> lock(other.m_mtx);
        // shared_lock<shared_mutex> lock(other.m_mtx);
        init();
        m_comp = other.m_comp;
        copy_nodes(other);
    }
    LinkedList(LinkedList &&other) noexcept { // Move constructor
        // con pointers raw no es necesario usar move
        unique_lock<mutex> lock(other.m_mtx);
        // unique_lock<shared_mutex> lock(other.m_mtx);
        m_comp = move(other.m_comp);
        steal_nodes(other);
        other.init();
        
    }
    LinkedList& operator=(const LinkedList &other) { // Copy assignment operator
        if(this == &other){
            return *this;
        }
        scoped_lock lock(m_mtx, other.m_mtx);
        clean_unsafe();
        m_comp = other.m_comp;
        copy_nodes(other);
        return *this;
    }
    LinkedList& operator=(LinkedList &&other) noexcept{ // Move assignment operator
        if(this == &other){
            return *this;
        }
        scoped_lock lock(m_mtx, other.m_mtx);
        
        clean_unsafe();
        m_comp = move(other.m_comp);
        steal_nodes(other);
        other.init();
        return *this;

    }
    
    // Limpia
    void clean(){
        unique_lock<mutex> lock(m_mtx);
        // unique_lock<shared_mutex> lock(m_mtx);
        clean_unsafe();
    }
    // Copiar nodos
    
    
    virtual        ~LinkedList() {
        clean_unsafe();
    }
    //pop front()
    virtual void    push_front(value_type value, Ref ref){
        unique_lock<mutex> lock(m_mtx);
        // unique_lock<shared_mutex> lock(m_mtx);
        Node* newNode = new Node(value, ref, m_pRoot);
        m_pRoot = newNode;
        ++m_size;
        if(m_pTail == nullptr)
            m_pTail = m_pRoot;
        
    }
    virtual auto    pop_front() -> std::optional<std::pair<value_type, Ref>>{
        unique_lock<mutex> lock(m_mtx);
        // unique_lock<shared_mutex> lock(m_mtx); 
        if (!m_pTail)
            return std::nullopt;
        
        Node* pTemp = m_pRoot;
        m_pRoot = m_pRoot->getNext();
        if (m_pRoot == nullptr)
            m_pTail = nullptr;
        --m_size;

        auto result = std::make_pair(pTemp->getData(), pTemp->getRef());
        delete pTemp;
        return result;
    }  
        //else
            //throw std::out_of_range("pop_front(): empty list");
        //    return nullopt;
        //}
    //push back()
    virtual void    push_back(value_type value, Ref ref){
        unique_lock<mutex> lock(m_mtx);
        // unique_lock<shared_mutex> lock(m_mtx);
        push_back_unsafe(value, ref);
    }
    //pop_back()
    virtual auto    pop_back() -> std::optional<std::pair<value_type, Ref>>{
        unique_lock<mutex> lock(m_mtx);
        // unique_lock<shared_mutex> lock(m_mtx);
        if(!m_pTail)
            return std::nullopt;
        
        Node* pTemp = m_pTail;
        if(m_pRoot == m_pTail){
            m_pRoot = nullptr;
            m_pTail = nullptr;
        }else{
            Node* pNode = m_pRoot;
            while(pNode->getNext() != m_pTail)
                pNode = pNode->getNext();
            pNode->setNext(nullptr);
            m_pTail = pNode;
        }
        --m_size;
        return std::make_pair(pTemp->getData(), pTemp->getRef());
        
    }
    value_type operator[](size_t index){
        lock_guard<mutex> lock(m_mtx);
        // shared_lock<shared_mutex> lock(m_mtx);
        if( index >= size_unsafe() )
            throw std::out_of_range("operator[]: index out of range");
        Node* pNode = m_pRoot;
        for( size_t i = 0 ; i < index ; ++i )
            pNode = pNode->getNext();
        
        return pNode->getDataRef();
    }
private:
    void internal_insert(Node* &pParent, const value_type &value, Ref ref);
    void init(){
        m_pRoot = nullptr;
        m_pTail = nullptr;
        m_size  = 0;
    }
    void clean_unsafe(){
        Node *pNode = m_pRoot;
        while(pNode){
            Node *pTemp = pNode;
            pNode = pNode->getNext();
            delete pTemp;
        }
        init();
    }
    void steal_nodes(LinkedList &other){
        m_pRoot = other.m_pRoot;
        m_pTail = other.m_pTail;
        m_size  = other.m_size;
    }
    void copy_nodes(const LinkedList &other){
        Node* pNode = other.m_pRoot;
        while(pNode){
            push_back_unsafe(pNode->getData(), pNode->getRef());
            
            pNode = pNode->getNext();
        }
    }
    void push_back_unsafe(value_type value, Ref ref){
        Node* newNode = new Node(value, ref, nullptr);
        if(m_pTail)
            m_pTail->setNext(newNode);
        else
            m_pRoot = newNode;
        m_pTail = newNode;
        ++m_size;
    }
    size_t size_unsafe() const { return m_size; }
    public:
    
    virtual void    insert(const value_type &value, Ref ref);
    
    // virtual Node& operator[](size_t index);
    virtual size_t  size() const { 
        //Usar atomic pero tambien se tiene que modificar en las otras funciones
        //std::atomic<size_t> m_size;
        lock_guard<mutex> lock(m_mtx);
        // shared_lock<shared_mutex> lock(m_mtx);
        return m_size; 
    }

    virtual string  toString();

    forward_iterator begin() { return forward_iterator(this, m_pRoot); }
    forward_iterator end()   { return forward_iterator(this, nullptr); }

    

    // Agregar Foreach
    template <typename Func, typename... Args>
    void ForEach(Func func, Args &&...  args){
        lock_guard<mutex> lock(m_mtx);
        // shared_lock<shared_mutex> lock(m_mtx);
        ::ForEach(begin(), end(), func, std::forward<Args>(args)... );
    }
    template <typename Func, typename... Args>
    forward_iterator FirstThat(Func func, Args &&... args){
        lock_guard<mutex> lock(m_mtx);
        // shared_lock<shared_mutex> lock(m_mtx);
        return ::FirstThat(begin(), end(), func, std::forward<Args>(args)... );
    }
};

template <typename Traits>
void LinkedList<Traits>::internal_insert(Node* &pPrev, const value_type &value, Ref ref){
    if(!pPrev || m_comp(value, pPrev->getDataRef())){
        Node* newNode = new Node(value, ref, pPrev);
        pPrev = newNode;
        ++m_size;
        
        //if(pPrev == m_pRoot)
        //    m_pTail = pPrev;
        if(newNode->getNext() == nullptr ){
            m_pTail = newNode;
        }   
        
        return;
    }
    internal_insert(pPrev->getNextRef(), value, ref);
}

template <typename Traits>
void LinkedList<Traits>::insert(const value_type &value, Ref ref){
    unique_lock<mutex> lock(m_mtx);
    // unique_lock<shared_mutex> lock(m_mtx);
    internal_insert(m_pRoot, value, ref);
}

template <typename Traits>
string  LinkedList<Traits>::toString() {
    lock_guard<mutex> lock(m_mtx);
    // shared_lock<shared_mutex> lock(m_mtx);
    stringstream ss;
    Node *pNode = m_pRoot;
    ss << "[";
    if( m_size > 0 ){
        for( size_t i = 0 ; i < size_unsafe()-1 ; ++i ){
            ss << *pNode << ",";
            pNode = pNode->getNext();
        }
        ss << *pNode;
    }
    ss << "]";
    return ss.str();
}

template <typename Traits>
ostream& operator<<(ostream& os, LinkedList<Traits>& list){
    return os << list.toString();
}
template <typename Traits>
istream& operator>>(istream& is, LinkedList<Traits>& list){
    char ch;
    if(!(is >> ch) || ch != '['){
        is.setstate(ios::failbit);
        return is;
    }
    
    // haremos con insert
    while (true){
        typename LinkedList<Traits>::Node node;
        
        is>>node;

        if (!is) return is;

        list.insert(node.getData(), node.getRef());

        is >> ch;

        if(ch == ']') break;
        if (ch != ',') {         
            is.setstate(ios::failbit);
            return is;
        }
    }


    return is;
}

#endif // __LINKEDLIST_H__